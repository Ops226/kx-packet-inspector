# CMSG Schema Discovery Playbook

**Status:** Confirmed & Validated Workflow

This document outlines the most efficient and reliable method for discovering and decoding all CMSG (Client-to-Server) packets that are built using the game's schema system.

## Core Principle

The client uses a master table to map CMSG opcodes to their corresponding packet schemas. By finding and dumping this table, we can create a definitive worklist to analyze every schema-based CMSG packet. This is primarily a dynamic analysis task, as the key memory addresses are only available at runtime.

Note that some simple or high-frequency packets (like the `HEARTBEAT`) may be built manually by the client and bypass this schema system entirely. Cross-referencing with live packet logs is crucial to identify these cases.

## The Workflow

### Step 1: Identify the Key Function (Static Analysis)

The entire process starts by understanding that the client has a generic function for building packets from a schema. The documentation identifies this as `CMSG::BuildAndSendPacket`. Analysis of this function reveals it uses a resolver, `FUN_140fd5b50`, to look up the schema for a given opcode. This confirms the existence of the master table.

### Step 2: Find the Schema Table Address (Dynamic Analysis)

The pointers to the connection object and the schema table are initialized at runtime. You must use a memory debugger like Cheat Engine on the live game process to find their addresses.

1.  **Find `MsgConn` Pointer:** The global pointer to the game connection object is at the static address `DAT_142628290`. In Cheat Engine, find the 8-byte pointer value stored at `"Gw2-64.exe"+2628290`. This is the live address of the `MsgConn` object.

2.  **Find `SchemaTableInfo` Pointer:** The pointer to the structure containing table information is at offset `+0x18` from the `MsgConn` object. Read the 8-byte pointer at `[MsgConn Address + 0x18]`.

3.  **Find Table Base and Size:** From the `SchemaTableInfo` address you just found, read the following:
    *   **Table Base:** The 8-byte pointer at offset `+0x50`. This is the final address of the CMSG schema table.
    *   **Table Size:** The 4-byte integer at offset `+0x5c`. This is the number of entries in the table.

### Step 3: Dump the Opcode-to-Schema Table

With the live base address and size of the table, use the provided LUA script (`CE_CMSG_SchemaDumper.lua`) in Cheat Engine to automatically iterate through the table and print the complete `Opcode -> Schema Address` mapping.

### Step 4: Decode an Individual Schema

For each schema address you dumped, you can determine the packet's structure by decoding its contents.

1.  **Go to the Schema Address** in a memory viewer.
2.  **Read Entries:** A schema is an array of **40-byte** entries. Read them sequentially.
3.  **Decode Typecode:** The first 4 bytes of each entry is a **typecode** that defines the data type for a field in the packet.
4.  **Repeat:** Continue reading 40-byte entries until you find a terminator entry (typecode `0x00` or `0x18`).

Use the Typecode Reference Table in the main `system-architecture.md` to translate the typecodes into C++ data types.

### Step 5: Document and Verify

Create a new markdown file for the discovered packet. Document its structure based on the schema. Compare your findings with live packet logs to confirm your analysis and check for overloaded opcodes where a simple manual packet might share an ID with a more complex schema-based one.