
void Agent_TinyMovement_OnUpdate(longlong param_1,longlong param_2)

{
  longlong lVar1;
  longlong lVar2;
  
  *(uint *)(param_1 + 0x470) = *(uint *)(param_1 + 0x470) | 0x10;
  if (param_2 != 0) {
    lVar1 = Parse_TinyMovementPayload(param_2);
    lVar2 = lVar1 + -8;
    if (lVar1 == 0) {
      lVar2 = 0;
    }
    if (lVar2 != 0) {
      FUN_141131d10((longlong *)(lVar2 + 0x9c8));
      return;
    }
  }
  return;
}

