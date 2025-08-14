# SMSG Packet Discovery Playbook (Final Workflow)

**Status:** Confirmed & Finalized

## Objective

This document provides the definitive, repeatable method for discovering and analyzing any Server-to-Client (SMSG) packet structure. The workflow leverages a live C++ hook for initial discovery and takes advantage of a key architectural pattern—the collocation of schema and handler pointers—to rapidly perform static analysis.

## Core Architectural Concepts

A full understanding of the client's incoming message handling requires recognizing these key patterns:

*   **Dynamic Dispatch:** The client uses a `call rax` instruction to dynamically dispatch most messages to different handler functions at runtime.
*   **The Master Dispatch Table:** A large table exists in the `.rdata` section of the executable. This table contains `[Schema Pointer, Handler Pointer]` pairs. While it is not indexed directly by opcode, it serves as a central repository for handler and schema information.
*   **Handler & Schema Collocation:** A critical discovery is that for a given entry in the master dispatch table, the pointer to the **schema** is located right next to the pointer for its corresponding **handler function**. Finding one often immediately reveals the other.
*   **Fast Path:** High-frequency packets (like `SMSG_AGENT_UPDATE_BATCH`) can bypass the generic dispatch system entirely. Their logic is hardcoded directly into the main dispatcher (`Msg::DispatchStream`) and they will not appear in the C++ logger's output.

---

## The Definitive Discovery Workflow

This workflow prioritizes the fastest path from live discovery to full static analysis.

### Stage 1: Live Discovery with the C++ Logger

This is the primary and most efficient method for mapping the SMSG protocol.

1.  **Inject the Hook:** Compile and inject the `MessageHandlerHook.cpp` tool into the live game client.
2.  **Monitor Output:** Run a debug monitor (like DebugView from Sysinternals, as Administrator) to capture the `OutputDebugStringA` messages from the hook.
3.  **Play the Game:** Perform actions in-game to trigger server messages.
4.  **Collect the Data:** The debug log will populate with `Opcode -> Handler Address` pairs. This is your primary discovery map.
    ```
    [Packet Discovery] Opcode: 0x003F -> Handler: Gw2-64.exe+99DFC0
    ```

### Stage 2: Static Analysis (The Fast Path)

With a Handler Address from the logger, you can find the schema almost instantly.

1.  **Go to the Handler in Ghidra:** Navigate to the Handler Address you discovered (e.g., `Gw2-64.exe+99DFC0`).

2.  **Find the Schema via Cross-Reference (XREF):**
    *   Find where this function's address is referenced in the code. Use Ghidra's "Show References to..." feature.
    *   You will find a reference that leads you directly into the **Master Dispatch Table** in the `.rdata` section. The assembly will look like this:
        ```assembly
        .rdata:01b5ecd0    addr   DAT_0253ff90      ; <-- This is the Schema Pointer
        .rdata:01b5ecd8    addr   Gs2c_ProtoDispatcher ; <-- This is your Handler Pointer
        ```
    *   The 8-byte pointer located immediately **before** your handler's pointer is the **Schema Pointer**.

3.  **Decode and Document:**
    *   You now have both the Handler Address and the Schema Address.
    *   Use the `KX_SMSG_Static_Decoder_RVA.lua` script with the schema's RVA to get the packet's structure.
    *   Analyze the handler's decompiled code to understand the purpose of the fields from the schema.
    *   Create a new Markdown file for the packet, documenting its purpose, both addresses, and the fully named structure.

### Alternate & Manual Methods

If a packet does not appear in the C++ logger, it is a "Fast Path" packet. These require manual tracing.

*   **The Hardware Breakpoint Trap:** The most reliable method is to perform an Array of Byte scan for a unique part of the packet's payload, set a hardware breakpoint on access, and trace the code that reads the data. This will lead you to the hardcoded logic inside `Msg::DispatchStream`.
*   **Manual Break-and-Check:** Set an unconditional breakpoint at the entry of `Msg::DispatchStream` (`Gw2-64.exe`+`FD18B0`). Repeatedly break and check the packet's opcode (found via the `[RDX+48]` pointer chain) until you trap the desired packet, then trace its execution with `F8`.

This finalized workflow represents the most efficient and accurate process for analyzing the entire SMSG protocol.