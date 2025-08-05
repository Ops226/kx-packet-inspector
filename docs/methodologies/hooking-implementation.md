# Hooking Implementation Details

This document details the practical implementation of the packet hooking mechanism used by the KX Packet Inspector. It covers the target functions, data structures, and specific strategies used to capture network messages.

## Hooking Strategy

### Outgoing (CMSG)

*   **Method:** MinHook entry point hook on the `MsgSend` function.
*   **Data:** Reads from a `MsgSendContext` structure to get the plaintext buffer before potential encryption.
*   **Status:** Stable and functional.

### Incoming (SMSG)

*   **Method:** SafetyHook mid-function hook (`MidHook`) inside the main message dispatcher function (`Msg::DispatchStream`).
*   **Target:** The hook specifically targets the `MOV RDX, [RBP+STACK_OFFSET_MESSAGE_DATA_PTR]` instruction at multiple offsets within the function. This instruction is responsible for loading the pointer to the message data just before the handler function is called.
*   **Data Extraction:** The hook uses the `SafetyHookContext` to read registers (`RBX` and `RBP`) to get pointers to the connection context (`pMsgConn`) and the message data. From there, it dereferences pointers using the known structure layouts to extract the message ID, size, and payload.
*   **Status:** Functional and captures plaintext messages from all identified call paths within the main dispatcher. **Note:** This method's reliance on a hardcoded stack offset (`[RBP - 0x18]`) is potentially fragile and may break with future game updates.

## Target Functions

Function signatures are provided to help locate these functions after a game update.

### Outgoing Packets (MsgSend)

*   **Purpose:** Prepares outgoing packets for sending.
*   **Signature:** `40 ? 48 83 EC ? 48 8D ? ? ? 48 89 ? ? 48 89 ? ? 48 89 ? ? 4C 89 ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 48 8B ? E8`

### Incoming Packets (MsgDispatch)

*   **Internal Name:** `Msg::DispatchStream`
*   **Purpose:** Processes a buffer of one or more decrypted, framed messages and calls the appropriate handler for each.
*   **Signature:** `48 89 5C 24 ? 4C 89 44 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 8B 82`

### Obsolete Target (MsgRecv - Low Level)

*   **Signature:** `40 55 41 54 41 55 41 56 41 57 48 83 EC ? 48 8D 6C 24 ? 48 89 5D ? 48 89 75 ? 48 89 7D ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 45 ? 44 0F B6 12`
*   **Reason for Deprecation:** Hooking this function was abandoned because it operates on the raw, encrypted message buffer. This required performing RC4 decryption manually and did not provide cleanly framed messages. The current approach of hooking the `Msg::DispatchStream` function is far more effective as it provides access to individual, plaintext messages after decryption and framing have already been handled by the game client.

## Key Data Structures & Offsets

These are the key data structures and their offsets used to extract information at the hook points. **Warning:** These offsets are subject to change with any game update.

### `MsgConn` Context

This structure holds the context for the network connection.

*   `+0x48`: `void* handlerInfoPtr` (Pointer to current Handler Info Structure)

### Handler Info Structure

This 32-byte structure contains information about the message and its handler.

*   `+0x00`: `uint16_t messageId` (The packet's opcode)
*   `+0x08`: `void* msgDefPtr` (Pointer to the Message Definition Structure)
*   `+0x18`: `void* handlerFuncPtr` (Pointer to the C++ function that handles this message)

### Message Definition Structure

*   `+0x20`: `uint32_t messageSize` (The size of the message payload)

## Toolchain

*   **Hooking:** MinHook, SafetyHook
*   **Reverse Engineering:** Ghidra, IDA Pro
*   **Memory Analysis:** Cheat Engine, ReClass.NET
*   **Debugging:** Visual Studio Debugger
