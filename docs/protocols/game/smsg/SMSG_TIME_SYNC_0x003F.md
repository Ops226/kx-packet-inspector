# SMSG_TIME_SYNC (0x003F)

**Direction:** Server -> Client
**Status:** Confirmed (Fast Path Packet)

## Summary

`SMSG_TIME_SYNC` is a high-frequency "Fast Path" packet responsible for synchronizing the client's internal timers with the server's master clock. Analysis has confirmed that this packet **does not use the generic handler system**. Instead, its logic is hardcoded directly into the main message dispatcher (`Msg::DispatchStream`) for maximum performance.

Two distinct variants of this 10-byte packet have been observed: a "Cadence/Tick" variant sent periodically during normal gameplay, and a "Seed/Epoch" variant sent in bursts during state transitions like map loading.

## Architectural Role & Dispatch Flow (Fast Path)

This packet serves as a prime example of a Fast Path optimization. Unlike most other packets, it bypasses the standard discovery workflow entirely:

1.  **Bypass of Generic Parsing:** When `Msg::DispatchStream` identifies opcode `0x003F`, it recognizes it as a Fast Path packet (likely via an internal "Dispatch Type" flag). It **skips the call** to the generic schema parser (`MsgUnpack::ParseWithSchema`).
2.  **Hardcoded Handler Logic:** The function then jumps to a dedicated block of code *within* `Msg::DispatchStream` itself. This internal block contains the hardcoded logic to read the 10-byte payload and update the relevant client-side timing variables.
3.  **Bypass of Dynamic Dispatch:** Crucially, the code then **jumps over the `call rax` instruction** that is used to call external handler functions for generic packets.

This is why no handler function could be found for this opcode—one does not exist.

## Payload Variants

### Variant A: Cadence/Tick (Normal Gameplay)

This variant is sent periodically to provide a continuous time-sync signal.

**Live Packet Sample:**
`0F 05 88 B7 7F 09 88 B9 0E 00`

**Inferred Structure:**
| Offset | Type   | Name         | Notes |
| ------ | ------ | ------------ | ----- |
| 0x00   | u16    | Subtype      | Always `0x050F`. |
| 0x02   | u32    | `time_lo`      | Monotonically increasing low part of a 48-bit timestamp. |
| 0x06   | u16    | `time_hi`      | High part of the timestamp. |
| 0x08   | u16    | `flags_or_id`  | A low-variability field, purpose unknown. |

### Variant B: Seed/Epoch (Map Load/Transition)

This variant is sent in a burst to initialize the client's clock to a new "epoch" when joining a new map or instance.

**Live Packet Sample:**
`0D 05 4A 8A E8 03 2A 02 00 00`

**Inferred Structure:**
| Offset | Type   | Name         | Notes |
| ------ | ------ | ------------ | ----- |
| 0x00   | u16    | Subtype      | Always `0x050D`. |
| 0x02   | u16    | `seed`         | A seed value for the new time epoch. |
| 0x04   | u16    | `millis_base`  | Observed as `0x03E8` (1000), likely a millisecond base unit. |
| 0x06   | u16    | `world_or_id`  | An ID for the current world or instance. |
| 0x08   | u16    | `flags`        | Observed as `0x0000`. |

## Evidence

The Fast Path nature of this packet was definitively confirmed via manual tracing. A hardware breakpoint was placed on a unique data signature of the packet's payload (e.g., `0D 05`) within the network receive buffer. Tracing the code that accessed this data led directly to the hardcoded processing logic inside `Msg::DispatchStream` and proved that the execution path **jumps over the generic `call rax` dispatcher**, confirming it has no external handler.

## Confidence

*   **Packet Identity:** High.
*   **Architectural Flow:** High (Confirmed via manual assembly trace).
*   **Payload Structures:** High (Inferred from consistent live data and validated by observing the hardcoded parsing logic).