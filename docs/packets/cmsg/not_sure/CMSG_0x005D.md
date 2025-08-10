# CMSG 0x005D — Unknown (pointer payload from object)

Status: Confirmed opcode. Payload pointer origin identified; size not explicitly set at this site. Name TBD.

## Summary

A Client-to-Server packet with opcode `0x005D` is emitted with a payload pointer taken from an object field. The site copies an 8-byte value from `(obj+0x24)` into `(obj+0x38)` and then sends `(obj+0x38)` as the payload pointer.

## Evidence

Emitter: CMSG_Build_0x005D_FromObj38 @ 0x14023C1B0

Decompiled highlights:
```
*(u64*)(param_1 + 0x38) = *(u64*)(param_1 + 0x24);
MsgConn::QueuePacket(DAT_1426280F0, 0, 0x5D, (u64*)(param_1 + 0x38));
```

Notes:
- Immediate opcode is `0x5D`.
- The payload pointer is `(obj+0x38)`, populated from `(obj+0x24)` just prior to sending.
- No explicit size constant is set at this call site.

## Payload (current understanding)

Conservative view:
```
struct CMSG_0x005D_Payload {
    // Pointer to data at (obj+0x38) sourced from (obj+0x24).
    // Exact size and composition are not defined at this site.
};
```

Open questions:
- Actual size and structure referred to by the pointer.
- Whether other emitters or schemas specify a fixed size.

## Confidence

- Opcode: High (direct `QueuePacket(..., 0x5D, ...)`).
- Pointer origin: High.
- Size/fields: Unknown at this site.
