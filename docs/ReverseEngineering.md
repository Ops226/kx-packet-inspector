# Reverse Engineering Notes for KX Packet Inspector

**Document Date:** 08 April 2025

This document outlines key findings, target functions, structure offsets, and ongoing challenges related to reverse engineering Guild Wars 2 (`Gw2-64.exe`) for the purpose of packet inspection.

## Table of Contents

*   [Target Functions](#target-functions)
    *   [Outgoing Packets (MsgSend)](#outgoing-packets-msgsend)
    *   [Incoming Packets (MsgDispatch)](#incoming-packets-msgdispatch)
    *   [Obsolete Target (MsgRecv - Low Level)](#obsolete-target-msgrecv---low-level)
*   [Key Data Structures & Offsets](#key-data-structures--offsets)
    *   [`MsgSendContext`](#msgsendcontext)
    *   [`MsgConn` Context](#msgconn-context)
    *   [Handler Info Structure](#handler-info-structure)
    *   [Message Definition Structure](#message-definition-structure)
*   [Hooking Strategy](#hooking-strategy)
    *   [Outgoing (CMSG)](#outgoing-cmsg)
    *   [Incoming (SMSG)](#incoming-smsg)
*   [Identified Packet Headers](#identified-packet-headers)
*   [Current Challenges & Next Steps](#current-challenges--next-steps)
*   [Toolchain](#toolchain)

## Target Functions

Function addresses are relative to the `Gw2-64.exe` module base. Patterns are IDA-style (space-separated hex, `?` for wildcard).

### Outgoing Packets (MsgSend)

*   **Internal Name:** `FUN_1412e9960` (Ghidra/IDA Auto-name)
*   **Pattern:** `40 ? 48 83 EC ? 48 8D ? ? ? 48 89 ? ? 48 89 ? ? 48 89 ? ? 4C 89 ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 48 8B ? E8`
*   **Purpose:** Prepares outgoing packets, potentially including encryption if `bufferState == 3`.
*   **Hook Point:** Function Entry (using MinHook).
*   **Context Arg:** `param_1` (RCX) points to `MsgSendContext`.

### Incoming Packets (MsgDispatch)

*   **Internal Name:** `FUN_1412e9390` (`Msg::DispatchStream`)
*   **Pattern:** `48 89 5C 24 ? 4C 89 44 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 8B 82`
*   **Purpose:** Processes an already decrypted/decompressed buffer containing one or more framed messages. Iterates through messages, looks up handlers, and calls the appropriate handler function for each message.
*   **Hook Points:** Mid-function, using SafetyHook MidHook, targeting the `MOV RDX, [RBP+STACK_OFFSET_MESSAGE_DATA_PTR]` instruction immediately preceding the handler calls at four distinct sites within the function:
    *   Offset `+0x219` (Contiguous Buffer, Dispatch Type 1)
    *   Offset `+0x228` (Contiguous Buffer, Dispatch Type 0)
    *   Offset `+0x3D4` (Wrapping Buffer, Dispatch Type 1)
    *   Offset `+0x3E3` (Wrapping Buffer, Dispatch Type 0)
    *   This ensures messages are captured regardless of internal buffer state or dispatch type within this function.
*   **Key Registers/Context at Hook Site:**
    *   `RBX`: Holds pointer to `MsgConn` context (`pMsgConn`).
    *   `RBP`: Frame pointer. Message data pointer (`local_50`) is read from `[RBP + STACK_OFFSET_MESSAGE_DATA_PTR]` (defined as -0x18 in `MessageHandlerHook.cpp`). (*Potential instability due to stack offset dependency*).
    *   `pMsgConn + 0x48`: Contains pointer to Handler Info Structure (`handlerInfoPtr`).
*   **Relevant Helper Functions Called Internally:**
    *   `FUN_1412e81c0`: Handler lookup (Input: Registry Ptr, MsgID; Output: Handler Info Ptr).
    *   `FUN_1412ed640`: Message data pointer lookup (Input: Processed Buffer Base, Size; Output: Message Data Ptr).

### Obsolete Target (MsgRecv - Low Level)

*   **Function Address (Approx):** `+0x012EA0C0` (Relative to base)
*   **Internal Name:** `FUN_1412ea0c0`
*   **Pattern:** `40 55 41 54 41 55 41 56 41 57 48 83 EC ? 48 8D 6C 24 ? 48 89 5D ? 48 89 75 ? 48 89 7D ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 45 ? 44 0F B6 12`
*   **Reason for Deprecation:** Hooking this function required external RC4 decryption and did not provide framed messages. The dispatcher (`FUN_1412e9390`) is a more suitable target for capturing individual, plaintext messages.

## Key Data Structures & Offsets

Offsets determined through static analysis (IDA/Ghidra) and dynamic analysis (memory views, logging). *All offsets are subject to change with game updates.*

### `MsgSendContext`

*(Relative to `param_1`/RCX passed to `FUN_1412e9960`)*

*   `+0xD0`: `uint8_t* currentBufferEndPtr`
*   `+0x108`: `int32_t bufferState` (Used by game, state 1=skip, 3=encrypt path)
*   `+0x398`: Start of data buffer relative to context pointer.

### `MsgConn` Context

*(Relative to `pMsgConn`/RDX passed to `FUN_1412e9390`)*

*   `+0x00`: `uint32_t connTypeId` (Hypothesized type ID, **observed as consistently 6 in-game and login - likely NOT the server type differentiator**).
*   `+0x18`: `void* registryPtr` (Pointer to message registry/protocol definition? **Observed as consistent between login/in-game - needs further investigation**).
*   `+0x40`: `uint32_t lastMsgId` (ID of message previously processed or currently being set up).
*   `+0x48`: `void* handlerInfoPtr` (Pointer to current 32-byte handler info structure, NULL between messages).
*   `+0x88`: `void* bufferBasePtr` (Base of internal ring buffer?).
*   `+0x94`: `uint32_t bufferReadIdx` (Current read offset in ring buffer).
*   `+0xA0`: `uint32_t bufferWriteIdx` (Current write offset/end of data in ring buffer).
*   `+0x108`: `int32_t bufferState` (Observed as 3 for encrypted connections handled by the *old* `FUN_1412ea0c0` hook. May or may not be relevant now).
*   `+0x110`: `void* funcPtr` (Function pointer? Virtual function? **Observed as consistent between login/in-game - needs further investigation**).
*   `+0x12C`: `RC4State rc4State` (Start of RC4 state used by `FUN_1412ed810`. **No longer directly used by current hook**).
*   `+0xC8`: `void* processedBufferBase` (Pointer to base of decrypted/decompressed buffer processed by dispatcher?).

### Handler Info Structure

*(32 bytes total, relative to `handlerInfoPtr`)*

*   `+0x00`: `uint16_t messageId` (Opcode, assuming 2 bytes).
*   `+0x08`: `void* msgDefPtr` (Pointer to Message Definition Structure).
*   `+0x10`: `int32_t dispatchType` (Control flow switch in dispatcher, 0 and 1 observed).
*   `+0x18`: `void* handlerFuncPtr` (Pointer to the C++ function handling this message).

### Message Definition Structure

*(Relative to `msgDefPtr`)*

*   `+0x20`: `uint32_t messageSize` (Size of the message payload data).

## Hooking Strategy

### Outgoing (CMSG)

*   **Method:** MinHook entry point hook on `FUN_1412e9960`.
*   **Data:** Reads from `MsgSendContext` using known offsets to get plaintext buffer before potential encryption.
*   **Status:** Stable and functional.

### Incoming (SMSG)

*   **Method:** SafetyHook mid-function hook (`MidHook`) inside `FUN_1412e9390`.
*   **Target:** Instructions `MOV RDX, [RBP+STACK_OFFSET_MESSAGE_DATA_PTR]` at offsets `+0x219`, `+0x228`, `+0x3D4`, and `+0x3E3` relative to function start (immediately before handler `call` instructions).
*   **Data Extraction:** Uses `SafetyHookContext` (`ctx`) to read `RBX` (for `pMsgConn`) and `RBP` (for `messageDataPtr` stack offset). Reads `handlerInfoPtr` from `pMsgConn+0x48`. Dereferences pointers using struct offsets to get `messageId`, `msgDefPtr`, and `messageSize`. **This extraction logic works consistently across all four hook sites.**
*   **Status:** Functional, captures individual plaintext messages from all identified call paths within this dispatcher. Relies on stack offset `STACK_OFFSET_MESSAGE_DATA_PTR` (`[RBP - 0x18]`) which could be fragile across game updates.

## Identified Packet Headers

See `PacketHeaders.h` for the current list of known `CMSG_HeaderId` and `SMSG_HeaderId` values. This is significantly incomplete, especially for SMSG.

## Current Challenges & Next Steps

1.  **Server Differentiation:** Determine how to reliably distinguish packets from the Game Server vs. Login/Auth Server.
    *   Investigate if the `pMsgConn` instance address differs between phases.
    *   Investigate the `pHandlerCtx` pointer (`R8` passed to dispatcher) for type information or different target objects.
    *   Re-examine `pMsgConn` structure for other potential differentiating fields.
    *   Update `PacketInfo`, `PacketProcessor`, and `PacketHeaders` to incorporate server type context once identified.
2.  **Opcode Identification:** Systematically identify more CMSG and especially SMSG opcodes by correlating logged packets with in-game actions and adding them to `PacketHeaders.h`.
3.  **Structure Analysis:** Begin reverse engineering the data payload (`PacketInfo.data`) for identified opcodes to create corresponding C++ structs.
4.  **Hook Completeness/Stability:**
    *   **Resolved (for FUN_1412e9390):** All four identified handler call preparation sites within the main dispatcher are now hooked, ensuring comprehensive capture of messages processed by this function. (Further investigation might be needed if *other* dispatch functions exist).
    *   Monitor the stability of using the stack offset `STACK_OFFSET_MESSAGE_DATA_PTR` (`[RBP - 0x18]`) for the message data pointer. If it breaks, investigate alternative methods (e.g., reading `RDX` directly before the `call` if possible with SafetyHook or other techniques).
5.  **Performance & Memory:** Implement packet log limiting. Evaluate if moving processing out of the hook callback into a separate thread is necessary.

## Toolchain

*   **Hooking:** MinHook (for entry points), SafetyHook (for mid-function/inline)
*   **Reverse Engineering:** IDA Pro / Ghidra
*   **Memory Analysis:** Cheat Engine (Structure Dissect), ReClass.NET
*   **Debugging:** Visual Studio Debugger, Console Output (`std::cout`, `OutputDebugStringA`)
*   **Language/UI:** C++, ImGui