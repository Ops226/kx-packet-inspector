# Packet Reference

This document provides a quick reference to all documented server-to-client (SMSG) packets. Each entry includes the packet's opcode, a brief summary of its function, and a link to its detailed documentation page.

| Opcode | Summary | Details |
|---|---|---|
| `0x0002` | Compact Player Tick Update | [SMSG_PLAYER_STATE_UPDATE_0x0002.md](./SMSG_PLAYER_STATE_UPDATE_0x0002.md) |
| `0x0025` | Structured Update (Multiple Variants) | [SMSG_0x0025.md](./SMSG_0x0025.md) |
| `0x0027` | Small Status/Flag Message | [SMSG_0x0027.md](./SMSG_0x0027.md) |
| `0x002E` | Object Validate/Forward | [SMSG_0x002E.md](./SMSG_0x002E.md) |
| `0x002F` | Forward to Handler A | [SMSG_0x002F.md](./SMSG_0x002F.md) |
| `0x0030` | Forward to Handler B | [SMSG_0x0030.md](./SMSG_0x0030.md) |
| `0x0031` | Object Attach/Notify | [SMSG_0x0031.md](./SMSG_0x0031.md) |
| `0x0032` | Forward with Extra Context | [SMSG_0x0032.md](./SMSG_0x0032.md) |
| `0x0033` | Conditional Notify + Finalize | [SMSG_0x0033.md](./SMSG_0x0033.md) |
| `0x0035` | Context Lookup + Enqueue Actions | [SMSG_0x0035.md](./SMSG_0x0035.md) |
| `0x0036` | Immediate Event Queue Push | [SMSG_0x0036.md](./SMSG_0x0036.md) |
| `0x003A` | Flagged State Update + Optional Payload | [SMSG_0x003A.md](./SMSG_0x003A.md) |
| `0x003F` | String Notify (Channel 1) | [SMSG_0x003F.md](./SMSG_0x003F.md) |
| `0x0040` | String Notify (Channel 2) | [SMSG_0x0040.md](./SMSG_0x0040.md) |
| `0x0041` | Array Install/Refresh | [SMSG_0x0041.md](./SMSG_0x0041.md) |
| `0x0043` | Immediate Notification Enqueue | [SMSG_0x0043.md](./SMSG_0x0043.md) |
| `0x0044` | Composite Handle/Object Insert | [SMSG_0x0044.md](./SMSG_0x0044.md) |
| `0x0045` | Attachment/Asset Install | [SMSG_0x0045.md](./SMSG_0x0045.md) |
| `0x0046` | Array/Table Update | [SMSG_0x0046.md](./SMSG_0x0046.md) |
| `0x0047` | Stateful Update with Cleanup/Gate | [SMSG_0x0047.md](./SMSG_0x0047.md) |
| `0x003F` | Time/Tick Synchronization | [SMSG_TIME_SYNC_0x003F.md](./SMSG_TIME_SYNC_0x003F.md) |
