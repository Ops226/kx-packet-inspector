# CMSG_AGENT_LINK (0x0036)

**Direction**: Client -> Server

## Summary

This packet is sent by the client to link two agents together (e.g., attaching a pet or effect to a character). Its structure is defined by a schema and it is built and queued for sending by the function `FUN_140245390`.

## Construction

*   **Builder Function:** `FUN_140245390`
*   **Schema Address:** `DAT_142513080`
*   **Queueing Function:** `FUN_14104d760` (QueueOutgoingPacket)

## Packet Structure

| Offset | Type | Name | Description |
|---|---|---|---|
| 0x00 | `uint16` | AgentId | The ID of the agent being linked (the "child"). |
| 0x02 | `uint16` | ParentId | The ID of the agent to link to (the "parent"). |
| 0x04 | `uint16` | Flags | Purpose unknown, likely flags or padding. Observed as 0. |

## Evidence

This packet is constructed and sent by `FUN_140245390`, which uses the schema at `DAT_142513080` to build the packet and then calls the queueing function with opcode `0x36`.

```c
// Decompiled builder function FUN_140245390
lVar2 = Msg_BuildPacketFromSchema(&local_30, (uint *)&DAT_142513080, ...);
if (lVar2 != 0) {
    FUN_14104d760(..., 0, 0x36, lVar2); // Opcode 0x36 is passed here
}
```