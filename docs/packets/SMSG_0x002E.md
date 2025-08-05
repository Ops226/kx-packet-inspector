# SMSG 0x002E — Object Validate/Forward

Status: Confirmed (code-backed)

Dispatch Path:
- Dispatcher: FUN_14025DA50, case 0x2E (index = opcode − 0x2B)
- Schema: &DAT_142512810 → passed to FUN_140FD4D50 (typed arg builder)
- Handler code slice (addresses approximate): 0x14025DC29..0x14025DC89

Behavior Summary:
- Parses tuple via FUN_140FD4D50(&DAT_142512810) → RBX.
- Validates/locates an object by calling FUN_1402584F0(param_1, RBX+0x1A, *(ushort**)(RBX+0x2A)).
  • If null → logs with 0x55B and returns.
- Fetches global ctx via FUN_1409A4490; forwards RBX to FUN_14025F6E0(ctx->1F0, RBX).

Key Instructions (evidence):
- tuple = FUN_140FD4D50(&DAT_142512810)
- target = FUN_1402584F0(param_1, (RBX+0x1A), *(ushort**)(RBX+0x2A)); guard against null
- ctx = *(longlong*)(FUN_1409A4490() + 0x1F0)
- FUN_14025F6E0(ctx, tuple)

Field Layout (inferred from access pattern):
- +0x1A: pointer used as an argument to the object lookup
- +0x2A: u16* used alongside to locate the object

Notes:
- Straight parse → validate → forward pattern; minimal branching.
