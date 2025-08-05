# SMSG_TIME_SYNC (0x003F) — Time/Tick Synchronization (Two Variants Observed)

Direction: Server → Client  
Observed size: 10 bytes  
Endianness: Little endian

## Summary

0x003F appears at a steady cadence during normal gameplay and also in a distinct repeating form around map transitions or loading. Two stable 10-byte variants are observed:

- Cadence/Tick variant (“normal”): bytes change monotonically over time, likely representing server tick/timebase progression.
- Seed/Epoch variant (“transition”): fixed constants repeated across several occurrences, consistent with a “seed/epoch install” during map/phase transitions.

Handler mapping (via SrvMsgDispatcher selector at vfunc +0x18) is pending; on-wire shapes are stable for consumers.

## Observed Variants and Layouts

We document conservative field cuts that fit both forms and keep consumers safe. All fields are little-endian.

### Variant A — Cadence/Tick (10 bytes)

Representative samples:
- `0F 05 38 39 88 10 38 6B 39 00`
- `0F 05 B0 25 88 10 B0 57 39 00`
- `0F 05 28 12 88 10 28 44 39 00`
- `0F 05 A0 FE 87 10 A0 30 39 00`
- `0F 05 18 EB 87 10 18 1D 39 00`
- `0F 05 90 D7 87 10 90 09 39 00`
- `0F 05 70 89 87 10 70 BB 38 00`
- `0F 05 E8 75 87 10 E8 A7 38 00`

Proposed minimal layout:

Offset | Type   | Name         | Notes
------ | ------ | ------------ | -----
0x00   | u16    | type         | Observed 0x050F in cadence forms
0x02   | u32    | time_lo      | Monotonic/cadenced (low part or local timebase unit)
0x06   | u16    | time_hi      | High part/segment (often 0x1088 / 0x1087 etc.)
0x08   | u16    | flags_or_id  | Low-variability field (often around 0x0039/0x0038/0x0000)

Notes:
- The 6-byte middle region reads naturally as u32 + u16 for a progressing timing source (time_lo + time_hi), followed by a small u16 tail used as a flag or segment ID.
- The pair (time_lo, time_hi) appears to increment over time; the tail (flags_or_id) shifts among small values (0x39, 0x38, 0x00).

### Variant B — Seed/Epoch (10 bytes)

Representative samples (identical payload repeats):
- `0D 05 4A 8A E8 03 2A 02 00 00` (multiple occurrences)
  
Proposed minimal layout:

Offset | Type   | Name         | Notes
------ | ------ | ------------ | -----
0x00   | u16    | type         | Observed 0x050D in seed/epoch form
0x02   | u16    | seed         | Observed 0x8A4A
0x04   | u16    | millis       | Observed 0x03E8 (1000)
0x06   | u16    | world_or_id  | Observed 0x022A
0x08   | u16    | flags        | Observed 0x0000

Notes:
- All samples repeat the same constants, consistent with a “time seed/epoch install” or “phase bind” value broadcast during transitions.
- The presence of a clean 1000 (0x03E8) strongly suggests “milliseconds per unit” or a fixed calibration parameter.

## Behavior / Semantics (Hypotheses)

- Variant A (Cadence/Tick): Periodic time synchronization message driving client-side timers or interpolation windows. The (time_lo, time_hi) pair increments over the session.
- Variant B (Seed/Epoch): A special-case broadcast during map/phase changes to install/reset a time epoch, milliseconds per unit, and a world/instance identifier.

## Evidence (selected log lines)

- Cadence:
  - `14:55:17.491  0F 05 38 39 88 10 38 6B 39 00`
  - `14:55:22.498  0F 05 B0 25 88 10 B0 57 39 00`
  - `14:55:27.486  0F 05 28 12 88 10 28 44 39 00`
  - `14:55:32.509  0F 05 A0 FE 87 10 A0 30 39 00`
  - `14:55:37.509  0F 05 18 EB 87 10 18 1D 39 00`
  - `14:55:42.511  0F 05 90 D7 87 10 90 09 39 00`
  - `14:56:02.535  0F 05 70 89 87 10 70 BB 38 00`
  - `14:56:07.535  0F 05 E8 75 87 10 E8 A7 38 00`

- Seed/Epoch:
  - `14:55:38.600  0D 05 4A 8A E8 03 2A 02 00 00`
  - `14:55:44.180  0D 05 4A 8A E8 03 2A 02 00 00`
  - `14:55:53.148  0D 05 4A 8A E8 03 2A 02 00 00`
  - `14:56:00.914  0D 05 4A 8A E8 03 2A 02 00 00`
  - `14:56:02.469  0D 05 4A 8A E8 03 2A 02 00 00`
  - `14:55:54.737  0D 05 4A 8A E8 03 2A 02 00 00`

## Proposed C-style structs

```cpp
#pragma pack(push, 1)

// Variant A: Cadence/Tick (10 bytes)
struct smsg_time_sync_tick {
    uint16_t type;        // e.g., 0x050F
    uint32_t time_lo;     // monotonic/cadenced low part
    uint16_t time_hi;     // high part/segment
    uint16_t flags_or_id; // small ID or flags (e.g., 0x0039/0x0038/0x0000)
};

// Variant B: Seed/Epoch (10 bytes)
struct smsg_time_sync_seed {
    uint16_t type;        // 0x050D
    uint16_t seed;        // 0x8A4A
    uint16_t millis;      // 0x03E8 (1000)
    uint16_t world_or_id; // 0x022A
    uint16_t flags;       // 0x0000
};

#pragma pack(pop)
```

## Mapping Status

- The exact handler for `SMSG_TIME_SYNC` must be discovered via dynamic analysis (e.g., setting a breakpoint at `"Gw2-64.exe"+FD1ACD` within `Gs2c_SrvMsgDispatcher` and observing the `RAX` register when this opcode is processed). This page focuses on stable on-wire decoding to unblock consumers now, as the parsing logic is handled by `Msg_ParseAndDispatch_BuildArgs` based on a schema.

## Consumer Guidance

- Distinguish variants by the first word (`type`), then decode fields as documented.
- Do not rely on `flags_or_id` for game logic without further correlation; use it as a metadata field.
- Cadence variant can be used as a monotonic timebase source for interpolation if needed; confirm against local measurements.

## Confidence

- Layout confidence: High (stable 10-byte forms; repeated constants in seed variant; monotonic behavior in cadence variant).
- Semantic naming: Medium (inferred; exact naming to be refined after handler mapping).
- Mapping: Pending (normal selector vs. potential fastpath).
