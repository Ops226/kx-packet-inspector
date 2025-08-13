# Packet Parser Discovery Playbook (SMSG Dynamic Analysis Workflow)

**Date:** 2025-08-05
**Status:** **Revised & Finalized**

## Objective

This document provides a concise, repeatable method for discovering Server-to-Client (SMSG) packet structures based on the confirmed **dynamic dispatch system**. This playbook supersedes all previous versions and is aligned with the dynamic architecture described in `../../system-architecture.md`. **Static analysis alone is not sufficient** for handler discovery.

## Scope and Constraints

*   **Primary Environment:** A live game client with a debugger (e.g., Cheat Engine) attached, or a custom hooking tool.
*   **Secondary Environment:** Ghidra CodeBrowser on `Gw2-64.exe` for static analysis of dynamically discovered functions.
*   **Methodology:** Dynamic analysis to find the correct post-parse handler, followed by static analysis to decode the schema and handler logic.
*   **Evidence:** All findings must be backed by live memory addresses and supported by decompiled code from the identified handler.

## Core Concepts

The client's incoming message handling is a multi-stage, dynamic process.

1.  **`Msg::DispatchStream`:** The primary entry point that receives the raw message stream.
2.  **`MsgUnpack::ParseWithSchema`:** A schema "virtual machine" called by the dispatcher to parse raw bytes into a structured data tuple. This function is crucial as it builds a temporary, C++-like struct in memory (the "parsed data tuple") from the raw packet bytes. Handlers then receive a pointer to this parsed tuple, not the raw network buffer.
3.  **Dynamic Post-Parse Handlers:** Crucially, there is **no single master handler**. After parsing, `Msg::DispatchStream` calls a function pointer that is resolved at runtime. This means different handlers can be active depending on the game's state. `Portal::DispatchMessage` is just one of these many possible handlers.

## Key Code Signatures for Dynamic Analysis

To reliably find the correct locations for dynamic analysis after a game update, we use the following code signatures. These can be used with a memory scanner to find the new addresses.

### SMSG Dynamic Handler Dispatch (`call rax`)

This is the most important location for SMSG analysis. It is the instruction that calls the dynamically resolved post-parse handler. Placing a breakpoint here allows capturing the handler address from the `RAX` register.

*   **Last Known Address:** `"Gw2-64.exe"+FD1ACD`
*   **Byte Signature (Pattern):** `48 8B 41 18 41 8B 0C 24 48 8B 55 E8 FF D0`
*   **Assembly Context:**

    ```assembly
    mov rax,[rcx+18]      ; Load handler function pointer into RAX
    mov ecx,[r12]         ; Load first argument for handler
    mov rdx,[rbp-18]      ; Load second argument (pointer to parsed data)
    call rax              ; Execute the handler
    ```

### SMSG Schema Parser Call

This is the instruction that calls the schema VM to parse the raw packet data. Placing a breakpoint here allows capturing the schema address.

*   **Last Known Address:** `"Gw2-64.exe"+FD1A47`
*   **Assembly Context:** `call MsgUnpack::ParseWithSchema`

## Workflow: Dynamic Analysis First

### Phase 1: Identify the True Post-Parse Handler (Automated)

The most efficient way to find the correct handler for an SMSG packet is to use a custom hooking tool.

1.  **Use `MessageHandlerHook.cpp`:** Your existing C++ hook (`src/MessageHandlerHook.h`) is designed for this. Compile and inject it into the game.
2.  **Capture Output:** Use a debug monitor (like DebugView from Sysinternals) to capture the `OutputDebugStringA` messages. These messages will contain lines like:
    `[Packet Discovery] Opcode: 0x0016 -> Handler: Gw2-64.exe+01350E60`
3.  **Trigger Packets:** Play the game and perform various actions to trigger as many different SMSG packets as possible. The hook will automatically log the `Opcode -> Handler` pairs.
4.  **Aggregate Results:** Collect all unique `Opcode -> Handler` mappings from the debug output. This is your primary SMSG discovery map.

### Phase 2: Analyze the Handler and Find the Schema

Now, switch to your static analysis tool (Ghidra).

