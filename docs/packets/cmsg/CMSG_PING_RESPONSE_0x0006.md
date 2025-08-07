# CMSG_PING_RESPONSE (0x0006)

**Direction:** Client -> Server
**Status:** Confirmed (Dynamic & Static Analysis Backed)

## Summary

This packet is sent by the client as a direct response to a command from the server, specifically `SMSG_SERVER_COMMAND (0x0027)` with subtype 4. It functions as a "ping-pong" or acknowledgment packet, confirming to the server that the client is responsive.

Although the server provides a value to be echoed, the client-side builder function that sends this packet currently sends an empty (null) payload, suggesting the value is only used for a preliminary check and not transmitted.

## Construction

*   **Builder Function:** **`CMSG::BuildPingResponse`** (`FUN_140245390`, previously `CMSG_Build_0x0006_Empty`)
*   **Trigger:** This builder is called by **`SMSG_Handler_PingResponse_Trigger`** (`FUN_14023cf00`), which is the handler for `SMSG_SERVER_COMMAND (0x0027)` subtype 4.
*   **Opcode:** `0x0006`

## Packet Structure

Despite being passed a `uint32_t` value from the triggering SMSG packet, the builder function sends a null payload.

```cpp
struct CMSG_0x0006_Payload {
    // empty
};
```

## Evidence

The relationship between `SMSG_0x0027` and `CMSG_0x0006` is proven by the handler for the SMSG packet, which directly calls the builder for this CMSG packet.

**Decompiled `SMSG_Handler_PingResponse_Trigger` (`FUN_14023cf00`):**
```c
void SMSG_Handler_PingResponse_Trigger(undefined8 param_1, longlong param_2)
{
  // param_2 is the parsed payload of the SMSG_0x0027 packet.
  // The handler extracts the value from offset +2 of the incoming packet...
  uint32_t server_value = *(uint *)(param_2 + 2);
  
  // ...and passes it to the builder for CMSG_0x0006.
  CMSG::BuildPingResponse(server_value);
}
```

**Decompiled `CMSG::BuildPingResponse` (`FUN_140245390`):**
```c
void CMSG::BuildPingResponse(uint param_1)
{
  // The function takes the server_value as param_1 but only uses it for a check.
  if (param_1 < 0x1389) {
    // ... some logic ...
  }

  // The call to queue the packet sends a null payload (0).
  MsgConn::QueuePacket(DAT_1426280f0, 0, 6, 0);
  return;
}
```

## Confidence

*   **Opcode:** High. Confirmed directly in the builder function.
*   **Purpose:** High. Confirmed by tracing its trigger from the `SMSG_0x0027` handler.
*   **Structure:** High. Confirmed to be an empty payload by the `MsgConn::QueuePacket` call within the builder.