# SMSG_SERVER_COMMAND (0x0027)

**Direction:** Server -> Client
**Status:** Confirmed (Dynamic Analysis Backed)

## Summary

`SMSG_0x0027` is a multi-purpose command packet sent by the server to trigger specific logic paths on the client. At least two distinct variants have been identified, each with its own unique payload and a dedicated handler function. The packet's behavior is determined by its subtype, which is the first field in the payload.

## Variants and Handlers

### Subtype 0x0003 (2-byte Payload) - Performance Trigger

This variant acts as a simple trigger for an internal client-side event related to graphics performance.

*   **Log Entry:** `10:19:28.207 [R] SMSG_UNKNOWN Op:0x0027 | Sz:2 | 03 00`
*   **Handler:** **`SMSG_Handler_Performance_Trigger`** (`FUN_14023cea0`)
*   **Purpose:** The handler retrieves a value from the graphics performance system (`GrPerf::GetCounterValue`) and then uses a generic builder function (`CMSG::BuildAndSendPacket`) to send a new CMSG packet, likely reporting this performance metric back to the server.

#### Layout (Subtype 0x0003)

| Offset | Type | Name | Notes |
|---|---|---|---|
| 0x00 | `uint16` | subtype | Always `0x0003`. |

---

### Subtype 0x0004 (6-byte Payload) - Ping Response Trigger

This variant is a server command that instructs the client to immediately send a response packet (`CMSG_0x0006`). This is a classic "ping-pong" or state-check mechanism.

*   **Log Entry:** `10:19:28.273 [R] SMSG_UNKNOWN Op:0x0027 | Sz:6 | 04 00 44 00 00 00`
*   **Handler:** **`SMSG_Handler_PingResponse_Trigger`** (`FUN_14023cf00`)
*   **Purpose:** The handler reads the `value` field from this packet's payload and immediately calls **`CMSG::BuildPingResponse`** to send a `CMSG_0x0006` packet containing the value back to the server.

#### Layout (Subtype 0x0004)

| Offset | Type | Name | Notes |
|---|---|---|---|
| 0x00 | `uint16` | subtype | Always `0x0004`. |
| 0x02 | `uint32` | value | A value to be sent back to the server in a `CMSG_0x0006` packet. |

## Evidence

The distinct handlers for each subtype were discovered via dynamic analysis by hooking the post-parse dispatch call within `Msg::DispatchStream`.

**Handler for Subtype 0x0003:**
`[Packet Discovery] Opcode: 0x0027 -> Handler: Gw2-64.exe+23CEA0` (`SMSG_Handler_Performance_Trigger`)

**Handler for Subtype 0x0004:**
`[Packet Discovery] Opcode: 0x0027 -> Handler: Gw2-64.exe+23CF00` (`SMSG_Handler_PingResponse_Trigger`)

Decompiled code for `SMSG_Handler_PingResponse_Trigger`:
```c
void SMSG_Handler_PingResponse_Trigger(undefined8 param_1, longlong param_2)
{
  // param_2 is a pointer to the parsed payload: { subtype: 0x0004, value: 0x00440000 }
  
  // Calls the CMSG builder, passing the value from offset +2 of the payload.
  CMSG::BuildPingResponse(*(uint *)(param_2 + 2));
}
```

## Proposed C-style structs

```cpp
#pragma pack(push, 1)

// Common header to read the subtype
struct smsg_0027_header {
    uint16_t subtype;
};

// Full struct for the 6-byte ping-response variant
struct smsg_0027_ping_response {
    uint16_t subtype;  // == 0x0004
    uint32_t value;    // The value to be echoed back to the server.
};

#pragma pack(pop)
```

## Confidence

*   **Opcodes and Handlers:** High. Confirmed via live dynamic analysis.
*   **Structures:** High. The layouts are simple and directly supported by the size of the logged packets and the logic of the handlers.
*   **Function Names:** High. Based on direct evidence from assert strings and functional analysis.