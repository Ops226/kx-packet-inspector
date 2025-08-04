# Reverse Engineering Notes for KX Packet Inspector

**Document Date:** 04 August 2025 (Definitive, Corrected, and Consolidated Edition)

This document outlines the definitive findings, architectural patterns, and recommended workflow for reverse engineering Guild Wars 2 (`Gw2-64.exe`) network packets. The system is a highly complex, multi-layered, polymorphic dispatch system.

## Table of Contents

*   [**Core Philosophy: The Memory Correlation Method**](#core-philosophy-the-memory-correlation-method)
*   [**System Architecture Deep Dive**](#system-architecture-deep-dive)
    *   [Entry Point: The Initial Dispatcher (`SrvMsgDispatcher`)](#entry-point-the-initial-dispatcher-srvmsgdispatcher)
    *   [The Handler Chain: From Protocol to Opcode](#the-handler-chain-from-protocol-to-opcode)
    *   [Key Discovery 1: Packet Polymorphism (Size-Based Dispatch)](#key-discovery-1-packet-polymorphism-size-based-dispatch)
    *   [Key Discovery 2: The Global Parser Table](#key-discovery-2-the-global-parser-table)
    *   [Router Chain & VTable Jump Hub](#router-chain--vtable-jump-hub)
*   [**Key Packet Structures (Confirmed)**](#key-packet-structures-confirmed)
    *   [SMSG 0x0008 - Agent Update (Tiny Movement Variant)](#smsg-0x0008---agent-update-tiny-movement-variant)
*   [**Practical Application & Workflow**](#practical-application--workflow)
    *   [Recommended Hooking Strategy](#recommended-hooking-strategy)
    *   [The Definitive Analysis Workflow](#the-definitive-analysis-workflow)
*   [**Reference: Key Functions & Data (with Signatures)**](#reference-key-functions--data-with-signatures)
*   [**Reference: Original Data Structure Findings**](#reference-original-data-structure-findings)
*   [**Toolchain**](#toolchain)
*   [**Current Challenges & Next Steps**](#current-challenges--next-steps)

## Core Philosophy: The Memory Correlation Method

While static analysis in Ghidra was instrumental in mapping the system's architecture, the extreme levels of indirection (vtables, callbacks, jump tables) make it an unreliable method for finding specific packet parsers. The most successful and recommended approach is **Memory Correlation**.

This workflow is the foundation of all further research:
1.  **Capture:** Use `kx-packet-inspector` to get a clean log of the target packet.
2.  **Freeze:** Use a debugger (e.g., Cheat Engine) to pause the game at a relevant moment.
3.  **Find:** Locate the corresponding game objects (e.g., the player character) in live memory.
4.  **Compare:** Correlate the raw hex data of the captured packet with the live values in memory to deduce the packet's structure.

## System Architecture Deep Dive

### Entry Point: The Initial Dispatcher (`SrvMsgDispatcher`)

All incoming, decrypted server messages pass through a single primary function.

*   **Internal Name:** `FUN_140fd18b0` (`Msg::DispatchStream`)
*   **Purpose:** This function processes a buffer of one or more framed messages. It does not contain game-logic-specific parsing but is responsible for identifying the protocol context (e.g., `gs2c` for Game Server) and dispatching to the next layer in the chain.

### The Handler Chain: From Protocol to Opcode

The processing of a packet is a chain of command, not a single function call.
1.  **`SrvMsgDispatcher`** receives the raw buffer and the main `MsgConn` context.
2.  It extracts a protocol-specific object from the context and calls a virtual function on it.
3.  This leads to the **`Gs2c_ProtoDispatcher`**, the specialist for the game server protocol.
4.  This, in turn, calls the **`Gs2c_OpcodeRouter`**, which is the primary routing hub for a vast number of `gs2c` opcodes. This function contains the main `switch` or `if-else` logic that routes packets based on their opcode and size.
5.  Finally, the Master Switchboard calls a **final handler function** responsible for parsing a specific packet type.

### Key Discovery 1: Packet Polymorphism (Size-Based Dispatch)

A single opcode can represent multiple distinct packet structures. The game uses the **total packet size** as the primary discriminator before dispatching to a specialized parser.

*   **Example (Opcode `0x0008` / `SMSG_AGENT_UPDATE`):**
    *   **Large Variant (116 bytes):** Used for full agent spawns (health, appearance, etc.). Handled by `SMSG_Handler_AgentUpdate_0x0008`.
    *   **Small Variant (7 bytes):** A high-frequency packet for simple state changes/movement. Handled by the separate `SMSG_Handler_AgentMovementUpdate` -> `Parse_MovementUpdate_Payload` chain.

### Key Discovery 2: The Global Parser Table

For more complex, polymorphic packets (like the 116-byte agent spawn), the system uses a data-driven factory pattern.

*   **Global Table Address:** `DAT_142578428`
*   **Purpose:** This is a global, runtime-populated table that stores pointers to specialized "parser objects."
*   **Mechanism:** The packet contains a **Subtype ID** (often a `uint32_t` at offset `+0x0C`). This ID is used as an index into this table to retrieve the specific parser object for that subtype. The game then calls a virtual function on that object to perform the final parsing.

### Router Chain & VTable Jump Hub

The detailed analysis of the dispatch chain reveals highly optimized forwarding and a central vtable-based jump hub:

1)  **`SrvMsgDispatcher`** (Msg::DispatchStream)
    *   Decompile evidence (abridged): Calls `Msg_ParseAndDispatch_BuildArgs(...)`, then invokes a function pointer from the protocol object (`param_2 + 0x48`) with an argument set built from the message.
    *   Role: Framed message loop and orchestration.

2)  **`Gs2c_ProtoDispatcher`**
    *   Decompile evidence: Extracts two 32-bit values (likely length/opcode or related header fields) from the parsed message block (`param_2+2`, `param_2+6`) and forwards to the router with a protocol state object.

3)  **`Gs2c_OpcodeRouter`**
    *   Decompile evidence: Stores header fields into the protocol object (`param_1+0xAB0`, `param_1+0xAB4`), then tail-calls a generic dispatch helper (`FUN_140250880`) with a callback pointer.

4)  **Jump Hub via VTable: `FUN_140253698`**
    *   Decompile evidence: `(**(code **)(*param_1 + 0xB8))();`
    *   Role: This is an indirect virtual call at vtable slot `+0xB8`. This is the hub used by the generic dispatcher to jump into the massive opcode-specific switchboard (or a dispatcher object implementing the switch).

## Key Packet Structures (Confirmed)

### SMSG 0x0008 - Agent Update (Tiny Movement Variant)

This struct has been derived through extensive debugging and correlation with live memory. It represents the common 7-byte update packet.

*   **Handler:** `SMSG_Handler_AgentMovementUpdate` (`1410f4560`)
*   **Parser:** `Parse_MovementUpdate_Payload` (`1410eb740`) - This function ultimately reads the following fields from the packet buffer.
*   **Packet Size:** 7 bytes

```cpp
// Tiny movement subtype 0 body layout (Pass 1 spec)
struct TinyMovementPayload_V0 {
    // Note: The first bytes are part of the header that the dispatcher already consumed.
    // This struct starts from the first byte of the 'payload' that Parse_MovementUpdate_Payload cares about.

    uint16_t dx_q88;  // Offset 0x00: Delta X (fixed-point Q8.8, convert to float by / 256.0f)
    uint16_t dy_q88;  // Offset 0x02: Delta Y (fixed-point Q8.8, convert to float by / 256.0f)
    uint16_t dz_q8_8; // Offset 0x04: Delta Z (fixed-point Q8.8, convert to float by / 256.0f)
    uint8_t flags;    // Offset 0x06: Flags (e.g., jump state, facing changes). Bit0 toggles iteration branch at 0x141447188.
};

// Example conversion struct
struct TinyMovementV0f {
    float dx;
    float dy;
    float dz;
    uint8_t flags;
};

// Example Parser Stub (Bounds-Checked, For Integration)
// buf points to the start of the tiny-movement packet body for subtype 0.
// len is the number of bytes available from buf onward.
// parse_ctx_subtype is the subtype value from parse_ctx+0x0C (must be 0 for this variant).
inline bool ParseTinyMoveSubtype0(const uint8_t* buf, size_t len, uint32_t parse_ctx_subtype, TinyMovementV0f& out) {
    if (!buf) return false;
    // Note: This subtype check is conceptual; the tiny 7-byte packet doesn't have 0x0C offset for subtype.
    // The handler already routes based on size.
    if (parse_ctx_subtype != 0) return false; // Default to 0 for this concrete variant

    // Body requires bytes up to offset 0x06 inclusive for this 7-byte packet.
    constexpr size_t kFlagsOff = 0x06;
    constexpr size_t kMinSize  = kFlagsOff + 1; // 7 bytes total
    if (len < kMinSize) return false;

    auto rd16 = [&](size_t off) -> int16_t {
        // Little-endian 16-bit signed
        int16_t v;
        std::memcpy(&v, &buf[off], sizeof(v));
        return v;
    };

    out.dx    = static_cast<float>(rd16(0x00)) / 256.0f;
    out.dy    = static_cast<float>(rd16(0x02)) / 256.0f;
    out.dz    = static_cast<float>(rd16(0x04)) / 256.0f;
    out.flags = buf[kFlagsOff];
    return true;
}

// Flags bit masks observed/assumed for Pass 1 (from TEST DIL,0x1)
constexpr uint8_t kTM0_Flag_Bit0 = 0x01; // toggles iteration branch at 0x141447188
```

## Practical Application & Workflow

### Recommended Hooking Strategy

*   **Outgoing (CMSG):** Hook the entry point of the `MsgSend` function (`FUN_1412e9960`). This is stable and provides the plaintext buffer before potential encryption.
*   **Incoming (SMSG):** A SafetyHook mid-function hook inside `FUN_1412e9390` (`Msg::DispatchStream`) is the perfect location for *capturing* raw, decrypted, and framed packets. Target the `MOV RDX, [RBP+STACK_OFFSET_MESSAGE_DATA_PTR]` instruction at specific offsets (`+0x219`, `+0x228`, etc.) before the handler calls.

### The Definitive Analysis Workflow

1.  **Capture & Isolate:** Use `kx-packet-inspector` to get a clean log. Note the **opcode** and **size** of the target packet.
2.  **Find the Handler via Debugging:** Static analysis has proven unreliable. Use a debugger to trace execution:
    *   Start with a breakpoint at the `Gs2c_Master_Opcode_Switch` (see signature below for how to find its *caller*).
    *   Use a conditional breakpoint (or manual checking) on the opcode to find the correct branch.
    *   Use "Step Into" (`F7`) to follow the call to the final handler.
3.  **Decompile and Correlate:** Decompile the final handler in Ghidra. Compare its direct memory reads (`*(type*)(buffer + offset)`) with live memory values from Cheat Engine to deduce the C++ struct.

## Reference: Key Functions & Data (with Signatures)

*   **`SrvMsgDispatcher`** (Initial Dispatcher)
    *   Ghidra: `140fd18b0`
    *   Signature: `48 89 5C 24 ? 4C 89 44 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 8B 82`

*   **`Gs2c_ProtoDispatcher`** (Game Server Protocol Specialist)
    *   Ghidra: `14099dfc0`
    *   Signature: `40 53 48 83 EC 20 48 8B DA E8 ? ? ? ? 44 8B 43 06`

*   **`Gs2c_OpcodeRouter`** (Protocol router)
    *   Ghidra: `14099abf0`
    *   Behavior: Writes header fields into protocol object (`param_1+0xAB0`/`+0xAB4`), then tail-calls dispatcher helper with jump hub.

*   **`VTable Jump Hub`** (Generic dispatch entry via vtable slot +0xB8)
    *   Ghidra: `140253698` → `(**(code **)(*param_1 + 0xB8))();`
    *   Discovery Method: `Gs2c_OpcodeRouter` tail-calls `FUN_140250880(..., FUN_140253698)`, which ultimately invokes vtable+0xB8.

*   **`Gs2c_Master_Opcode_Switch`** (Main `gs2c` Opcode Router)
    *   **Discovery Method:** This function is the destination of the `JMP qword ptr [RAX + 0xB8]` instruction at `14025369B`.
    *   **Signature:** `48 89 5C 24 ? 57 48 83 EC ? 48 8B DA E8 ? ? ? ? 44 8B C3` (This signature should point to the actual implementation).

*   **`SMSG_Handler_AgentMovementUpdate`** (Entry for tiny movement packets)
    *   Ghidra: `1410f4560`
    *   Signature: `48 83 EC ? 83 89 ? ? ? ? ? 48 85 D2`

*   **`Parse_MovementUpdate_Payload`** (Actual parser for tiny movement)
    *   Ghidra: `1410eb740`
    *   Signature: `48 83 EC ? 48 8B 0D ? ? ? ? 48 8B D9 E8 ? ? ? ? 48 8B F8 48 85 C0`

*   **`SMSG_Handler_AgentUpdate_0x0008`** (Entry for 116-byte full agent updates)
    *   Ghidra: `14093f7e0`
    *   Signature: `48 83 EC ? 83 FA ? 0F 85 ? ? ? ? E8`

*   **`DAT_142578428`** (Global table for polymorphic parser objects)
    *   Ghidra: `142578428`
    *   **Discovery Method:** Set a memory write breakpoint on `(Base Address of Gw2-64.exe) + 0x2578828` and trigger game initialization. The instruction that writes to it will be in the table initializer.

## Reference: Original Data Structure Findings

*These offsets from the original analysis remain a valuable reference for the `Msg::DispatchStream` hook but may be fragile across updates.*

*   **`MsgConn` Context** (Relative to `pMsgConn`/RDX in `Msg::DispatchStream`):
    *   `+0x48`: `void* handlerInfoPtr` (Pointer to `Handler Info Structure`)
*   **`Handler Info Structure`** (Relative to `handlerInfoPtr`):
    *   `+0x00`: `uint16_t messageId` (Opcode)
    *   `+0x08`: `void* msgDefPtr` (Pointer to `Message Definition Structure`)
    *   `+0x18`: `void* handlerFuncPtr` (Pointer to a handler function)
*   **`Message Definition Structure`** (Relative to `msgDefPtr`):
    *   `+0x20`: `uint32_t messageSize`

## Toolchain

*   **Hooking:** MinHook, SafetyHook
*   **Reverse Engineering (Static):** Ghidra
*   **Memory Analysis & Debugging (Dynamic):** Cheat Engine
*   **Language/UI:** C++, ImGui

## Current Challenges & Next Steps

1.  **Structure Analysis:** Continue building out C++ structs for identified opcodes, paying close attention to size and subtype polymorphism. The `SMSG_UPDATE_BLOCK` (opcode 1) is a critical candidate for future analysis due to its high frequency and varying sizes.
2.  **Hook Stability:** Monitor the stability of the `STACK_OFFSET_MESSAGE_DATA_PTR` (`[RBP - 0x18]`) in the `Msg::DispatchStream` hook. If it breaks, a more robust method of finding the message data pointer may be needed.
3.  **Server Differentiation:** Re-investigate how to reliably distinguish packets from Game Server vs. Login/Auth Server using the `MsgConn` context (`pMsgConn+0x00` `connTypeId`) now that the deeper architecture is understood.