1.  **Locate the Handler Function:** Convert the live handler address from your discovery map into a static address in Ghidra.
2.  **Identify the Schema Address:** The handler function (which received the parsed data tuple) will typically perform its logic by reading fields from this tuple. To discover the schema that created this tuple, you must trace backward from the `Msg::DispatchStream` breakpoint. The schema address is passed as an argument to the schema parser (`"Gw2-64.exe"+FD43C0`) just before the handler is dynamically called.
    *   **Setting a Conditional Breakpoint:** To capture this schema address for a specific opcode (e.g., `0x0016`), set a breakpoint on the `call MsgUnpack::ParseWithSchema` instruction (`"Gw2-64.exe"+FD1A47`). Use the following condition:

    ```lua
    -- Break if messageId is 0x16
    local ptr = readPointer(RBX + 0x48)
    if ptr and ptr ~= 0 then
      if readSmallInteger(ptr) == 0x16 then
        return 1
      end
    end
    ```

    *   When the breakpoint hits, the `RCX` register will contain the address of the schema definition for that packet.

### Phase 3: Decode the Schema and Understand Handler Logic

1.  **Decode the Schema:** Once you have the schema address (from `RCX` in Phase 2), you can use your `KX_CMSG_Full_Schema_Decoder.lua` script to decode its structure. Paste the schema address into the script's input prompt.

    *   **Solved: The Definitive SMSG Typecode Map:** The investigation into the SMSG typecode discrepancy is complete. Analysis of the `MsgUnpack::ParseWithSchema` function's internal `switch` logic has confirmed that it interprets certain typecodes differently than its CMSG counterpart (`Msg::MsgPack`).

        The most critical divergence is typecode `0x08`, which corresponds to an 8-byte `long long` in SMSG packets, not the 12-byte `float[3]` used in CMSG.

        Use the following definitive `TYPECODE_MAP` for 100% accurate decoding of all SMSG schemas:

| Typecode | Data Type (from `MsgUnpack::ParseWithSchema`) | Size (bytes) | Notes |
| :--- | :--- | :--- | :--- |
| `0x01` | `short` | 2 | Consistent with CMSG. |
| `0x02` | `byte` | 1 | Consistent with CMSG. |
| `0x03` | `int` | 4 | Consistent with CMSG. |
| `0x04` | Compressed `int` | 1-5 | Variable-length integer. Consistent with CMSG. |
| `0x05` | `long long` | 8 | Consistent with CMSG. |
| `0x06` | `float` or `int` | 4 | Consistent with CMSG. |
| `0x07` | `float[2]` | 8 | Consistent with CMSG. |
| **`0x08`** | **`long long` or `qword`** | **8** | **DIVERGENCE:** CMSG uses this for `float[3]` (12 bytes). SMSG uses it for an 8-byte integer or pointer. |
| `0x09` | `float[4]` | 16 | Consistent with CMSG. |
| `0x0A` | Special Vector + Compressed Int | 12 + variable | Consistent with CMSG. |
| `0x0D` | `string` (wchar_t\*) | Variable | Null-terminated wide string. Consistent with CMSG. |
| `0x0E` | `string_utf8` (char\*) | Variable | Null-terminated UTF-8 string. Consistent with CMSG. |
| `0x0F` | Optional Block | Variable | Preceded by a flag byte. Consistent with CMSG. |
| `0x11` | Variable Array (byte count) | Variable | Consistent with CMSG. |
| `0x12` | Variable Array (short count) | Variable | Consistent with CMSG. |
| `0x14` | Variable Buffer (byte count) | Variable | Consistent with CMSG. |
| `0x15` | Variable Buffer (short count) | Variable | Consistent with CMSG. |
| `0x18` | Terminator | 0 | Marks the end of a schema. Consistent with CMSG. |

With this corrected map, your tooling can now be updated to accurately decode SMSG schemas.

