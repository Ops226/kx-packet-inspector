
void AgentMovement_ApplyTinyUpdate(longlong *param_1)

{
  int iVar1;
  longlong lVar2;
  longlong *plVar3;
  longlong lVar4;
  
  lVar2 = (**(code **)(*(longlong *)param_1[0x14] + 200))();
  if (lVar2 == 0) {
    return;
  }
  if (param_1[0xd] == 0) {
    return;
  }
  plVar3 = (longlong *)(**(code **)(*(longlong *)(param_1[0x14] + 8) + 0x168))();
  if ((plVar3 != (longlong *)0x0) && (lVar2 = (**(code **)(*plVar3 + 0x278))(plVar3), lVar2 != 0)) {
    lVar2 = (*(code *)**(undefined8 **)(lVar2 + 8))(lVar2 + 8);
    plVar3 = (longlong *)FUN_1410f5f90(lVar2);
    if (plVar3 != (longlong *)0x0) {
      lVar2 = (**(code **)(*plVar3 + 0x80))(plVar3);
      lVar4 = Parse_TinyMovementPayload(lVar2);
      lVar2 = lVar4 + -8;
      if (lVar4 == 0) {
        lVar2 = 0;
      }
      if (((lVar2 != 0) &&
          (plVar3 = (longlong *)(**(code **)(**(longlong **)(lVar2 + 0xa68) + 200))(),
          plVar3 != (longlong *)0x0)) &&
         (iVar1 = (**(code **)(*plVar3 + 0x8d0))(plVar3), iVar1 != 0)) goto FUN_141131e41;
    }
  }
  plVar3 = FUN_1410dff60();
  lVar2 = (**(code **)(*(longlong *)(param_1[0x14] + 8) + 0x140))();
  lVar4 = (**(code **)(*plVar3 + 0x40))(plVar3);
  if ((lVar4 != lVar2) && (iVar1 = (**(code **)(*param_1 + 0x10))(param_1), iVar1 == 0)) {
    if (*(float *)((longlong)param_1 + 0x3c) != 0.0) {
      return;
    }
    FUN_141134780((longlong)param_1,0);
    return;
  }
FUN_141131e41:
  plVar3 = (longlong *)(**(code **)(*(longlong *)param_1[0x14] + 200))();
  if ((plVar3 != (longlong *)0x0) && (iVar1 = (**(code **)(*plVar3 + 0x8d0))(plVar3), iVar1 != 1)) {
    (**(code **)(*plVar3 + 0x8f8))(plVar3,1);
  }
  *(undefined4 *)((longlong)param_1 + 0x3c) = 0x40000000;
  return;
}

