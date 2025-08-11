# CMSG Packet Reference

This document provides a quick reference to all documented client-to-server (CMSG) packets. Packets marked as "(Schema-Defined, Unverified)" have a known schema in the client's data, but their structure has not been confirmed to match live network traffic for common in-game actions.

| Opcode | Summary | Details |
|---|---|---|
| `0x0001` | Unknown (Schema-Defined, Unverified) | [CMSG_0x0001.md](./not_sure/CMSG_0x0001.md) |
| `0x0002` | Unknown (Schema-Defined, Unverified) | [CMSG_0x0002.md](./not_sure/CMSG_0x0002.md) |
| `0x0004` | Unknown (Schema-Defined, Unverified) | [CMSG_0x0004.md](./not_sure/CMSG_0x0004.md) |
| `0x0006` | Ping Response / Unknown | [CMSG_PING_RESPONSE_0x0006.md](./not_sure/CMSG_PING_RESPONSE_0x0006.md) |
| `0x0009` | Jump (Live-Validated) | [CMSG_JUMP_0x0009.md](./CMSG_JUMP_0x0009.md) |
| `0x000B` | Movement w/ Rotation (Schema-Defined, Unverified) | [CMSG_0x000B.md](./not_sure/CMSG_0x000B.md) |
| `0x0011` | Heartbeat / Unknown (Schema-Defined, Unverified) | [CMSG_0x0011.md](./not_sure/CMSG_0x0011.md) |
| `0x0012` | Movement / Unknown | [CMSG_0x0012.md](./CMSG_0x0012.md) |
| `0x0018` | Mount Movement | [CMSG_0x0018.md](./CMSG_0x0018.md) |
| `0x0023` | Logout to Char-Select (Live-Validated) | [CMSG_LOGOUT_TO_CHAR_SELECT_0x0023.md](./not_sure/CMSG_LOGOUT_TO_CHAR_SELECT_0x0023.md) |
| `0x0024` | Unknown (Manual Builder Confirmed) | [CMSG_0x0024.md](./not_sure/CMSG_0x0024.md) |
| `0x0025` | Unknown (Manual Builder Confirmed) | [CMSG_0x0025.md](./not_sure/CMSG_0x0025.md) |
| `0x0028` | Unknown (Manual Builder Confirmed) | [CMSG_0x0028.md](./not_sure/CMSG_0x0028.md) |
| `0x002B` | Unknown (Manual Builder Confirmed) | [CMSG_0x002B.md](./not_sure/CMSG_0x002B.md) |
| `0x0036` | Agent Link | [CMSG_AGENT_LINK_0x0036.md](./not_sure/CMSG_AGENT_LINK_0x0036.md) |
| `0x0050` | Simple U32 (Manual Builder Confirmed) | [CMSG_0x0050.md](./not_sure/CMSG_0x0050.md) |
| `0x0051` | Unknown (Manual Builder Confirmed) | [CMSG_0x0051.md](./not_sure/CMSG_0x0051.md) |
| `0x005B` | Unknown (Manual Builder Confirmed) | [CMSG_0x005B.md](./not_sure/CMSG_0x005B.md) |
| `0x005D` | Unknown (Manual Builder Confirmed) | [CMSG_0x005D.md](./not_sure/CMSG_0x005D.md) |
| `0x005F` | Unknown (Manual Builder Confirmed) | [CMSG_0x005F.md](./not_sure/CMSG_0x005F.md) |
| `0x0065` | Unknown (Manual Builder Confirmed) | [CMSG_0x0065.md](./not_sure/CMSG_0x0065.md) |
| `0x0068` | Unknown (Manual Builder Confirmed) | [CMSG_0x0068.md](./not_sure/CMSG_0x0068.md) |
| `0x0069` | Unknown (Manual Builder Confirmed) | [CMSG_0x0069.md](./not_sure/CMSG_0x0069.md) |
| `0x006A` | Unknown (Manual Builder Confirmed) | [CMSG_0x006A.md](./not_sure/CMSG_0x006A.md) |
| `0x006B` | Unknown (Manual Builder Confirmed) | [CMSG_0x006B.md](./not_sure/CMSG_0x006B.md) |
| `0x00E5` | Unknown (Schema-Defined, Unverified) | [CMSG_0x00E5.md](./not_sure/CMSG_0x00E5.md) |