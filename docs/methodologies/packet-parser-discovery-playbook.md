# Packet Parser Discovery Playbook (Schema-Driven)

**Date:** 2025-08-05

## Objective

This document provides a concise, repeatable method for discovering and documenting packet parsers based on the now-confirmed schema-driven dispatch system. This playbook supersedes all previous versions and is aligned with the architecture described in `system-architecture.md`.

## Scope and Constraints

*   **Environment:** Ghidra CodeBrowser on `Gw2-64.exe` with full analysis.
*   **Methodology:** Static analysis of the client's message dispatchers and schema definitions.
*   **Evidence:** All findings should be backed by references to the relevant functions and data structures in the decompiled code.

## Core Concepts

The client's network message processing is fundamentally data-driven. Instead of relying on polymorphic vtables, the system uses a combination of dispatchers and per-opcode schemas to parse and handle messages.

*   **Dispatchers:** These are the central functions that orchestrate the message handling process. The key dispatchers are:
    *   `Gs2c_SrvMsgDispatcher`: The entry point for incoming server messages.
    *   `Gs2c_PostParseDispatcher`: A secondary dispatcher that routes parsed messages to their handlers, often via a large `switch` statement. **Note:** This dispatcher is not strictly part of the networking layer and handles more general system events.
*   **Schemas:** Each opcode has a corresponding schema (e.g., `&DAT_14251xxxx`) that defines the structure of the packet. These schemas are interpreted by the `Msg_ParseAndDispatch_BuildArgs` function.
*   **Handlers:** These are the functions that are ultimately called to handle the logic of a specific packet. They receive the data parsed from the packet as a typed argument tuple.

## Workflow

### Phase 1: Locate the Dispatcher and Schema

1.  **Start at the `Gs2c_PostParseDispatcher`:** This function is the most efficient starting point for analyzing a specific opcode. It contains a large `switch` statement that maps opcodes to their handlers.
2.  **Identify the Opcode's Case:** Find the `case` block for the opcode you are investigating.
3.  **Locate the Schema:** Within the `case` block, you will find a call to `Msg_BuildArgs_FromSchema` or a similar function. The second argument to this function is a pointer to the schema for that opcode (e.g., `&DAT_14251xxxx`).

### Phase 2: Decode the Schema

1.  **Analyze the Schema Data:** Navigate to the schema's data address in Ghidra. The schema is a sequence of typecodes that define the packet's structure.
2.  **Interpret the Typecodes:** Use the typecode definitions from `system-architecture.md` to manually decode the schema. This will give you the exact layout of the packet, including the types and order of its fields.

### Phase 3: Analyze the Handler

1.  **Identify the Handler Function:** The `case` block in the `Gs2c_PostParseDispatcher` will contain a call to the handler function for the opcode.
2.  **Decompile the Handler:** Analyze the decompiled code of the handler to understand how it uses the parsed data. This will reveal the semantic meaning of the packet's fields.

### Phase 4: Document Your Findings

1.  **Create a New Packet Document:** Create a new `.md` file in the `packets` directory for the opcode you are analyzing.
2.  **Document the Packet Structure:** In the new file, create a field table that details the offset, type, name, and a description of each field in the packet.
3.  **Document the Handler Logic:** Provide a summary of the handler's behavior and how it uses the packet's data.
4.  **Provide Evidence:** Include snippets of the decompiled code from the dispatcher and handler to support your analysis.

## Example: `SMSG_0x003F` (String Notification)

*   **Dispatcher:** `Gs2c_PostParseDispatcher`, `case 0x3F`.
*   **Schema:** `&DAT_142511600`.
*   **Handler:** `MsgConn_DispatchString` (with `channel = 1`).
*   **Analysis:** By decoding the schema and analyzing the handler, we can determine that this packet contains a null-terminated ANSI string that is dispatched to the game's string handling system on channel 1.

## Conclusion

This schema-driven approach to packet analysis is a powerful and reliable method for reverse engineering the game's network protocol. By following this playbook, you can systematically decode any packet and contribute to a more complete understanding of the game's inner workings.