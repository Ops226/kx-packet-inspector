# CMSG Packet Discovery Playbook

**Status:** Confirmed & Validated Master Workflow

## Objective

This document provides the definitive, repeatable method for discovering and validating any Client-to-Server (CMSG) packet structure. This workflow is a complete, end-to-end process, starting with dumping the client's schema metadata and proceeding to live traffic analysis to confirm the findings.

## Core Principle: Two Serialization Systems

The client uses two distinct pathways for building outgoing packets. The key to successful analysis is to determine which pathway a packet uses.

1.  **Schema-Driven Path:** For complex or data-rich packets. A high-level function calls a generic builder (`CMSG::BuildAndSendPacket`), which uses a master schema table to serialize the data into the main outgoing buffer.
2.  **Manual "Fast Path":** For high-frequency or performance-critical packets. A specialized function manually constructs a simplified byte structure and writes it directly to the outgoing buffer, bypassing the schema system.

An opcode can be **overloaded**, having both a schema-defined variant and a manually-built variant. **Live traffic is the only source of truth.**

---

## Part 1: Prerequisite - Dumping the Master Schema Table

Before analyzing individual packets, we must first extract the client's own metadata. This provides a complete worklist of all schema-driven packets.

### Step 1.1: Find the Schema Table Address (Dynamic Analysis)

The pointer to the schema table is initialized at runtime. Use a memory debugger (like Cheat Engine) on a live, logged-in game client.

1.  **Find `MsgConn` Pointer:** The global pointer to the game connection object is at the static address `DAT_142628290`. In Cheat Engine, find the 8-byte pointer value stored at `"Gw2-64.exe"+2628290`. This is the live address of the `MsgConn` object.

2.  **Find `SchemaTableInfo` Pointer:** The pointer to the structure containing table information is at offset `+0x18` from the `MsgConn` object. Read the 8-byte pointer at `[MsgConn Address + 0x18]`.

3.  **Find Table Base and Size:** From the `SchemaTableInfo` address, read the following:
    *   **Table Base:** The 8-byte pointer at offset `+0x50`. This is the final address of the CMSG schema table.
    *   **Table Size:** The 4-byte integer at offset `+0x5c`. This is the number of entries in the table.

### Step 1.2: Dump the Opcode-to-Schema Table

With the live base address and size, use the provided LUA script (`tools/cheat-engine/CE_CMSG_SchemaDumper.lua`) in Cheat Engine to automatically print the complete `Opcode -> Schema Address` mapping. This dump is the foundation for all subsequent schema analysis.

---

## Part 2: The Universal CMSG Discovery Workflow

### Step 2.1: Baseline Analysis (Hypothesis Generation)

Start by gathering initial data to form a hypothesis for a specific packet.

1.  **Capture Live Traffic:** Use a packet inspector to capture a sample of the packet. Note its opcode and raw hex data. This is your "ground truth."
2.  **Consult the Schema Dump:** Look up the opcode in the schema table dump from Part 1. Decode the schema at the corresponding address using the typecode reference table.
3.  **Compare and Form Hypothesis:**
    *   **If the live packet's structure appears to match the decoded schema:** Your hypothesis is that the packet is **schema-driven**.
    *   **If the live packet is much simpler or has a different structure:** Your hypothesis is that the packet is **manually built** on a "Fast Path."

### Step 2.2: Pinpoint the Builder Function (Dynamic Analysis)

For the vast majority of gameplay-related packets, the data is written to a central buffer. This gives us a single, reliable point for interception.

1.  **Set a Write Breakpoint:** In your debugger, set a hardware **write breakpoint** on the `MsgSendContext` buffer. This buffer is located at offset `+0x398` from the base address of the `MsgSendContext` object.
    *(Tip: Find the `MsgSendContext` address by breaking on `MsgConn::FlushPacketBuffer` once and reading the `RCX` register.)*

2.  **Trigger the Action & Catch the Break:** Perform the in-game action that sends the packet. The debugger will pause.

3.  **Find the Builder in the Call Stack:** Examine the debugger's **Call Stack**. Walk up the stack until you find the high-level function that initiated the process. This is the **true builder**. It will be outside the low-level `Msg` or `MsgConn` serialization namespaces.

### Step 2.3: Confirm the Pathway (Static Analysis)

Navigate to the builder function's address in Ghidra and analyze its code. This confirms your hypothesis from Step 2.1.

*   **If you see calls to `CMSG::BuildAndSendPacket`:** The packet is **schema-driven**.
*   **If you see manual byte manipulation and writes:** The packet is **manually built**.

### Step 2.4: Document the Verified Structure

Create or update the markdown file for the packet in `docs/packets/cmsg/`.

*   State which pathway the packet uses (Schema-Driven or Manually Built).
*   Provide the confirmed packet structure and link to the decompiled builder function(s) as evidence.
*   If an opcode is overloaded, document both variants and clearly mark their status.