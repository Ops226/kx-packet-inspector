# CMSG_PING_RESPONSE (0x0006)

**Direction:** Client -> Server
**Status:** Confirmed (Dynamic & Static Analysis Backed)

## Summary

This packet is intended to be sent by the client as a direct response to a command from the server, specifically `SMSG_SERVER_COMMAND (0x0027)` with subtype 4. It functions as a "ping-pong" or acknowledgment packet, likely designed to confirm to the server that the client is responsive.

However, analysis of the builder function shows that the packet is only sent if a specific condition is met, which does not appear to happen during normal gameplay. This suggests the packet is part of a legacy, deprecated, or rarely used anti-AFK or client-state check mechanism.

## Behavior and Conditional Sending

The relationship between `SMSG_0x0027` and `CMSG_0x0006` is proven by the handler for the SMSG packet, which directly calls the builder for this CMSG packet.

1.  The server sends `SMSG_SERVER_COMMAND (0x0027)` with subtype 4, which contains a `uint32_t` value.
2.  The client's handler, `SMSG_Handler_PingResponse_Trigger`, receives this packet and calls `CMSG::BuildPingResponse`, passing the `uint32_t` value.
3.  Inside `CMSG::BuildPingResponse`, the code checks if this value is less than 5001 (`0x1389`).
4.  If the value is less than 5001, the function terminates early. The `MsgConn::QueuePacket` call is **never reached**.
5.  If the value is 5001 or greater, the function proceeds to call `MsgConn::QueuePacket` with opcode `0x0006`.

Live network logging shows that during normal gameplay, the server always sends values less than 5001 in the `SMSG_0x0027` packet, which explains why `CMSG_0x0006` is never observed being sent.

## Construction

*   **Builder Function:** **`CMSG::BuildPingResponse`** (`FUN_140245390`)
*   **Trigger:** This builder is called by **`SMSG_Handler_PingResponse_Trigger`** (`FUN_14023cf00`).
*   **Opcode:** `0x0006`

## Packet Structure

When the conditions for sending are met, the builder function sends a null payload.

```cpp
struct CMSG_0x0006_Payload {
    // empty
};
```

## Evidence

**Decompiled `CMSG::BuildPingResponse` (`FUN_140245390`):**
```c
void CMSG::BuildPingResponse(uint param_1)
{
  // The function takes the server_value as param_1.
  // If the value is less than 5001, the function exits via a non-returning call.
  if (param_1 < 0x1389) {
    /* WARNING: Subroutine does not return */
    thunk_FUN_1418c47c0(...);
  }

  // This line is only reached if param_1 >= 5001.
  MsgConn::QueuePacket(DAT_1426280f0, 0, 6, 0);
  return;
}
```

## Confidence

*   **Opcode:** High. Confirmed directly in the builder function's code path.
*   **Purpose:** High. Confirmed by tracing its trigger from the `SMSG_0x0027` handler and analyzing its conditional logic.
*   **Structure:** High. Confirmed to be an empty payload by the `MsgConn::QueuePacket` call within the builder.