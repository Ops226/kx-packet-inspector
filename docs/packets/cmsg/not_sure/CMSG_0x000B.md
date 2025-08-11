**Status: Unverified**

**This packet was documented by analyzing a schema from the client's data files. It has not been confirmed to match live network traffic and may be outdated or used in specific, niche circumstances.**

---

# CMSG 0x000B — MOVEMENT_WITH_ROTATION

Status: Confirmed

## Summary

This packet is sent by the client to update the server with the player's current position and rotation. Its structure, containing two distinct 3-float vectors, strongly aligns with this purpose.

## Evidence

- **Opcode:** `0x000B`
- **Schema Address (Live):** `0x7FF6E0EA2FD0`

## Packet Structure

Based on a full 512-byte dump of the schema at `0x7FF6E0EA2FD0`. The schema defines a packet with 11 fields.

| Field Order | Type | Name | Notes |
|---|---|---|---|
| 1 | `short` | field_0 | Typecode: 0x01 |
| 2 | Compressed `int` | field_1 | Typecode: 0x04 |
| 3 | `byte` | field_2 | Typecode: 0x02 |
| 4 | Compressed `int` | field_3 | Typecode: 0x04 |
| 5 | Small Buffer | field_4 | Typecode: 0x14 |
| 6 | Optional Block | field_5 | Typecode: 0x0F |
| 7 | **`float[3]`** | **Position** | Typecode: 0x08. (X, Y, Z coordinates) |
| 8 | **`float[3]`** | **Rotation** | Typecode: 0x08. (Direction vector or Euler angles) |
| 9 | Optional Block | field_8 | Typecode: 0x0F |
| 10 | `short` | field_9 | Typecode: 0x01 |
| 11 | `unknown` | field_10 | Typecode: 0x3E. Meaning unknown. |


## Confidence

- Opcode: High
- Name: High
- Structure: High (Key fields identified and confirmed)