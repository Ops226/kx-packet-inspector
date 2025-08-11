# CMSG Packet Discovery Playbook

**Status:** Confirmed & Validated Master Workflow

## Objective

This document provides the definitive, repeatable method for discovering and validating any Client-to-Server (CMSG) packet structure. This workflow is a complete, end-to-end process, starting with dumping the client's schema metadata and proceeding to live traffic analysis to confirm the findings.

## Core Principle: The Hybrid Model

The client uses a sophisticated hybrid model for building outgoing packets, combining high-level logic with a generic serialization engine. Understanding this is key to successful analysis.

1.  **High-Level "Data Aggregators":** For each major player action (moving, jumping, using a skill), there is a dedicated high-level function (e.g., in the `Gw2::Engine::Agent::AgCommand` family). This function's job is to gather all necessary game state and package it into a temporary data structure.
2.  **Generic Schema-Driven Serialization:** The aggregator function then calls a generic builder, which in turn uses a master schema table and the `Msg::MsgPack` engine to serialize the temporary data structure into the final packet bytes.
3.  **"Fast Path" Optimization:** For high-frequency packets, the aggregator may only provide a *minimal subset* of the data defined in the full schema. The serialization engine intelligently skips the unprovided optional fields, resulting in a small, efficient packet on the wire.

**Live traffic is the only source of truth** for determining the final on-wire structure of a packet.

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

With the live base address and size, use the provided LUA script (`tools/cheat-engine/CE_CMSG_SchemaDumper.lua`) in Cheat Engine to automatically print the complete `Opcode -> Schema Address` mapping.

---

## Part 2: The Universal CMSG Discovery Workflow

### Step 2.1: Find a Low-Level Write Primitive

1.  In your debugger, use a logging feature (e.g., Cheat Engine's "Find out what writes to this address") on the main `MsgSendContext` buffer (offset `+0x398` from the `MsgSendContext` object).
2.  Perform the desired in-game action.
3.  Analyze the log. For many packets, the 2-byte opcode is written by a generic "write primitive" function. A common one is `mov [rcx], ax` at address **`"Gw2-64.exe"+FD2DAE`**. This instruction is our initial hook point.

### Step 2.2: Isolate the Builder with a Conditional Breakpoint

This is the most critical step. We will pause the game *only* when the specific opcode we're interested in is being written.

1.  **Set a Breakpoint:** In your debugger, go to the address of the low-level write primitive (e.g., `"Gw2-64.exe"+FD2DAE`). Set a normal breakpoint (F5).
2.  **Set the Condition:** Right-click the breakpoint and add a condition. The condition must check the value of the register being written. For a 2-byte opcode written from `AX`, the formula is:
    ```lua
    (RAX & 0xFFFF) == 0x0017
    ```
    *(Replace `0x0017` with the hex value of the opcode you are hunting. The register name `RAX` must be uppercase and the hex value must start with `0x`.)*
3.  **Trigger and Break:** Perform the in-game action. The debugger will now pause *only* when your target opcode is being written.

### Step 2.3: Find the High-Level Aggregator in the Call Stack

When the conditional breakpoint hits, examine the debugger's **Call Stack**.

*   **Interpreting the Call Stack:** You are paused deep inside the serialization engine. You must walk up the stack, past the low-level serialization functions (e.g., `Msg::MsgPack`), until you find a function that is clearly part of the higher-level game logic. This function is the **true data aggregator** that initiates the packet creation process. It will typically have a different address range (e.g., `...101xxxx` instead of `...FDxxxx`).

    *Example Call Stack:*
    ```
    ...FD2DAE      <-- You are here (Low-level write)
    ...FD2A1A      <-- Serialization Engine
    ...1021B1F     <-- Serialization Engine
    ...10225DE     <-- Generic Builder
    ...1016919     <-- **THIS IS THE HIGH-LEVEL AGGREGATOR**
    ```

### Step 2.4: Statically Analyze the Full Call Chain

Navigate to the aggregator function's address in Ghidra and analyze its code, following the chain of calls down to the serialization engine. This will reveal the complete construction process.

### Step 2.5: Document the Verified Structure

Create or update the markdown file for the packet.

*   Describe the full construction chain, from the high-level aggregator down to the serialization call.
*   Provide the confirmed on-wire packet structure, validated against live captures.
*   Link to the key decompiled functions as evidence.