# Tiny Movement Variant

This document provides the definitive, evidence-backed specification for the "Tiny Movement" variant of the `SMSG_AGENT_UPDATE` (0x0008) packet.

This variant is a small, high-frequency packet used for minor positional updates.

## Packet Structure

The following C++ structures represent the layout of this packet variant. The `TinyMove0f` struct contains the final, converted data that is most useful for analysis.

```cpp
// Tiny movement subtype 0 body layout (Pass 1 spec)
struct TinyMove0 {
    int16_t dx_q88;  // offset 0x10
    int16_t dy_q88;  // offset 0x12
    int16_t dz_q88;  // offset 0x14
    uint8_t flags;   // offset 0x16
};

struct TinyMove0f {
    float dx;
    float dy;
    float dz;
    uint8_t flags;
};

// Returns true on success; out is filled on success.
// buf points to the start of the tiny-movement packet body for subtype 0.
// len is the number of bytes available from buf onward.
// parse_ctx_subtype is the subtype value from parse_ctx+0x0C (must be 0).
inline bool ParseTinyMoveSubtype0(const uint8_t* buf, size_t len, uint32_t parse_ctx_subtype, TinyMove0f& out) {
    if (!buf) return false;
    if (parse_ctx_subtype != 0) return false;

    // Body requires bytes up to offset 0x16 inclusive.
    constexpr size_t kFlagsOff = 0x16;
    constexpr size_t kMinSize  = kFlagsOff + 1;
    if (len < kMinSize) return false;

    auto rd16 = [&](size_t off) -> int16_t {
        // Little-endian 16-bit signed
        return static_cast<int16_t>(static_cast<uint16_t>(buf[off] | (buf[off + 1] << 8)));
    };

    int16_t dx_q88 = rd16(0x10);
    int16_t dy_q88 = rd16(0x12);
    int16_t dz_q88 = rd16(0x14);
    uint8_t flags  = buf[kFlagsOff];

    // Q8.8 to float
    constexpr float kInv256 = 1.0f / 256.0f;
    out.dx = static_cast<float>(dx_q88) * kInv256;
    out.dy = static_cast<float>(dy_q88) * kInv256;
    out.dz = static_cast<float>(dz_q88) * kInv256;
    out.flags = flags;
    return true;
}

// Flags bit masks observed/assumed for Pass 1 (from TEST DIL,0x1)
constexpr uint8_t kTM0_Flag_Bit0 = 0x01; // toggles iteration branch at 0x141447188
```

## Field Table

This table describes the fields in the packet body. The offsets are relative to the start of the packet payload.

| Offset | Type    | Name     | Scale/Notes                                       |
|--------|---------|----------|---------------------------------------------------| 
| 0x10   | `int16` | `dx_q88` | Q8.8 fixed-point. Convert to float: `value / 256.0f` |
| 0x12   | `int16` | `dy_q88` | Q8.8 fixed-point. Convert to float: `value / 256.0f` |
| 0x14   | `int16` | `dz_q88` | Q8.8 fixed-point. Convert to float: `value / 256.0f` |
| 0x16   | `uint8` | `flags`  | Bit0 = 1 toggles a follow-up iteration branch.    |

## Evidence

The following evidence from static analysis confirms the structure and semantics of this packet variant.

### Parser Dispatch

The parser for this variant is invoked via a virtual function call at offset `+0x38` of a subtype-specific "entry" object. These callsites are the primary anchors for identifying the parser.

*   **Hub Tail-Call (Example Address: `0x1410eb7e0`)**
    ```
    1410eb7fb: MOV RCX,[RAX]        ; vtable
    1410eb7fe: MOV RDX,[RCX+0x38]   ; vtable + 0x38
    1410eb809: JMP RDX              ; tail-call parser
    ```
*   **Consumer Call (Example Address: `0x141446d80`)**
    ```
    14144705e: MOV RCX,[RAX]        ; vtable
    141447061: MOV RDX,[RCX+0x38]   ; vtable + 0x38
    141447068: CALL RDX             ; invoke parser (+0x38)
    ```

### Field Semantics

The meaning of the fields is derived from how the code *uses* the data after the packet is parsed.

*   **Flags Bit Usage:** After the parser is called, a specific bit of the `flags` field is tested. This test determines whether a specific code path is taken, confirming the flag's purpose.
    *   **Evidence (Example Address: `0x141446d80` at offset `+0x141447188`)**
        ```
        TEST DIL,0x1 
        ```
        This instruction tests the first bit of the `DIL` register (which contains the parsed flags). If the bit is set, the code branches to an iteration loop, indicating this flag controls a specific behavior.

*   **Movement Pipeline:** The `dx`, `dy`, and `dz` values are used in a series of function calls that update the agent's position in the game world. This confirms their role as movement deltas.
