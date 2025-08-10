# CMSG_LOGOUT_TO_CHAR_SELECT (0x0023)

**Direction**: Client -> Server

## Summary

This packet is sent by the client when the user logs out to the character select screen. In this context, it acts as a simple command packet with a minimal 2-byte payload.

**Note:** This opcode appears to be multi-purpose. Other functions in the client send opcode `0x0023` with a different, more complex pointer-based payload in other game contexts.

## Structure (On Logout)

This is the observed on-wire structure when logging out.

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | `uint16` | Opcode | The opcode of the packet itself, `0x0023`. |

### Full Log Entry (On Logout)
`22:17:00.599 [S] CMSG_UNKNOWN Op:0x0023 | Sz:2 | 23 00`

## Emitters (Known Functions that Send this Opcode)

### A) Logout Emitter (Observed via Logging)

The simple 2-byte version of this packet is sent when the player clicks the "Log Out" button to return to the character select screen. The exact builder function for this specific action has not yet been identified, but its existence is confirmed by network logs.

### B) Complex Pointer Emitter (`FUN_14023C240`)

A different version of this packet is sent by `FUN_14023C240`. In this context, it sends a pointer to a data structure inside a larger game object. The purpose of this variant is still unknown.

**Decompiled highlights:**
```c
// From FUN_14023C240
*(u64*)(param_1 + 0x38) = *(u64*)(param_1 + 0x24);
MsgConn::QueuePacket(DAT_1426280F0, 0, 0x23, (u64*)(param_1 + 0x38));
```

## Confidence

*   **Opcode:** High. Confirmed via both static analysis and live logs.
*   **Structure (on logout):** High. Confirmed by multiple live logs.
*   **Name:** High. The packet is consistently sent during the logout process.
