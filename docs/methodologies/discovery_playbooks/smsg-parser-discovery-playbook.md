# Packet Parser Discovery Playbook (SMSG Dynamic Analysis Workflow)

**Date:** 2025-08-05

## Objective

This document provides a concise, repeatable method for discovering Server-to-Client (SMSG) packet structures based on the confirmed **dynamic dispatch system**. This playbook supersedes all previous versions and is aligned with the dynamic architecture described in `system-architecture.md`. **Static analysis alone is not sufficient** for handler discovery.

## Scope and Constraints

*   **Primary Environment:** A live game client with a debugger (e.g., Cheat Engine) attached.
*   **Secondary Environment:** Ghidra CodeBrowser on `Gw2-64.exe` for static analysis of dynamically discovered functions.
*   **Methodology:** Dynamic analysis to find the correct post-parse handler, followed by static analysis to decode the schema and handler logic.
*   **Evidence:** All findings must be backed by live memory addresses and supported by decompiled code from the identified handler.

## Core Concepts

The client's incoming message handling is a multi-stage, dynamic process.

1.  **`Msg::DispatchStream`:** The primary entry point that receives the raw message stream.
2.  **`MsgUnpack::ParseWithSchema`:** A schema "virtual machine" called by the dispatcher to parse raw bytes into a structured data tuple.
3.  **Dynamic Post-Parse Handlers:** Crucially, there is **no single master handler**. After parsing, `Msg::DispatchStream` calls a function pointer that is resolved at runtime. This means different handlers can be active depending on the game's state. `Portal::DispatchMessage` is just one of these many possible handlers.

## Key Code Signatures for Dynamic Analysis

To reliably find the correct locations for dynamic analysis after a game update, we use the following code signatures. These can be used with a memory scanner to find the new addresses.

### SMSG Dynamic Handler Dispatch (`call rax`)

This is the most important location for SMSG analysis. It is the instruction that calls the dynamically resolved post-parse handler. Placing a breakpoint here allows capturing the handler address from the `RAX` register.

* **Last Known Address:** `"Gw2-64.exe"+FD1ACD`
* **Byte Signature (Pattern):** `48 8B 41 18 41 8B 0C 24 48 8B 55 E8 FF D0`
* **Assembly Context:**

  ```assembly
  mov rax,[rcx+18]      ; Load handler function pointer into RAX
  mov ecx,[r12]         ; Load first argument for handler
  mov rdx,[rbp-18]      ; Load second argument (pointer to parsed data)
  call rax              ; Execute the handler
  ```

### SMSG Schema Parser Call

This is the instruction that calls the schema VM to parse the raw packet data. Placing a breakpoint here allows capturing the schema address.

* **Last Known Address:** `"Gw2-64.exe"+FD1A47`
* **Assembly Context:** `call MsgUnpack::ParseWithSchema`

## Workflow: Dynamic Analysis First

### Phase 1: Identify the True Post-Parse Handler

The only reliable way to find the correct handler for an SMSG packet is to observe the dispatch in a live game client.

1.  **Log into the game world.** This ensures the full game connection is active.
2.  **Set a Breakpoint:** In your debugger (e.g., Cheat Engine), set a breakpoint on the `CALL` instruction inside `Msg::DispatchStream` that executes the dynamic post-parse handler. As of this writing, the address is:
    `"Gw2-64.exe"+FD1ACD` (`call rax`)
    This is the point immediately after `MsgUnpack::ParseWithSchema` returns, where `RAX` holds the pointer to the dynamically chosen handler.
3.  **Trigger the Packet:** Perform an in-game action to generate the specific network traffic you want to inspect (e.g., move your character for `SMSG_PLAYER_STATE_UPDATE`).
4.  **Capture the Handler Address:** When the breakpoint hits, the `RAX` register will contain the live memory address of the true post-parse handler for that specific message. The `RDX` register will contain a pointer to the parsed data tuple. **Record both addresses.**
5.  **Examine the Parsed Data:** Using the `RDX` pointer, inspect the first few bytes of the parsed data tuple. This often provides a "type" or "ID" that helps uniquely identify the message (e.g., `FE 03` for the `Marker::Cli::ProcessAgentMarkerUpdate`).

### Phase 2: Analyze the Handler and Find the Schema

Now, switch to your static analysis tool (Ghidra).

1.  **Locate the Handler Function:** Convert the live handler address from your debugger into a static address in Ghidra (e.g., `140000000 + offset`). Go to this function.
2.  **Identify the Schema:** The handler function (which received the parsed data tuple) will typically perform its logic by reading fields from this tuple. To discover the schema that created this tuple, you must trace backward from the `Msg::DispatchStream` breakpoint. The schema address is passed as an argument to the schema parser (`"Gw2-64.exe"+FD43C0`) just before the handler is dynamically called.

### Phase 3: Decode the Schema and Understand Handler Logic

1.  **Decode the Schema:** Navigate to the schema's data address in Ghidra. Use the typecode definitions from `system-architecture.md` to determine the exact layout of the packet's parsed data.
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

## Conclusion

This dynamic-first workflow is the only reliable method for reverse engineering the game's incoming network protocol. Static analysis is a powerful tool, but it must be guided by live, in-game observation to ensure accuracy and account for dynamic dispatch.