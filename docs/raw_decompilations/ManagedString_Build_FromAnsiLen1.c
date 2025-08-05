
/* WARNING: Removing unreachable block (ram,0x000140246dce) */
/* WARNING: Removing unreachable block (ram,0x000140246d03) */
/* WARNING: Removing unreachable block (ram,0x000140246cf8) */
/* WARNING: Removing unreachable block (ram,0x000140246d06) */
/* WARNING: Removing unreachable block (ram,0x000140246d0b) */
/* WARNING: Removing unreachable block (ram,0x000140246d33) */
/* WARNING: Removing unreachable block (ram,0x000140246d54) */
/* WARNING: Removing unreachable block (ram,0x000140246d14) */
/* WARNING: Removing unreachable block (ram,0x000140246d17) */
/* WARNING: Removing unreachable block (ram,0x000140246d1b) */
/* WARNING: Removing unreachable block (ram,0x000140246d22) */
/* WARNING: Removing unreachable block (ram,0x000140246d83) */
/* WARNING: Removing unreachable block (ram,0x000140246d91) */
/* WARNING: Removing unreachable block (ram,0x000140246dd3) */

void ManagedString_Build_FromAnsiLen1(longlong *param_1,char *param_2,char *param_3,uint param_4)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  char *pcVar5;
  char *pcVar6;
  undefined1 *puVar7;
  undefined7 extraout_var;
  undefined7 extraout_var_00;
  byte bVar8;
  uint uVar9;
  char *pcVar10;
  char *pcVar11;
  uint uVar12;
  uint uVar13;
  
  pcVar11 = (char *)0x0;
  if (param_2 == (char *)0x0) {
    puVar7 = (undefined1 *)*param_1;
LAB_140246c25:
    if (puVar7 != (undefined1 *)0x0) {
      *puVar7 = 0;
    }
    *(undefined4 *)(param_1 + 1) = 0;
    return;
  }
  pcVar5 = param_2;
  pcVar10 = pcVar11;
  if (param_4 != 0) {
    do {
      if (*pcVar5 == '\0') {
        puVar7 = (undefined1 *)*param_1;
        goto LAB_140246c25;
      }
      uVar9 = (int)pcVar10 + 1;
      pcVar5 = pcVar5 + 1;
      pcVar10 = (char *)(ulonglong)uVar9;
    } while (uVar9 < param_4);
  }
  pcVar6 = param_2 + param_4;
  pcVar5 = pcVar11;
  pcVar10 = param_3;
  if ((uint)param_3 != 0) {
    do {
      pcVar10 = pcVar5;
      if (*pcVar6 == '\0') break;
      uVar9 = (int)pcVar5 + 1;
      pcVar5 = (char *)(ulonglong)uVar9;
      pcVar6 = pcVar6 + 1;
      pcVar10 = param_3;
    } while (uVar9 < (uint)param_3);
  }
  pcVar5 = (char *)*param_1;
  uVar9 = (uint)pcVar10;
  if (pcVar5 <= param_2 + ((ulonglong)pcVar10 & 0xffffffff) + (ulonglong)param_4) {
    pcVar6 = pcVar11;
    if (*(uint *)(param_1 + 1) != 0) {
      pcVar6 = pcVar5 + *(uint *)(param_1 + 1);
    }
    if (param_2 + param_4 <= pcVar6) {
      uVar2 = FUN_1409a8e80();
      if (uVar9 + 1 != 0) {
                    /* WARNING: Subroutine does not return */
        FUN_1409bfc20(1,(ulonglong)(uVar9 + 1),0,0,
                      "D:\\Perforce\\Live\\NAEU\\v2\\Code\\Arena\\Core\\Collections\\Allocator.h",
                      0x26c,uVar2 & 0xffff);
      }
      if (((uVar9 != 0) && (uVar9 != 0)) && (uVar9 != 0)) {
                    /* WARNING: Subroutine does not return */
        FUN_1409bfc20(1,(ulonglong)pcVar10 & 0xffffffff,0,0,
                      "D:\\Perforce\\Live\\NAEU\\v2\\Code\\Arena\\Core\\Collections\\Allocator.h",
                      0x26c,uVar2 & 0xffff);
      }
                    /* WARNING: Subroutine does not return */
      thunk_FUN_1418c47c0((undefined8 *)0x0,(undefined8 *)(param_2 + param_4),
                          (ulonglong)pcVar10 & 0xffffffff);
    }
  }
  if (pcVar5 != (char *)0x0) {
    *pcVar5 = '\0';
  }
  uVar2 = *(uint *)((longlong)param_1 + 0x14);
  uVar13 = uVar9 + 1;
  *(undefined4 *)(param_1 + 1) = 0;
  if (uVar2 < uVar13) {
    uVar12 = uVar13;
    if (uVar2 != 0) {
      uVar2 = uVar2 * 2;
      uVar3 = 0x100;
      if (0x100 < uVar2) {
        uVar3 = uVar2;
      }
      if (uVar13 < uVar3) {
        if (uVar13 < 0x100) {
          uVar12 = uVar2;
          if (uVar2 < uVar13) {
            uVar12 = uVar13;
          }
          if ((uVar12 & uVar12 - 1) != 0) {
            iVar4 = FUN_1409b1700(uVar12);
            uVar12 = 1 << ((char)iVar4 + 1U & 0x1f);
          }
        }
        else {
          iVar4 = FUN_1409b1700(uVar13);
          bVar8 = (char)iVar4 - 1;
          uVar12 = (3 << (bVar8 & 0x1f) & uVar13) + (1 << (bVar8 & 0x1f));
          if (uVar12 < uVar13) {
                    /* WARNING: Subroutine does not return */
            FUN_1409cd550();
          }
        }
      }
    }
    if (uVar12 != *(uint *)((longlong)param_1 + 0x14)) {
      pcVar5 = (char *)*param_1;
      pcVar10 = pcVar11;
      if ((uVar12 != 0) &&
         ((pcVar5 == (char *)0x0 ||
          (bVar1 = FUN_1409bfea0((longlong)pcVar5,(ulonglong)uVar12), pcVar10 = pcVar5,
          (int)CONCAT71(extraout_var,bVar1) == 0)))) {
                    /* WARNING: Subroutine does not return */
        FUN_1409bfc20((char)(short)param_1[2],(ulonglong)uVar12,0,0,
                      "D:\\Perforce\\Live\\NAEU\\v2\\Code\\Arena\\Core\\Collections\\Allocator.h",
                      0x26c,(uint)*(ushort *)((longlong)param_1 + 0x12));
      }
      pcVar5 = (char *)*param_1;
      if ((pcVar5 != (char *)0x0) && (pcVar10 != pcVar5)) {
                    /* WARNING: Subroutine does not return */
        thunk_FUN_1418c47c0((undefined8 *)pcVar10,(undefined8 *)pcVar5,
                            (ulonglong)*(uint *)(param_1 + 1));
      }
      *param_1 = (longlong)pcVar10;
      *(uint *)((longlong)param_1 + 0x14) = uVar12;
    }
  }
  FUN_140246b50(param_1,(undefined8 *)(param_2 + param_4),uVar9);
  uVar9 = *(uint *)(param_1 + 1);
  uVar2 = *(uint *)((longlong)param_1 + 0x14);
  uVar13 = uVar9 + 1;
  if (uVar2 < uVar13) {
    uVar12 = uVar13;
    if (uVar2 != 0) {
      uVar2 = uVar2 * 2;
      uVar3 = 0x100;
      if (0x100 < uVar2) {
        uVar3 = uVar2;
      }
      if (uVar13 < uVar3) {
        if (uVar13 < 0x100) {
          uVar12 = uVar2;
          if (uVar2 < uVar13) {
            uVar12 = uVar13;
          }
          if ((uVar12 & uVar12 - 1) != 0) {
            iVar4 = FUN_1409b1700(uVar12);
            uVar12 = 1 << ((char)iVar4 + 1U & 0x1f);
          }
        }
        else {
          iVar4 = FUN_1409b1700(uVar13);
          bVar8 = (char)iVar4 - 1;
          uVar12 = (3 << (bVar8 & 0x1f) & uVar13) + (1 << (bVar8 & 0x1f));
          if (uVar12 < uVar13) {
                    /* WARNING: Subroutine does not return */
            FUN_1409cd550();
          }
        }
      }
    }
    if (uVar12 != *(uint *)((longlong)param_1 + 0x14)) {
      pcVar5 = (char *)*param_1;
      if ((uVar12 != 0) &&
         ((pcVar5 == (char *)0x0 ||
          (bVar1 = FUN_1409bfea0((longlong)pcVar5,(ulonglong)uVar12), pcVar11 = pcVar5,
          (int)CONCAT71(extraout_var_00,bVar1) == 0)))) {
                    /* WARNING: Subroutine does not return */
        FUN_1409bfc20((char)(short)param_1[2],(ulonglong)uVar12,0,0,
                      "D:\\Perforce\\Live\\NAEU\\v2\\Code\\Arena\\Core\\Collections\\Allocator.h",
                      0x26c,(uint)*(ushort *)((longlong)param_1 + 0x12));
      }
      pcVar5 = (char *)*param_1;
      if ((pcVar5 != (char *)0x0) && (pcVar11 != pcVar5)) {
                    /* WARNING: Subroutine does not return */
        thunk_FUN_1418c47c0((undefined8 *)pcVar11,(undefined8 *)pcVar5,
                            (ulonglong)*(uint *)(param_1 + 1));
      }
      *param_1 = (longlong)pcVar11;
      *(uint *)((longlong)param_1 + 0x14) = uVar12;
    }
  }
  *(uint *)(param_1 + 1) = uVar13;
  *(undefined1 *)((ulonglong)uVar9 + *param_1) = 0;
  return;
}

