# Packet Parser Discovery Playbook (CMSG Workflow)

**Date:** 2025-08-05

## Objective

This document provides a concise, repeatable method for discovering Client-to-Server (CMSG) packet structures. This playbook is a companion to the SMSG playbook and aligns with the architecture described in `system-architecture.md`.

## Scope and Constraints

*   **Environment:** Ghidra CodeBrowser on `Gw2-64.exe` for static analysis. A debugger (e.g., Cheat Engine) is required for dynamic verification of observed packet behavior.
*   **Methodology:** A hybrid approach combining static analysis (tracing calls from core builder/queueing functions) with dynamic observation of emitted packets.
*   **Evidence:** All findings should be backed by references to the relevant functions and data structures in the decompiled code and confirmed by live network logs.

## Core Concepts

The client's outgoing message construction is a multi-stage process, utilizing **two distinct pathways** optimized for different types of traffic:

1.  **Game Logic:** High-level functions related to player actions (e.g., movement, skill use) prepare the raw data for a packet.
2.  **`MsgConn::BuildPacketFromSchema`:** This central utility function takes a schema and raw data, then orchestrates the serialization.
3.  **`Msg::MsgPack`:** An internal "virtual machine" that `MsgConn::BuildPacketFromSchema` uses to write data into a packet buffer according to the schema's typecodes.
4.  **Queueing & Sending:** The completed packet is then passed to a queueing mechanism (e.g., `MsgConn::QueuePacket` or `MsgConn::FlushPacketBuffer`) to be sent to the server.

## Workflow: Tracing Packet Construction

### Phase 1: Identify Packet Emitters via Dual Pathways

To understand how CMSG packets are structured and sent, it's crucial to identify their emission points. The client uses two distinct pathways for outgoing packets.

#### Pathway 1: The "Buffered" Stream (High-Frequency Data) Discovery

This pathway handles continuous or aggregated data (e.g., player movement, mount movement). Data is written into a buffer, which is periodically flushed.

1.  **Hook the Flush Point:** To capture packets on this pathway, hook `FUN_140fd1e80` (renamed `MsgConn::FlushPacketBuffer`). This function is called whenever the buffered data is ready to be sent.
2.  **Capture and Parse:** When the hook triggers, the `MsgSendContext` object (passed as a parameter) will contain a buffer (at `0x398` offset) with concatenated packet data. You must manually parse this buffer to extract individual `(Opcode, Payload)` pairs. This requires parsing the opcode (first 2-4 bytes) and then using knowledge of schema definitions to determine the length of each sub-packet.
3.  **Trigger Action:** Perform continuous actions in-game (e.g., move your character, use abilities that involve persistent effects) to generate these buffered packets.

#### Pathway 2: The "Direct Queue" (Discrete Events) Discovery

This pathway handles single, discrete events that need to be sent immediately (e.g., heartbeats, skill activation, interactions).

1.  **Hook the Queueing Point:** Hook `FUN_14104d760` (renamed `MsgConn::QueuePacket`). This function is directly called with the opcode and payload for discrete packets.
2.  **Capture Data:** When the hook triggers, the `opcode` is passed as the third argument, and a pointer to the `pPayload` as the fourth.
3.  **Trigger Action:** Perform discrete actions in-game (e.g., jump, use a single skill, click to interact with an NPC). The constant `0x0005` heartbeat packet is a prime example of traffic on this pathway.

### Phase 2: Analyze Calling Functions to Deduce Packet Structure and Purpose

Once you have captured opcodes and payload data using the hooks from Phase 1, you can deduce their structure and origin.

1.  **Identify Builder/Wrapper Functions:**
    *   For packets captured via Pathway 2, the function that calls `MsgConn::QueuePacket` is the specific CMSG packet builder.
    *   For packets captured via Pathway 1, you'll need to analyze the buffer, then identify the high-level logic that created those specific sub-packets.
2.  **Find the Schema (if applicable):** For schema-driven packets, trace backward from the builder function to find the call to `MsgConn::BuildPacketFromSchema`. The schema address is passed as the second argument to this call.
3.  **Deduce Purpose:** Analyze the code of the builder function and the data it prepares. This, combined with the opcode, will help you deduce the packet's purpose.

### Phase 3: Decode the Schema and Document

1.  **Decode the Schema:** Navigate to the schema's data address in Ghidra. Use the typecode definitions from `system-architecture.md` to determine the exact layout of the packet's data.
2.  **Document the Packet:** Create a new `.md` file in the `packets/cmsg/` directory for the discovered CMSG packet.
    *   Include its opcode, a summary of its purpose, its schema address (if applicable), and a detailed field table (offset, type, name, description).
    *   Provide code snippets from the calling function as evidence.
    *   Add the packet to the `packets/cmsg/README.md` reference table.

## Example: Agent Link Packet (`0x0036`)

*   **Builder Call Found:** `CMSG::BuildAgentLink` is found to call `MsgConn::BuildPacketFromSchema`.
*   **Schema & Opcode:** Inside `CMSG::BuildAgentLink`, the schema `DAT_142513080` is passed to `MsgConn::BuildPacketFromSchema`. The opcode `0x36` is passed to `MsgConn::QueuePacket`.
*   **Pathway:** This packet uses Pathway 2 (Direct Queue).
*   **Data Analysis:** Further analysis of `CMSG::BuildAgentLink` reveals the data being passed matches the expected structure of an "Agent Link" update.
*   **Documentation:** This leads to the creation of `packets/cmsg/CMSG_AGENT_LINK_0x0036.md` detailing its structure and purpose.

## Conclusion

By systematically employing dynamic hooks and tracing the code, you can accurately discover and document all client-to-server packets. This hybrid approach efficiently covers both buffered and direct-queued outgoing traffic.