2.  **Analyze Handler Logic:** With the schema structure known, analyze the decompiled code of the handler function you discovered. This will reveal the semantic meaning of the packet's fields by observing how the handler uses the parsed data.

    **Note on Fast Path Handlers:** For high-frequency packets that use the "Fast Path" (like `SMSG_AGENT_UPDATE_BATCH`), the dynamically discovered handler (e.g., `Event::PreHandler_Stub_0x88`) is a *notification stub* that is called *after* the packet has already been parsed by hardcoded logic inside `Msg::DispatchStream`. Do not look for parsing logic in these stubs; it resides in the dispatcher itself.

### Phase 4: Document Your Findings

1.  **Create a New Packet Document:** Create a new `.md` file in the `packets/smsg/` directory for the opcode or unique identifier you are analyzing.
2.  **Document the Packet Structure:** Create a field table detailing the offset (within the *parsed tuple*), type, name, and description of each field.
3.  **Document the Handler Logic:** Provide a summary of the handler's behavior and link to its address. **Crucially, note that this handler was discovered dynamically and is context-dependent.**

## Example: A Message Handled by `FUN_1413e5d90` (Marker::Cli::ProcessAgentMarkerUpdate)

*   **Dynamic Discovery:** A breakpoint at `"Gw2-64.exe"+FD1ACD` is triggered. The `RAX` register contains the address for `FUN_1413e5d90` (renamed `Marker::Cli::ProcessAgentMarkerUpdate`), and `RDX` points to a data tuple beginning with bytes `FE 03 ...`.
*   **Static Analysis:** We navigate to `Marker::Cli::ProcessAgentMarkerUpdate` in Ghidra. We analyze its code to see how it uses the fields from the data tuple passed to it. We then trace back to the schema parser call to find the schema that defines this structure.
*   **Conclusion:** This dynamic approach allows us to correctly associate the packet data with its specific handler, `Marker::Cli::ProcessAgentMarkerUpdate`, bypassing incorrect assumptions about a single static dispatcher.

## Architectural Note: Why SMSG Cannot Be Mass-Dumped Like CMSG

Unlike the Client-to-Server (CMSG) protocol, which utilizes a single, contiguous master table of schema pointers, the Server-to-Client (SMSG) system is architecturally different. SMSG schemas are not located in a linear block of memory. Instead, each opcode's schema pointer is stored within a decentralized "Handler Info" structure that is resolved dynamically at runtime.

This means a simple memory region scan, as performed for CMSG, is not possible for SMSG packets. Discovery **must** be done dynamically by observing live game traffic to map opcodes to their respective schema addresses. However, this process can be fully automated, as described in the "Automating Mass Discovery" section below.

## Next Steps: Automating Mass Discovery

While the manual workflow described above is reliable for analyzing individual packets, the ultimate goal is to mass-discover and dump all SMSG schemas automatically. Based on the decentralized architecture, a two-stage "harvest and process" approach is the most effective solution.

### Stage 1: The "Harvester" Tool (Dynamic Discovery)

The next evolution of the KX Packet Inspector should include a tool designed to automatically harvest opcode-to-schema mappings during live gameplay.

1.  **Target Function:** The tool would hook the schema virtual machine function, **`MsgUnpack::ParseWithSchema`**.
2.  **Hook Logic:** The detour function for this hook would perform the following actions on every call:
    *   **Extract Opcode:** Read the `messageId` of the packet currently being processed.
    *   **Extract Schema Address:** Read the pointer to the schema definition (passed as the first argument, typically in the `RCX` register).
    *   **Log the Mapping:** Write every unique `[Opcode -> Schema Address]` pair it observes to a log file (e.g., `smsg_schema_map.txt`).

### Stage 2: The "Processor" Script (Static Dumping)

Once the harvester has generated a map, a second script can be used to process it.

1.  **Input:** The script would read the `smsg_schema_map.txt` file.
2.  **Logic:** It would iterate through each line, take the schema address, and run it through the same decoding logic used by the `KX_SMSG_Schema_Decoder.lua` script.
3.  **Output:** The script would append the formatted output for every discovered schema into a single, comprehensive Markdown document, creating a definitive master reference for the entire SMSG protocol.

This automated two-stage process represents the definitive path to achieving a complete dump of the SMSG protocol, overcoming the architectural limitations that prevent a simple linear scan.
