
void Table_InstallOrRefresh(longlong param_1,longlong *param_2,undefined8 *param_3,uint param_4)

{
  uint uVar1;
  code *pcVar2;
  undefined8 *puVar3;
  short *psVar4;
  bool bVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  int *piVar9;
  int *piVar10;
  undefined8 uVar11;
  int *piVar12;
  undefined7 extraout_var;
  undefined7 extraout_var_00;
  longlong *plVar13;
  uint uVar14;
  longlong *plVar15;
  longlong *plVar16;
  uint *puVar17;
  longlong *local_res10;
  undefined8 *local_res18;
  uint local_res20;
  short *local_88;
  int local_80;
  uint *local_78;
  int local_70;
  longlong *local_68;
  int local_60;
  short *local_58;
  int local_50;
  undefined2 local_48;
  undefined2 local_46;
  undefined4 local_44;
  
  plVar16 = (longlong *)0x0;
  local_res10 = param_2;
  local_res18 = param_3;
  local_res20 = param_4;
  if (param_2 == (longlong *)0x0) {
    local_68 = (longlong *)*param_3;
    local_60 = *(int *)(param_3 + 1);
    uVar11 = FUN_140269cd0((longlong *)&local_68);
    iVar6 = (int)uVar11;
  }
  else {
    piVar9 = (int *)(**(code **)(*param_2 + 0x130))(param_2);
    plVar13 = *(longlong **)(param_1 + 0x170);
    plVar15 = plVar13 + *(uint *)(param_1 + 0x17c);
    for (; plVar13 != plVar15; plVar13 = plVar13 + 1) {
      piVar10 = (int *)(**(code **)(**(longlong **)(*plVar13 + 0x78) + 0x130))();
      if ((((*piVar10 == *piVar9) && (piVar10[1] == piVar9[1])) && (piVar10[2] == piVar9[2])) &&
         (piVar10[3] == piVar9[3])) {
        plVar15 = (longlong *)*plVar13;
        uVar14 = (uint)((longlong)plVar13 - *(longlong *)(param_1 + 0x170) >> 3);
        goto LAB_14025d333;
      }
    }
    uVar14 = 0xffffffff;
    plVar15 = plVar16;
LAB_14025d333:
    local_68 = (longlong *)*param_3;
    piVar9 = (int *)(param_3 + 1);
    local_60 = *piVar9;
    uVar11 = FUN_140269cd0((longlong *)&local_68);
    iVar6 = (int)uVar11;
    local_70 = iVar6;
    if (plVar15 != (longlong *)0x0) {
      if (((*(byte *)(param_1 + 400) & 1) != 0) && ((*(uint *)(plVar15 + 0xd) & 2) == 0)) {
        *(uint *)(plVar15 + 0xd) = *(uint *)(plVar15 + 0xd) | 2;
        FUN_140ffb580((longlong)(plVar15 + 1));
      }
      if (local_70 != 0) {
        puVar17 = (uint *)(param_1 + 0x17c);
        local_res10 = (longlong *)CONCAT44(local_res10._4_4_,uVar14);
        uVar1 = *puVar17;
        if (uVar1 <= uVar14) {
          FUN_140233f00("D:\\Perforce\\Live\\NAEU\\v2\\Code\\Arena\\Core\\Collections\\Array.h",
                        0x531,0x1418fc310,(undefined4 *)&local_res10,puVar17);
          pcVar2 = (code *)swi(3);
          (*pcVar2)();
          return;
        }
        if (uVar14 + 1 < uVar1) {
          *(undefined8 *)(*(longlong *)(param_1 + 0x170) + (ulonglong)uVar14 * 8) =
               *(undefined8 *)(*(longlong *)(param_1 + 0x170) + (ulonglong)(uVar1 - 1) * 8);
        }
        *puVar17 = *puVar17 - 1;
        plVar13 = (longlong *)(param_1 + -8);
        if (param_1 == 0) {
          plVar13 = plVar16;
        }
        FUN_140259850((longlong)(param_2 + 2),(longlong *)&local_58,plVar13);
        local_88 = local_58;
        local_80 = local_50;
        FUN_140276290(plVar13,param_2,(longlong *)&local_88);
        FUN_140ffa9a0((longlong)(plVar15 + 1));
                    /* WARNING: Subroutine does not return */
        thunk_FUN_1409bf4b0((longlong)plVar15);
      }
      local_80 = *piVar9;
      local_88 = (short *)*param_3;
      local_68 = (longlong *)(param_1 + -8);
      if (param_1 == 0) {
        local_68 = plVar16;
      }
      if ((local_80 == 0) || ((local_80 == 1 && (*local_88 == 0)))) {
                    /* WARNING: Subroutine does not return */
        FUN_1409cd550();
      }
      piVar10 = (int *)(**(code **)(*local_68 + 0xa8))(local_68);
      puVar17 = (uint *)param_2[0xb];
      local_78 = puVar17 + (ulonglong)*(uint *)((longlong)param_2 + 100) * 0xc;
      if (puVar17 != local_78) {
        do {
          piVar12 = (int *)(**(code **)(**(longlong **)(puVar17 + 2) + 0xa8))();
          if ((((*piVar12 == *piVar10) && (piVar12[1] == piVar10[1])) && (piVar12[2] == piVar10[2]))
             && (piVar12[3] == piVar10[3])) {
            if (puVar17 != (uint *)0x0) {
              *puVar17 = *puVar17 & 0xfffffffe;
              local_48 = 1;
              local_58 = (short *)0x0;
              local_50 = 0;
              uVar7 = FUN_1409a8e80();
              local_46 = (undefined2)uVar7;
              local_44 = 0;
              FUN_140266460((longlong *)&local_58,(longlong *)(puVar17 + 6),0xffffffff,0);
              FUN_140266460((longlong *)(puVar17 + 6),(longlong *)&local_88,0xffffffff,0);
              iVar6 = local_50;
              psVar4 = local_58;
              puVar3 = local_res18;
              if ((local_res20 != 0) && (uVar14 = puVar17[4], puVar17[4] = local_res20, uVar14 == 0)
                 ) {
                    /* WARNING: Subroutine does not return */
                _guard_check_icall();
              }
              if (((local_50 != 0) && ((local_50 != 1 || (*local_58 != 0)))) ||
                 ((*piVar9 != 0 && ((*piVar9 != 1 || (*(short *)*local_res18 != 0)))))) {
                iVar8 = *piVar9;
                if (local_50 == iVar8) {
                  if (local_50 == 0) {
                    iVar8 = -(uint)(iVar8 != 0);
                  }
                  else if (iVar8 == 0) {
                    iVar8 = 1;
                  }
                  else {
                    uVar11 = FUN_1409aaa90((longlong)local_58,(ushort *)*local_res18,-1);
                    iVar8 = (int)uVar11;
                  }
                  if (iVar8 == 0) goto LAB_14025d630;
                }
                local_88 = psVar4;
                local_80 = iVar6;
                bVar5 = FUN_140269b50((longlong *)&local_88);
                if ((int)CONCAT71(extraout_var,bVar5) != 0) {
                  local_88 = (short *)*puVar3;
                  local_80 = *(int *)(puVar3 + 1);
                  uVar11 = FUN_140269bc0((longlong *)&local_88);
                  if ((int)uVar11 != 0) {
                    local_88 = psVar4;
                    local_80 = iVar6;
                    plVar13 = (longlong *)(param_1 + -8);
                    if (param_1 == 0) {
                      plVar13 = plVar16;
                    }
                    FUN_140276190(plVar13,local_res10,(longlong *)&local_88);
                    goto LAB_14025d630;
                  }
                }
                local_88 = psVar4;
                local_80 = iVar6;
                uVar11 = FUN_140269bc0((longlong *)&local_88);
                if ((int)uVar11 != 0) {
                  local_88 = (short *)*puVar3;
                  local_80 = *(int *)(puVar3 + 1);
                  bVar5 = FUN_140269b50((longlong *)&local_88);
                  if ((int)CONCAT71(extraout_var_00,bVar5) != 0) {
                    local_88 = psVar4;
                    local_80 = iVar6;
                    plVar13 = (longlong *)(param_1 + -8);
                    if (param_1 == 0) {
                      plVar13 = plVar16;
                    }
                    FUN_140276350(plVar13,local_res10,&local_88);
                    goto LAB_14025d630;
                  }
                }
                local_88 = psVar4;
                local_80 = iVar6;
                plVar13 = (longlong *)(param_1 + -8);
                if (param_1 == 0) {
                  plVar13 = plVar16;
                }
                FUN_140276230(plVar13,local_res10,&local_88);
              }
LAB_14025d630:
              *(uint *)(plVar15 + 0xd) = *(uint *)(plVar15 + 0xd) & 0xfffffffe;
              if (psVar4 == (short *)0x0) {
                return;
              }
                    /* WARNING: Subroutine does not return */
              FUN_1409bf4b0((longlong)psVar4);
            }
            break;
          }
          puVar17 = puVar17 + 0xc;
        } while (puVar17 != local_78);
      }
                    /* WARNING: Subroutine does not return */
      FUN_1409cd550();
    }
  }
  if (iVar6 != 0) {
    return;
  }
                    /* WARNING: Subroutine does not return */
  FUN_1409a94a0();
}

