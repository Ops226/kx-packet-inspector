# Packet Parser Discovery Playbook (CMSG Workflow)

**Date:** 2025-08-05

## Objective

This document provides a concise, repeatable method for discovering Client-to-Server (CMSG) packet structures. This playbook is a companion to the SMSG playbook and aligns with the architecture described in `system-architecture.md`.

## Scope and Constraints

*   **Environment:** Ghidra CodeBrowser on `Gw2-64.exe` with full analysis.
*   **Methodology:** Primarily static analysis, tracing calls backward from the packet serialization core.
*   **Evidence:** All findings should be backed by references to the relevant functions and data structures in the decompiled code.

## Core Concepts

The client's outgoing message construction is a multi-stage process:

1.  **Game Logic:** High-level functions related to player actions (e.g., movement, skill use) prepare the raw data for a packet.
2.  **`MsgConn::BuildPacketFromSchema`:** This central utility function takes a schema and raw data, then orchestrates the serialization.
3.  **`Msg::MsgPack`:** An internal "virtual machine" that `MsgConn::BuildPacketFromSchema` uses to write data into a packet buffer according to the schema's typecodes.
4.  **Queueing & Sending:** The completed packet is then passed to a queueing mechanism (e.g., `MsgConn::QueuePacket`) to be sent to the server.

## Workflow: Tracing Packet Construction

### Phase 1: Identify the Packet Builder Calls

To understand how CMSG packets are structured, we must find where they are created.

1.  **Start at the Core Packet Builder:** In Ghidra, go to `MsgConn::BuildPacketFromSchema`. This function is responsible for building a packet according to a schema.
2.  **Find All Cross-References:** Find all code cross-references to `MsgConn::BuildPacketFromSchema`. This list represents every function in the game that initiates the construction of a CMSG packet.

### Phase 2: Analyze Calling Functions to Deduce Packet Structure and Purpose

Each function that calls `MsgConn::BuildPacketFromSchema` corresponds to a specific CMSG packet.

1.  **Select a Calling Function:** Choose one function from the cross-references discovered in Phase 1 (e.g., `CMSG::BuildAgentLink`).
2.  **Examine its Code:** Analyze the decompiled code of this calling function. Look for:
    *   **The Schema Address:** The second argument passed to `MsgConn::BuildPacketFromSchema` will be the pointer to the schema that defines this packet's structure (e.g., `&DAT_142513080`).
    *   **The Data Payload:** Identify the local variables or parameters whose values are passed as the third argument (the raw data payload) to `MsgConn::BuildPacketFromSchema`. This data, combined with the schema, reveals the packet's fields.
    *   **The Opcode (and Queueing Call):** Follow the control flow after `MsgConn::BuildPacketFromSchema` returns. You will typically find a call to a queueing function (e.g., `MsgConn::QueuePacket`). The opcode for the packet is usually passed as one of the arguments to this queueing function.
    *   **Deduce Purpose:** The name and context of the calling function, along with the data it's sending, will help you deduce the packet's purpose (e.g., `CMSG_PLAYER_MOVE`, `CMSG_USE_SKILL`).

### Phase 3: Decode the Schema and Document

1.  **Decode the Schema:** Navigate to the schema's data address in Ghidra. Use the typecode definitions from `system-architecture.md` to determine the exact layout of the packet's data.
2.  **Document the Packet:** Create a new `.md` file in the `packets/cmsg/` directory for the discovered CMSG packet.
    *   Include its opcode, a summary of its purpose, its schema address, and a detailed field table (offset, type, name, description).
    *   Provide code snippets from the calling function as evidence.
    *   Add the packet to the `packets/cmsg/README.md` reference table.

## Example: Agent Link Packet (`0x0036`)

*   **Builder Call Found:** `CMSG::BuildAgentLink` is found to call `MsgConn::BuildPacketFromSchema`.
*   **Schema & Opcode:** Inside `CMSG::BuildAgentLink`, the schema `DAT_142513080` is passed to `MsgConn::BuildPacketFromSchema`. The opcode `0x36` is passed to `FUN_14104d760` (`MsgConn::QueuePacket`).
*   **Data Analysis:** Further analysis of `CMSG::BuildAgentLink` reveals the data being passed matches the expected structure of an "Agent Link" update.
*   **Documentation:** This leads to the creation of `packets/cmsg/CMSG_AGENT_LINK_0x0036.md` detailing its structure and purpose.

## Conclusion

By systematically tracing calls to `MsgConn::BuildPacketFromSchema`, you can accurately discover and document all client-to-server packets. This static-first approach is efficient for CMSG because the builder function is a clear choke point in the outgoing data flow.