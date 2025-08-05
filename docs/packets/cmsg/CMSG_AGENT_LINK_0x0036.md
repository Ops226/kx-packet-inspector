# CMSG_AGENT_LINK (0x0036)

Direction: Client -> Server

## Summary

This packet is responsible for handling the "Agent Link" functionality. Its structure is defined by a schema and it is built and queued for sending by the function `FUN_140245390`.

## Construction

*   **Builder Function:** `FUN_140245390`
*   **Schema Address:** `DAT_142513080`
*   **Queueing Function:** `FUN_14104d760` (QueueOutgoingPacket)

## Packet Structure (Inferred from Schema `DAT_142513080`)

*(Analyze the schema at this address in Ghidra and fill out the fields. The call in `FUN_140245390` passes `0x12` as the size, which is 18 bytes. The live log shows a 6-byte packet, so the schema likely defines a larger structure where only the first few fields are used in this context.)*

| Offset | Type | Name | Notes |
|---|---|---|---|
| 0x00 | ... | ... | ... |

## Evidence

Decompiled builder function `FUN_140245390`:
```c
lVar2 = Msg_BuildPacketFromSchema(&local_30, (uint *)&DAT_142513080, 0x12, (longlong)local_48, local_res8);
if (lVar2 != 0) {
    FUN_14104d760(DAT_142628800, 0, 0x36, lVar2);
}
```