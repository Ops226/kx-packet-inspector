# SMSG 0x003A — Flagged State Update + Optional Payload

Status: Confirmed (code-backed)

Dispatch Path:
- Dispatcher: FUN_14025DA50, case 0x3A (index = opcode − 0x2B)
- Schema: &DAT_142513FA0 → passed to FUN_140FD4D50 (typed arg builder)
- Handler code slice (addresses approximate): 0x14025E3A0..0x14025E523

Behavior Summary:
- Parses tuple via FUN_140FD4D50(&DAT_142513FA0) → RBX.
- Resolves/validates a target object (param_1 context; typical (pointer + u16*) key).
- Applies decoded option bits (via FUN_140269790 or simple bit tests) to update flags in a state block (e.g., obj+0x198, representative), possibly toggling/setting fields.
- If certain flags or a count/length field is non-zero, processes an optional payload region:
  • Copies a len-encoded blob/string from the tuple (len-u8 or len-u16 depending on schema typecode 0x14/0x15).
  • May trigger a small cleanup path (e.g., FUN_140276040) before applying the payload.
- Bounded flow; guarded by simple bit checks and non-null tests.

Key Instructions (evidence):
- tuple = FUN_140FD4D50(&DAT_142513FA0)
- obj = FUN_140257E70 / FUN_1402584F0 style lookup; guard against null
- flags = decode from tuple (FUN_140269790 or bit masks) → write/toggle into target fields
- if (has_payload) { read len + data → install/apply; optional cleanup }

Field Layout (inferred from handler offsets):
- Early fields: keys for lookup (pointer + u16*)
- Flag field(s): one or more u8/u32 packed bits; used to gate updates
- Optional payload: a len-encoded region via 0x14 (len-u8) or 0x15 (len-u16), present only when gated by flags

Wire Structure (first-pass; refine from &DAT_142513FA0 typecodes)

```cpp
#pragma pack(push, 1)

struct Smsg_003A_Tuple {
    // Lookup keys
    uint8_t*   key_ptr;      // pointer used in lookup
    uint16_t*  key_u16p;     // auxiliary id
    // Flags/options
    uint32_t   flags;        // representative; exact width via schema
    // Optional payload (present if gated by flags/length)
    uint8_t    opt_len;      // or uint16_t if schema uses 0x15
    uint8_t    opt_data[];   // variable-length payload
};

#pragma pack(pop)
```

Notes:
- Good example to internalize: flag decoding, optional len-encoded payloads, and safe application under guards.
