# System Architecture: Network Message Processing

**Status:** Confirmed (Dynamic Analysis Backed)

## High-Level Model

The Guild Wars 2 client processes incoming server messages (SMSG) through a **dynamic, multi-stage dispatch system**. Previous analysis suggested a single, static post-parse dispatcher, but this has been proven incorrect. The authoritative flow relies on function pointers that are resolved at runtime.

The confirmed process is as follows:

1.  **Framing & Initial Processing:** All incoming game server traffic is handled by `Gs2c_SrvMsgDispatcher`. This function is a loop that reads the raw byte stream, identifies individual message frames, and prepares them for parsing.

2.  **Schema-Driven Parsing:** For each message, `Gs2c_SrvMsgDispatcher` calls the schema virtual machine, `Msg_ParseAndDispatch_BuildArgs`. This function uses an opcode-specific schema (e.g., `&DAT_14251xxxx`) to transform the raw bytes into a structured, typed data tuple in temporary memory.

3.  **Dynamic Handler Dispatch:** This is the critical step. After a message is successfully parsed, `Gs2c_SrvMsgDispatcher` retrieves a function pointer from a runtime object (`[[MsgConn+48]+18]`). It then **calls this dynamic function pointer**, passing it a pointer to the newly created data tuple.

This means there is **no single, master switch statement**. The post-parse handler function can be different depending on the game's state or connection context.

## Key Components

### 1. The Server Message Dispatcher (`Gs2c_SrvMsgDispatcher`)

*   **Function:** `Gs2c_SrvMsgDispatcher` (see `raw_decompilations/Gs2c_SrvMsgDispatcher.c`)
*   **Role:** This is the primary orchestrator for game server messages. It manages the message stream, calls the parser for each message, and executes the **currently assigned post-parse handler function**.

### 2. The Schema-Driven Parser (`Msg_ParseAndDispatch_BuildArgs`)

*   **Function:** `Msg_ParseAndDispatch_BuildArgs` (see `raw_decompilations/Msg_ParseAndDispatch_BuildArgs.c`)
*   **Role:** This function acts as a virtual machine that interprets a schema to parse a raw byte buffer into a typed argument tuple. Its role remains central to understanding packet structures.
*   **Schema Typecodes:** The schema is defined by a series of typecodes. Key typecodes include:
    *   `0x02`: `u8`
    *   `0x11`: Array with `u8` count.
    *   `0x12`: Array with `u16` count.
    *   `0x14`: Byte array with `u8` length.
    *   `0x15`: Byte array with `u16` length.
    *   `0x17`: `u32`

### 3. Dynamic Post-Parse Handlers

*   **Role:** These are the functions that receive the structured data tuple from the parser. The client can swap which handler is active.
*   **Example (Discovered via Dynamic Analysis):** `FUN_1413e5d90` (Proposed Name: `SMSG_PostParse_Handler_Type03FE`). This function was observed handling a packet whose parsed tuple began with the value `0x03FE`.
*   **`Gs2c_PostParseDispatcher`:** This is now understood to be just **one of many possible post-parse handlers**. It is not the universal destination for all packets. It appears to handle a specific subset of application-level messages, but it is not the primary network message router.

## Packet Flow Example (Confirmed via Dynamic Analysis)

The handling of a message whose parsed tuple begins with `FE 03` illustrates the true process:

1.  **Dispatch:** `Gs2c_SrvMsgDispatcher` receives the raw bytes for an unknown message.
2.  **Parsing:** It calls `Msg_ParseAndDispatch_BuildArgs` (`call "Gw2-64.exe"+FD43C0`). The parser uses the appropriate schema and creates a data tuple in memory. The first bytes of this tuple are `FE 03 2F 01 ...`. The pointer to this tuple is passed to the next stage.
3.  **Dynamic Handling:** `Gs2c_SrvMsgDispatcher` retrieves a function pointer from its dispatch object (`mov rax,[rcx+18]`). In this live context, the pointer is the address of `FUN_1413e5d90`.
4.  **Execution:** The dispatcher calls the pointer (`call rax` at `"Gw2-64.exe"+FD1ACD`). `FUN_1413e5d90` then executes, reading fields from the data tuple to carry out its logic.

## Implications for Reverse Engineering

*   **Dynamic Analysis is Required:** Static analysis alone is insufficient. The only reliable way to find the correct handler for a specific packet is to use a debugger (like Cheat Engine) to trace execution and identify which post-parse function is actually called.
*   **The Schema is Still King, But Harder to Find:** Once the true handler is identified via dynamic analysis, you can then analyze it statically to find the `CALL` to the schema parser and retrieve the schema address. The schema remains the authoritative source for the packet's structure.
*   **The System is Context-Driven:** The client's ability to swap post-parse handlers means the meaning and processing of packets can change depending on what the player is doing in the game.