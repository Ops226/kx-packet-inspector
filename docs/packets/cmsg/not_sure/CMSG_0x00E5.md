**Status: Unverified**

**This packet was documented by analyzing a schema from the client's data files. It has not been confirmed to match live network traffic and may be outdated or used in specific, niche circumstances.**

---

# CMSG 0x00E5 — Unknown

Status: Schema address confirmed. Structure decoded. Name and purpose are unknown.

## Summary

This packet has opcode `0x00E5`. Its purpose is currently unknown.

## Evidence

- **Opcode:** `0x00E5`
- **Schema Address (Live):** `0x7FF6E0EB2470`

## Packet Structure

Based on a full 512-byte dump of the schema. The schema defines a packet with 10 fields.

| Field Order | Type | Name | Notes |
|---|---|---|---|
| 1 | `short` | field_0 | Typecode: 0x01 |
| 2 | `int` | field_1 | Typecode: 0x06. Could also be a float. |
| 3 | `short` | field_2 | Typecode: 0x01 |
| 4 | `int` | field_3 | Typecode: 0x06. Could also be a float. |
| 5 | `short` | field_4 | Typecode: 0x01 |
| 6 | `byte` | field_5 | Typecode: 0x02 |
| 7 | `short` | field_6 | Typecode: 0x01 |
| 8 | `int` | field_7 | Typecode: 0x06. Could also be a float. |
| 9 | `short` | field_8 | Typecode: 0x01 |
| 10 | `int` | field_9 | Typecode: 0x06. Could also be a float. |

## Analysis

The user community hypothesized this opcode could be `SELECT_AGENT`. However, the schema defines a complex, 10-field structure. This seems overly complex for a simple selection action. The packet's true purpose is likely something more sophisticated and remains to be identified.

## Confidence

- Opcode: High
- Schema Address: High
- Structure: High (Complete schema decoded)
- Name: Unknown (Hypothesis of `SELECT_AGENT` is unlikely)