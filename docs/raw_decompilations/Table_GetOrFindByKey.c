
void Table_GetOrFindByKey(longlong param_1,ulonglong *param_2)

{
  code *pcVar1;
  ulonglong uVar2;
  ulonglong uVar3;
  ulonglong uVar4;
  longlong lVar5;
  int *piVar6;
  uint uVar7;
  ulonglong uVar8;
  ulonglong *puVar9;
  undefined1 auStackY_58 [32];
  uint local_28 [2];
  undefined8 local_20;
  undefined8 uStack_18;
  ulonglong local_10;
  
  local_10 = DAT_142551900 ^ (ulonglong)auStackY_58;
  uVar2 = *param_2;
  uVar3 = param_2[1];
  uVar8 = 0x325d1eae;
  puVar9 = &local_20;
  local_20 = uVar2;
  uVar2 = local_20;
  uStack_18 = uVar3;
  uVar3 = uStack_18;
  uVar7 = 0x325d1eae;
  do {
    uVar4 = *puVar9;
    puVar9 = (ulonglong *)((longlong)puVar9 + 1);
    uVar7 = ((&DAT_141b61870)[uVar8 >> 0x18] ^ (&DAT_141b61870)[(byte)uVar4] ^ uVar7 >> 6) + uVar7;
    uVar8 = (ulonglong)uVar7;
  } while (puVar9 != &local_10);
  local_28[0] = uVar7 & *(uint *)(param_1 + 0x40);
  if (*(uint *)(param_1 + 0x3c) <= local_28[0]) {
    FUN_140233f00("D:\\Perforce\\Live\\NAEU\\v2\\Code\\Arena\\Core\\Collections\\Array.h",0x2b2,
                  0x1418fc310,local_28,(uint *)(param_1 + 0x3c));
    pcVar1 = (code *)swi(3);
    (*pcVar1)();
    return;
  }
  piVar6 = (int *)((ulonglong)local_28[0] * 0x20 + *(longlong *)(param_1 + 0x30));
  if (((*(ulonglong *)(piVar6 + 4) & 1) == 0) && (lVar5 = *(longlong *)(piVar6 + 4), lVar5 != 0)) {
    uStack_18._4_4_ = (int)(uVar3 >> 0x20);
    uStack_18._0_4_ = (int)uVar3;
    local_20._4_4_ = (int)(uVar2 >> 0x20);
    local_20._0_4_ = (int)uVar2;
    while ((((*(uint *)(*(int *)(param_1 + 0x20) + lVar5) != uVar7 ||
             (*(int *)(lVar5 + 0x18) != (int)local_20)) ||
            (*(int *)(lVar5 + 0x1c) != local_20._4_4_)) ||
           ((*(int *)(lVar5 + 0x20) != (int)uStack_18 || (*(int *)(lVar5 + 0x24) != uStack_18._4_4_)
            )))) {
      if (((*(ulonglong *)(*piVar6 + lVar5 + 8) & 1) != 0) ||
         (lVar5 = *(longlong *)(*piVar6 + lVar5 + 8), lVar5 == 0)) break;
    }
  }
  local_20 = uVar2;
  uStack_18 = uVar3;
                    /* WARNING: Subroutine does not return */
  FUN_140e37790(local_10 ^ (ulonglong)auStackY_58);
}

