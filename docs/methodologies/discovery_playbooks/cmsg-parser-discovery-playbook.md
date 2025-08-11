# CMSG Packet Discovery Playbook (Live-Validation Workflow)

**Status:** Confirmed & Validated Workflow

## Objective

This document provides the definitive, repeatable method for discovering and validating any Client-to-Server (CMSG) packet structure. This workflow is based on a dynamic-first approach that prioritizes live traffic and call stack analysis to correctly identify the true origin of a packet.

## Core Principle: Two Serialization Systems

The client uses two distinct pathways for building outgoing packets. The key to successful analysis is to determine which pathway a packet uses.

1.  **Schema-Driven Path:** For complex or data-rich packets. A high-level function calls a generic builder (`CMSG::BuildAndSendPacket`), which uses a master schema table to serialize the data into the main outgoing buffer.
2.  **Manual "Fast Path":** For high-frequency or performance-critical packets. A specialized function manually constructs a simplified byte structure and writes it directly to the outgoing buffer, bypassing the schema system.

An opcode can be **overloaded**, having both a schema-defined variant and a manually-built variant. **Live traffic is the only source of truth.**

## The Universal CMSG Discovery Workflow

### Step 1: Baseline Analysis (Hypothesis Generation)

Start by gathering initial data to form a hypothesis.

1.  **Capture Live Traffic:** Use a packet inspector to capture a sample of the packet you want to analyze. Note its opcode and raw hex data. This is your "ground truth."
2.  **Consult the Schema Dump:** Look up the opcode in your CMSG schema table dump. Decode the schema at the corresponding address.
3.  **Compare and Form Hypothesis:**
    *   **If the live packet's structure appears to match the decoded schema:** Your hypothesis is that the packet is **schema-driven**.
    *   **If the live packet is much simpler or has a different structure:** Your hypothesis is that the packet is **manually built** on a "Fast Path."

### Step 2: Pinpoint the Builder Function (Dynamic Analysis)

For the vast majority of gameplay-related packets (movement, skills, jumps, interactions), the data is written to a central buffer before being sent. This gives us a single, reliable point for interception.

1.  **Set a Write Breakpoint:** In a debugger (like Cheat Engine), set a hardware **write breakpoint** on the `MsgSendContext` buffer. This buffer is located at offset `+0x398` from the base address of the `MsgSendContext` object.
    *(Tip: Find the `MsgSendContext` object's address by breaking on `MsgConn::FlushPacketBuffer` once and reading the `RCX` register.)*

2.  **Trigger the Action & Catch the Break:**
    *   Perform the in-game action that sends the packet.
    *   The debugger will pause the game at the exact instruction that is writing data to the buffer.

3.  **Find the Builder in the Call Stack:**
    *   Examine the debugger's **Call Stack**.
    *   Walk up the stack from the breakpoint until you find the high-level function that initiated the process. This function is the **true builder**. It will be outside the low-level `Msg` or `MsgConn` serialization namespaces (e.g., its address will not be in the `140FDxxxx` range).

### Step 3: Confirm the Pathway (Static Analysis)

Navigate to the builder function's address in Ghidra and analyze its code. This will confirm your hypothesis from Step 1.

*   **If you see calls to `CMSG::BuildAndSendPacket` (or a virtual call that leads to it):** The packet is **schema-driven**. Your analysis of the schema from the master table is correct.
*   **If you see the function manually writing bytes (e.g., creating a local struct and calling a memory copy function):** The packet is **manually built**. The logic inside this function defines its true structure.

### Step 4: Document the Verified Structure

Create or update the markdown file for the packet in `docs/packets/cmsg/`.

*   **Crucially, state which pathway the packet uses (Schema-Driven or Manually Built).**
*   Provide the confirmed packet structure, field by field.
*   Link to the decompiled builder function(s) you discovered as evidence.
*   If the packet is overloaded, document both the observed variant and the unobserved (schema) variant, clearly marking the status of each.

## Note on the `Direct Queue` Pathway (`MsgConn::QueuePacket`)

While most gameplay packets use the buffered stream, some system-level or simple command packets may call `MsgConn::QueuePacket` directly. If the write breakpoint on the buffer yields no results for a specific packet, setting a breakpoint on `MsgConn::QueuePacket` is a valid alternative investigation path.