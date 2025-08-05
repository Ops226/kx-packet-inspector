# System Architecture

This document provides a high-level overview of the Guild Wars 2 network message dispatch system, from the initial entry point to the final packet-specific handler.

## Core Concepts

The system is a highly complex, multi-layered, polymorphic dispatch system. The processing of a packet is a chain of command, not a single function call.

### Key Architectural Patterns

*   **Packet Polymorphism (Size-Based Dispatch):** A single opcode can represent multiple distinct packet structures. The game uses the **total packet size** as the primary discriminator before dispatching to a specialized parser.
*   **Global Parser Table:** For more complex, polymorphic packets, the system uses a data-driven factory pattern. A global, runtime-populated table stores pointers to specialized "parser objects." A **Subtype ID** within the packet is used as an index into this table to retrieve the specific parser for that subtype.

## The Dispatch Chain

All incoming, decrypted server messages pass through a single primary function and are routed through a series of dispatchers and routers.

1.  **`SrvMsgDispatcher` (Initial Dispatcher)**
    *   **Role:** This function processes a buffer of one or more framed messages. It does not contain game-logic-specific parsing but is responsible for identifying the protocol context (e.g., `gs2c` for Game Server) and dispatching to the next layer in the chain.
    *   **Signature:** `48 89 5C 24 ? 4C 89 44 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 8B 82`

2.  **`Gs2c_ProtoDispatcher` (Game Server Protocol Specialist)**
    *   **Role:** This is the specialist for the game server protocol. It extracts header fields (like length and opcode) from the message and forwards them to the opcode router.
    *   **Signature:** `40 53 48 83 EC 20 48 8B DA E8 ? ? ? ? 44 8B 43 06`

3.  **`Gs2c_OpcodeRouter` (Protocol Router)**
    *   **Role:** This function acts as a routing hub for a vast number of `gs2c` opcodes. It stores the header fields into a protocol object and then tail-calls a generic dispatch helper, passing a pointer to the VTable Jump Hub.

4.  **VTable Jump Hub (Generic Dispatch)**
    *   **Role:** This is an indirect virtual call at a fixed vtable slot (`+0xB8`). This hub is used by the generic dispatcher to jump into the massive opcode-specific switchboard (or a dispatcher object implementing the switch).
    *   **Decompiled Evidence:** `(**(code **)(*param_1 + 0xB8))();`

5.  **Final Handler**
    *   **Role:** Finally, the master switchboard calls a final handler function responsible for parsing a specific packet type.

This chain validates the `SrvMsgDispatcher` -> `Gs2c_ProtoDispatcher` -> `Gs2c_OpcodeRouter` -> VTable Jump Hub model.
