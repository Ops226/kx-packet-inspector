# System Architecture: Network Message Processing

**Status:** Confirmed (Dynamic Analysis Backed & Unified)

## High-Level Model

The Guild Wars 2 client processes network messages through a **dynamic, multi-stage dispatch system** for both incoming (SMSG) and outgoing (CMSG) traffic. This architecture leverages runtime-resolved function pointers and schema-driven serialization/deserialization.

The unified process is as follows:

1.  **Incoming (SMSG) - Framing & Initial Processing:** All incoming game server traffic is handled by `Msg::DispatchStream`. This function reads the raw byte stream, identifies individual message frames, and prepares them for parsing.
2.  **Incoming (SMSG) - Schema-Driven Parsing:** For each message, `Msg::DispatchStream` calls the schema virtual machine, `MsgUnpack::ParseWithSchema`. This function uses an opcode-specific schema to transform raw bytes into a structured data tuple.
3.  **Incoming (SMSG) - Dynamic Handler Dispatch:** After parsing, `Msg::DispatchStream` retrieves a dynamically resolved function pointer (e.g., from `[[MsgConn+0x48]+0x18]`) and calls it, passing the parsed data tuple. There is **no single, master switch statement**; the handler can vary based on connection context or message content.
4.  **Outgoing (CMSG) - Game Logic Initiates:** A high-level game logic function (e.g., character movement, skill usage) initiates the process by preparing data for a packet.
5.  **Outgoing (CMSG) - Packet Building:** The game logic calls `MsgConn::BuildPacketFromSchema`, providing the packet's schema and the data to be serialized. This function internally uses `Msg::MsgPack` (the CMSG counterpart to the SMSG parser) to write the data into a packet buffer.
6.  **Outgoing (CMSG) - Queueing for Send:** The completed packet buffer is then passed to a queueing function (e.g., `MsgConn::QueuePacket`), which places it in a thread-safe queue to be sent by the network thread.

## Key Components

### 1. The Server Message Dispatcher (`Msg::DispatchStream`) (see `raw_decompilations/smsg/Msg::DispatchStream.c`)
*   **Role:** This is the primary orchestrator for incoming game server messages. It manages the message stream, calls the parser for each message, and executes the **currently assigned dynamic post-parse handler function**.

### 2. The Schema-Driven Parser (`MsgUnpack::ParseWithSchema`) (see `raw_decompilations/common/MsgUnpack::ParseWithSchema.c`)
*   **Role:** This function acts as a virtual machine that interprets a schema to parse a raw byte buffer into a typed argument tuple. Its role is central to understanding both incoming (SMSG) and outgoing (CMSG) packet structures.
*   **Schema Typecodes:** The schema is defined by a series of typecodes. Key typecodes include:
    *   `0x02`: `u8`
    *   `0x0c`: Composite type (struct-like, inline fields)
    *   `0x11`: Array with `u8` count.
    *   `0x12`: Array with `u16` count.
    *   `0x13`: Fixed-size byte array.
    *   `0x14`: Byte array with `u8` length.
    *   `0x15`: Byte array with `u16` length.
    *   `0x16`: Untyped (raw bytes, for opaque payloads)
    *   `0x17`: `u32`

### 3. Dynamic Post-Parse Handlers (SMSG)

*   **Role:** These are the functions that receive the structured data tuple from `MsgUnpack::ParseWithSchema`. The client dynamically loads which handler is active.
*   **Discovery:** Identified by dynamic analysis (e.g., observing the `RAX` register at `"Gw2-64.exe"+FD1ACD` within `Msg::DispatchStream`).
*   **Example:** `FUN_1413e5d90` (renamed `Marker::Cli::ProcessAgentMarkerUpdate`). This function handles packets whose parsed tuple begins with `0x03FE`.
*   **`Gs2c_PostParseDispatcher` (see `raw_decompilations/smsg/Gs2c_PostParseDispatcher.c`):** This is understood to be **one of many possible dynamic post-parse handlers**. It is not the universal destination for all packets and handles a specific subset of application-level messages.

### 4. Outgoing Packet Builder (`MsgConn::BuildPacketFromSchema`)
*   **Role:** This is the primary utility function for constructing outgoing packets. It takes a schema and raw data, then serializes the data into a buffer.
*   **Internal Call:** It internally uses `FUN_140fd2c70` (Proposed Name: `Msg::MsgPack`) which acts as the CMSG-specific schema serialization engine.

### 5. Outgoing Packet Queueing (`MsgConn::QueuePacket`)
*   **Role:** Takes a fully constructed packet buffer and its opcode, and places it into a thread-safe queue.
*   **Internal Call:** It calls `FUN_141051690` (Proposed Name: `MsgConn::EnqueuePacket`) which performs the low-level queueing operations.

## Packet Flow Examples (Confirmed via Dynamic Analysis)

### Incoming (SMSG) Example: Message with `FE 03` Prefix

The handling of a message whose parsed tuple begins with `FE 03` illustrates the dynamic incoming process:

1.  **Dispatch:** `Msg::DispatchStream` receives the raw bytes for an unknown message.
2.  **Parsing:** It calls `MsgUnpack::ParseWithSchema` (`call "Gw2-64.exe"+FD43C0`). The parser uses the appropriate schema and creates a data tuple (e.g., `FE 03 2F 01 ...`).
3.  **Dynamic Handling:** `Msg::DispatchStream` retrieves a function pointer from its dispatch object (`mov rax,[rcx+18]`). In this live context, the pointer is the address of `FUN_1413e5d90` (`Marker::Cli::ProcessAgentMarkerUpdate`).
4.  **Execution:** The dispatcher calls the pointer (`call rax` at `"Gw2-64.exe"+FD1ACD`). `Marker::Cli::ProcessAgentMarkerUpdate` then executes, reading fields from the data tuple to carry out its logic.

### Outgoing (CMSG) Example: Agent Link (`0x0036`)

This illustrates the CMSG construction and sending process:

1.  **Game Logic:** A high-level game logic function (e.g., `FUN_140245390`, proposed `CMSG::BuildAgentLink`) is called to prepare an Agent Link packet.
2.  **Packet Building:** It calls `MsgConn::BuildPacketFromSchema` (`FUN_140fd4c10`), providing the schema `DAT_142513080` and the packet data.
3.  **Serialization:** `MsgConn::BuildPacketFromSchema` uses `Msg::MsgPack` (`FUN_140fd2c70`) to serialize the data into the internal packet buffer.
4.  **Queueing:** The constructed packet is passed to `MsgConn::QueuePacket` (`FUN_14104d760`) along with its opcode `0x0036`.
5.  **Sending:** `MsgConn::QueuePacket` then calls `MsgConn::EnqueuePacket` (`FUN_141051690`) to place the packet in the send queue.

## Implications for Reverse Engineering

*   **Dynamic Analysis is Required for Handlers:** Static analysis alone is insufficient. The only reliable way to find the correct SMSG handler for a specific packet is to use a debugger (like Cheat Engine) to trace execution and identify which post-parse function is actually called.
*   **CMSG Discovery is Reference-Based:** CMSG packets are discovered by finding cross-references to `MsgConn::BuildPacketFromSchema`.
*   **The Schema is Still King:** For both SMSG and CMSG, the schema remains the authoritative source for the packet's on-wire structure. However, discovering which schema applies to a given packet requires tracing.
*   **The System is Highly Context-Driven:** The client's ability to dynamically swap handlers and build packets via centralized logic means the meaning and processing of packets can change depending on the game's state.

---