# SMSG 0x002F — Forward to Handler A

Status: Confirmed (code-backed)

Dispatch Path:
- Dispatcher: FUN_14025DA50, case 0x2F (index = opcode − 0x2B)
- Schema: &DAT_1425129A0 → passed to FUN_140FD4D50 (typed arg builder)
- Handler code slice (addresses approximate): 0x14025DAF1..0x14025DCB4

Behavior Summary:
- Parses tuple via FUN_140FD4D50(&DAT_1425129A0) → RAX.
- Immediately forwards the tuple to FUN_1402605F0(param_1, tuple, param_3[2], param_3+4).
- No additional guards beyond tuple construction.

Key Instructions (evidence):
- tuple = FUN_140FD4D50(&DAT_1425129A0)
- FUN_1402605F0(param_1, tuple)

Notes:
- Straight parse → call flow, ideal “quick win” with clear schema symbol and single target function.
