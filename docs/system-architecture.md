# System Architecture: Network Message Processing

**Status:** Consolidated and Confirmed (Code-Backed)

## High-Level Model

The Guild Wars 2 client processes incoming server messages (SMSG) through a sophisticated, schema-driven dispatch system. This architecture avoids traditional polymorphic vtable hierarchies for message handling. Instead, it relies on a system of registered callbacks and per-opcode schemas to decode and route messages.

The process can be summarized as follows:

1.  **Dispatch:** An incoming message is received by a central dispatcher, often referred to as `SrvMsgDispatcher` or `Msg::DispatchStream`.
2.  **Parsing:** The dispatcher uses a schema virtual machine, implemented in `Msg_ParseAndDispatch_BuildArgs`, to parse the raw byte stream into a structured, typed tuple of arguments. Each opcode has an associated schema that defines its structure.
3.  **Handling:** The dispatcher then invokes a registered handler for the given opcode, passing the parsed argument tuple to it.

This document provides a detailed breakdown of this architecture, drawing evidence from the decompiled C code in the `raw_decompilations` directory.

## Key Components

### 1. The Server Message Dispatcher (`Gs2c_SrvMsgDispatcher`)

*   **Function:** `Gs2c_SrvMsgDispatcher` (see `raw_decompilations/Gs2c_SrvMsgDispatcher.c`)
*   **Role:** This is the primary function responsible for receiving and processing incoming server messages. It reads the incoming data stream, frames it, and orchestrates the parsing and handling of each message.

### 2. The Schema-Driven Parser (`Msg_ParseAndDispatch_BuildArgs`)

*   **Function:** `Msg_ParseAndDispatch_BuildArgs` (see `raw_decompilations/Msg_ParseAndDispatch_BuildArgs.c`)
*   **Role:** This function acts as a virtual machine that interprets a schema to parse a raw byte buffer into a typed argument tuple. It is the core of the client's ability to handle a wide variety of message structures in a data-driven way.
*   **Schema Typecodes:** The schema is defined by a series of typecodes that specify the data types and structures within the packet. Some of the key typecodes include:
    *   `0x02`: `u8`
    *   `0x11`: A length-prefixed array with a `u8` count.
    *   `0x12`: A length-prefixed array with a `u16` count.
    *   `0x13`: A fixed-size byte array.
    *   `0x14`: A length-prefixed byte array with a `u8` length.
    *   `0x15`: A length-prefixed byte array with a `u16` length.
    *   `0x17`: `u32`

### 3. Post-Parse Dispatcher (`Gs2c_PostParseDispatcher`)

*   **Function:** `Gs2c_PostParseDispatcher` (see `raw_decompilations/Gs2c_PostParseDispatcher.c`)
*   **Role:** After a message has been parsed into an argument tuple, this function is responsible for dispatching it to the appropriate handler. It contains a large `switch` statement that routes messages based on their opcode.
*   **Note on Networking:** Subsequent analysis has revealed that `Gs2c_PostParseDispatcher` is not part of the core networking stack. It is a more general-purpose message dispatcher that handles a variety of internal system events, not just network messages.
*   **Handler Blocks:** For contiguous ranges of opcodes, the `Gs2c_PostParseDispatcher` may delegate to specialized handler blocks, such as the one at `FUN_14025DA50` which handles opcodes from `0x2B` to `0x50`.

## Packet Flow Example: `SMSG_0x003F` (String Notification)

The handling of `SMSG_0x003F` provides a clear example of the entire dispatch process:

1.  **Dispatch:** `Gs2c_SrvMsgDispatcher` receives the raw bytes for the `0x003F` message.
2.  **Parsing:** It calls `Msg_ParseAndDispatch_BuildArgs` with the schema for `0x003F` (located at `DAT_142511600`). This schema defines the packet as containing a null-terminated ANSI string.
3.  **Handling:** After parsing, the `Gs2c_PostParseDispatcher` internally routes the message. For opcode `0x003F`, it dispatches the parsed string to `MsgConn_DispatchString` with `channel = 1`. The `SMSG_0x0040` packet follows an identical flow but is dispatched to `channel = 2`.

## Notes on Agent Messages

*   The `gs2c` agent messages are in the 31 to 88 range (decimal).
*   The message `gs2c 0x0008` was previously thought to be an agent update but is now understood to be a small performance-related message. The size-based polymorphic dispatch described in the `archive` directory is not the primary mechanism for message handling.

## Implications for Reverse Engineering

*   **Schema is King:** The authoritative source for a packet's structure is its schema. By locating the schema for a given opcode, we can definitively determine its layout.
*   **Handler Logic Provides Semantics:** The handler function for a given opcode reveals how the parsed data is used, providing the semantic meaning of the packet's fields.
*   **System is Data-Driven:** The use of a schema-driven approach means that new messages can be added or existing ones changed without requiring significant code changes to the core dispatch logic.

This consolidated overview should serve as a more accurate and comprehensive guide to the client's network message processing architecture.