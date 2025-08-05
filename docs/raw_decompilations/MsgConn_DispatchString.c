
void MsgConn_DispatchString
               (undefined8 param_1,undefined8 *param_2,int param_3,undefined4 param_4,
               undefined8 param_5)

{
  uint uVar1;
  int iVar2;
  longlong lVar3;
  longlong *plVar4;
  longlong *plVar5;
  uint *puVar6;
  longlong *plVar7;
  uint local_38 [4];
  
  lVar3 = FUN_1409a4490();
  lVar3 = *(longlong *)(lVar3 + 0x1f0);
  if (*(int *)(lVar3 + 0x6cc) != 0) {
    uVar1 = FUN_140247e60((uint *)(lVar3 + 0x6c8),param_2,local_38);
    if ((*(int *)((ulonglong)uVar1 * 0x38 + 0x30 + *(longlong *)(lVar3 + 0x6d0)) != 0) &&
       (puVar6 = (uint *)(*(longlong *)(lVar3 + 0x6d0) + 0x18 + (ulonglong)uVar1 * 0x38),
       puVar6 != (uint *)0x0)) {
      plVar5 = *(longlong **)(puVar6 + 2);
      if (plVar5 == (longlong *)0x0) {
        plVar7 = (longlong *)0x0;
      }
      else {
        plVar7 = plVar5 + (ulonglong)*puVar6 * 2;
        if (plVar5 != plVar7) {
          iVar2 = (int)plVar5[1];
          while ((iVar2 == 0 && (plVar5 = plVar5 + 2, plVar5 < plVar7))) {
            iVar2 = (int)plVar5[1];
          }
        }
      }
LAB_14025fb82:
      if (*(longlong *)(puVar6 + 2) == 0) {
        plVar4 = (longlong *)0x0;
      }
      else {
        plVar4 = (longlong *)((ulonglong)*puVar6 * 0x10 + *(longlong *)(puVar6 + 2));
      }
      if (plVar5 != plVar4) {
        plVar4 = (longlong *)*plVar5;
        iVar2 = (**(code **)(*plVar4 + 0x18))(plVar4);
        if (iVar2 == param_3) {
          FUN_140fe99f0((longlong)plVar4,param_4,param_5);
        }
        do {
          plVar5 = plVar5 + 2;
          if (plVar7 <= plVar5) break;
        } while ((int)plVar5[1] == 0);
        goto LAB_14025fb82;
      }
    }
  }
  return;
}

