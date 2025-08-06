# Packet Reference

This document provides a quick reference to all documented server-to-client (SMSG) and client-to-server (CMSG) packets.

## SMSG Packets

| Opcode | Summary | Details |
|---|---|---|
| `0x0002` | Compact Player Tick Update | [smsg/SMSG_PLAYER_STATE_UPDATE_0x0002.md](./smsg/SMSG_PLAYER_STATE_UPDATE_0x0002.md) |
| `0x0027` | Small Status/Flag Message | [smsg/SMSG_0x0027.md](./smsg/SMSG_0x0027.md) |
| `0x003F` | Time/Tick Synchronization | [smsg/SMSG_TIME_SYNC_0x003F.md](./smsg/SMSG_TIME_SYNC_0x003F.md) |

## CMSG Packets

| Opcode | Summary | Details |
|---|---|---|
| `0x0036` | Agent Link | [cmsg/CMSG_AGENT_LINK_0x0036.md](./cmsg/CMSG_AGENT_LINK_0x0036.md) |
| *...more packets here as they are discovered...* |