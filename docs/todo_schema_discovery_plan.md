# Packet Schema Discovery Action Plan

This document outlines a more direct and efficient strategy for discovering CMSG and SMSG packet schemas by targeting the game client's central opcode-to-schema resolution tables.

## Primary Objective

Accelerate the packet discovery process by shifting from a one-by-one handler analysis to a bulk analysis of master lookup tables.

---

## CMSG Discovery Plan (Static-First)

The goal is to find the master table that maps CMSG opcodes to their corresponding schemas.

**Evidence:** The function `CMSG::BuildAndSendPacket.c` uses a resolver function (`FUN_140fd5b50`) to look up schemas.

**Action Items:**

1.  **Analyze the CMSG Resolver Function:**
    *   **Task:** Perform a detailed static analysis of the resolver function (`FUN_140fd5b50`) identified in `CMSG::BuildAndSendPacket.c`.
    *   **Goal:** Determine how it performs its lookup (e.g., direct array access, hash map).
    *   **Status: COMPLETED**
    *   **Analysis:** The function `FUN_140fd5b50` is a simple resolver that performs a direct array lookup. It takes a pointer to a `SchemaTableInfo` structure and an `opcode` as arguments.
        *   It uses `*(SchemaTableInfo + 0x5c)` as the number of entries in the table to perform a bounds check on the `opcode`.
        *   It uses `*(SchemaTableInfo + 0x50)` as the base address of the lookup table.
        *   It calculates the entry address as `base_address + opcode * 0x10`. Each entry is 16 bytes.
        *   It checks for a non-null schema pointer at offset `+8` within the entry.
        *   If valid, it returns a pointer to the 16-byte entry structure itself.

2.  **Dump the Master CMSG Table:**
    *   **Task:** Once the lookup mechanism is understood, locate the master data table it uses.
    *   **Goal:** Write a script (e.g., Ghidra script) to dump this table, creating a definitive list of `Opcode -> Schema Address`.
    *   **Status: COMPLETED**
    *   **Findings:**
        *   **Table Base Address:** `0x000002BE68192490` (live address)
        *   **Table Size:** `521` entries
        *   **Entry Size:** `16` bytes
        *   **Action:** A script can now be created to iterate `521` times from the base address. For each 16-byte entry, the script should read the schema pointer at offset `+8` to build the full `Opcode -> Schema Address` map.

3.  **Systematic Documentation:**
    *   **Task:** Use the dumped table as a worklist. For each known schema, find its cross-references to identify its builder function(s).
    *   **Goal:** Update the Master Packet Index table and create documentation for each new packet discovered.

---

## SMSG Discovery Plan (Dynamic Verification Required)

The goal is to find the master table that maps SMSG opcodes to their `Handler Info` structures (containing both schema and handler function pointers).

**Evidence:** `Msg::DispatchStream.c` and `hooking-implementation.md` describe a system where opcodes are used to look up a `Handler Info` structure.

**Action Items:**

1.  **Analyze the SMSG Dispatcher:**
    *   **Task:** Perform a detailed static analysis of `Msg::DispatchStream.c`.
    *   **Goal:** Pinpoint the exact location where the incoming opcode is used to index or look up an entry in the master handler/schema table.

2.  **Dump the Master SMSG Table:**
    *   **Task:** Locate the master data table identified in the previous step.
    *   **Goal:** Write a script to dump this table, creating a list of `Opcode -> {Schema Address, Handler Address}`.

3.  **Verification via Dynamic Analysis:**
    *   **Task:** Use the dynamic analysis workflow from the `smsg-parser-discovery-playbook.md` (breakpoint on `call rax` at `"Gw2-64.exe"+FD1ACD`).
    *   **Goal:** Verify that the handlers found in the static table match the handlers observed at runtime. This is critical to account for dynamic dispatch, "Fast Path" packets like `0x0001`, and other potential runtime modifications.

4.  **Systematic Documentation:**
    *   **Task:** Use the verified table data to document all newly discovered SMSG packets.
    *   **Goal:** Create schema files and update the Master Packet Index.

