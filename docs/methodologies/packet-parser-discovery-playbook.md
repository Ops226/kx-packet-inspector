# Packet Parser Discovery Playbook (Dynamic Analysis Workflow)

**Date:** 2025-08-05

## Objective

This document provides a concise, repeatable method for discovering packet structures based on the now-confirmed **dynamic dispatch system**. This playbook supersedes all previous versions and is aligned with the dynamic architecture described in `system-architecture.md`. **Static analysis alone is not sufficient.**

## Scope and Constraints

*   **Primary Environment:** A live game client with a debugger (e.g., Cheat Engine) attached.
*   **Secondary Environment:** Ghidra CodeBrowser on `Gw2-64.exe` for static analysis of dynamically discovered functions.
*   **Methodology:** Dynamic analysis to find the correct post-parse handler, followed by static analysis to decode the schema and logic.
*   **Evidence:** All findings must be backed by live memory addresses and supported by decompiled code from the identified handler.

## Core Concepts

The client's message handling is a multi-stage, dynamic process.

1.  **`Gs2c_SrvMsgDispatcher`:** The primary entry point that receives the raw message stream.
2.  **`Msg_ParseAndDispatch_BuildArgs`:** A schema "virtual machine" called by the dispatcher to parse raw bytes into a structured data tuple.
3.  **Dynamic Post-Parse Handlers:** Crucially, there is **no single master handler**. After parsing, `Gs2c_SrvMsgDispatcher` calls a function pointer that is resolved at runtime. This means different handlers can be active depending on the game's state. `Gs2c_PostParseDispatcher` is just one of these many possible handlers.

## Workflow: Dynamic Analysis First

### Phase 1: Identify the True Post-Parse Handler

The only reliable way to find the correct handler for a packet is to observe the dispatch in a live game client.

1.  **Log into the game world.**
2.  **Set a Breakpoint:** In your debugger (e.g., Cheat Engine), set a breakpoint on the `CALL` instruction inside `Gs2c_SrvMsgDispatcher` that executes the dynamic post-parse handler. As of this writing, the address is:
    `"Gw2-64.exe"+FD1ACD` (`call rax`)
3.  **Trigger the Packet:** Perform an in-game action to generate the network traffic you want to inspect.
4.  **Capture the Handler Address:** When the breakpoint hits, the `RAX` register will contain the live memory address of the true post-parse handler for that specific message. The `RDX` register will contain a pointer to the parsed data tuple. **Record both addresses.**

### Phase 2: Analyze the Handler and Find the Schema

Now, switch to your static analysis tool (Ghidra).

1.  **Locate the Handler Function:** Convert the live handler address from Cheat Engine into a static address in Ghidra (e.g., `140000000 + offset`). Go to this function.
2.  **Find the Schema:** The true handler receives the parsed data tuple as an argument. To discover the schema that created this tuple, you must trace backward from the initial `Gs2c_SrvMsgDispatcher` breakpoint. The schema address is passed as an argument to the schema parser (`"Gw2-64.exe"+FD43C0`) just before the handler is called.

### Phase 3: Decode the Schema and Logic

1.  **Decode the Schema:** Navigate to the schema's data address in Ghidra. Use the typecode definitions from `system-architecture.md` to determine the exact layout of the packet.
2.  **Analyze Handler Logic:** With the structure known, analyze the decompiled code of the handler you discovered in Phase 1. This will reveal the semantic meaning of the packet's fields.

### Phase 4: Document Your Findings

1.  **Create a New Packet Document:** Create a new `.md` file in the `packets` directory.
2.  **Document the Packet Structure:** Create a field table detailing the offset, type, name, and description of each field.
3.  **Document the Handler Logic:** Provide a summary of the handler's behavior and link to its address. **Crucially, note that this handler was discovered dynamically.**

## Example: A Message Handled by `FUN_1413e5d90`

*   **Dynamic Discovery:** A breakpoint at `"Gw2-64.exe"+FD1ACD` is triggered. The `RAX` register contains the address for `FUN_1413e5d90`, and `RDX` points to a data tuple beginning with bytes `FE 03 ...`.
*   **Static Analysis:** We navigate to `FUN_1413e5d90` in Ghidra. We analyze its code to see how it uses the fields from the data tuple passed to it. We then trace back to the schema parser call to find the schema that defines this structure.
*   **Conclusion:** This dynamic approach allows us to correctly associate the packet data with its specific handler, `FUN_1413e5d90`, bypassing incorrect assumptions about a single static dispatcher.

## Conclusion

This dynamic-first workflow is the only reliable method for reverse engineering the game's network protocol. Static analysis is a powerful tool, but it must be guided by live, in-game observation to ensure accuracy.