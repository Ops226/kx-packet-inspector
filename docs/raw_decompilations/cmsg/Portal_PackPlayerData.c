
void Portal::PackPlayerData(undefined4 param_1)

{
  int iVar1;
  longlong *plVar2;
  byte *pbVar3;
  longlong lVar4;
  undefined8 uVar5;
  char *pcVar6;
  ulonglong uVar7;
  short *psVar8;
  ulonglong uVar9;
  uint uVar10;
  ulonglong uVar11;
  longlong unaff_RBP;
  longlong *unaff_RDI;
  undefined **ppuVar12;
  code *pcVar13;
  undefined4 unaff_R13D;
  undefined4 unaff_000000ac;
  byte *pbVar14;
  uint unaff_R15D;
  char *in_stack_00000030;
  char *in_stack_00000058;
  char *in_stack_00000060;
  uint *in_stack_00000068;
  char *in_stack_00000070;
  uint *in_stack_00000078;
  
  *(undefined4 *)(unaff_RBP + -0x6e) = unaff_R13D;
  *(undefined2 **)(unaff_RBP + -0x23) = &DAT_1418fd248;
  *(undefined2 **)(unaff_RBP + -0x1b) = &DAT_1418fd248;
  plVar2 = (longlong *)(**(code **)(*unaff_RDI + 0xe8))(param_1,0,0xffffffff);
  if (plVar2 != (longlong *)0x0) {
    do {
      ppuVar12 = &PTR_DAT_141902250;
      (**(code **)*plVar2)(plVar2);
      pbVar3 = (byte *)(**(code **)(*plVar2 + 0x10))(plVar2);
      pcVar13 = *(code **)(*plVar2 + 0x80);
      lVar4 = (*pcVar13)(plVar2,"scope",0);
      if ((lVar4 == 0) || (uVar5 = FUN_140268cc0(lVar4), (int)uVar5 != 0)) {
        uVar11 = 0;
        do {
          pcVar6 = (char *)(**(code **)*plVar2)(plVar2);
          uVar5 = FUN_1409aaa50((longlong)*ppuVar12,pcVar6,-1);
          if ((int)uVar5 == 0) {
            if ((&DAT_141902258)[uVar11 * 8] == 4) {
              uVar5 = (**(code **)(*plVar2 + 0x10))(plVar2);
              iVar1 = FUN_140dc86b0(uVar5,(undefined1 (*) [32])(unaff_RBP + 0x20));
              if ((((iVar1 != 0) && (*(int *)(unaff_RBP + 0x20) == 0)) &&
                  (*(int *)(unaff_RBP + 0x24) == 0)) &&
                 ((*(int *)(unaff_RBP + 0x28) == 0 && (*(int *)(unaff_RBP + 0x2c) == 0)))) break;
            }
            if (uVar11 * 0x20 != -0x141902250) {
              pbVar14 = (byte *)(unaff_RBP + -0x70 + (&DAT_141902260)[uVar11 * 4]);
              switch((&DAT_141902258)[uVar11 * 8]) {
              case 0:
                uVar9 = thunk_FUN_140e4935c((longlong)pbVar3,(longlong *)&stack0x00000058,10);
                if ((0xff < uVar9) || (*in_stack_00000058 != '\0')) goto LAB_14027b63a;
                *pbVar14 = (byte)uVar9;
                break;
              case 1:
                if (0xfe < (uint)(&DAT_14190226c)[uVar11 * 8]) {
                    /* WARNING: Subroutine does not return */
                  FUN_1409cd550();
                }
                if ((&DAT_14190226c)[uVar11 * 8] == 0) {
                    /* WARNING: Subroutine does not return */
                  FUN_1409cd550();
                }
                if (*pbVar3 == 0) {
                  *pbVar14 = *pbVar14 & ~*(byte *)(&DAT_14190226c + uVar11 * 8);
                }
                else {
                  *pbVar14 = *pbVar14 | *(byte *)(&DAT_14190226c + uVar11 * 8);
                }
                break;
              case 2:
                uVar9 = thunk_FUN_140e491e8((longlong)pbVar3,(longlong *)&stack0x00000060,10,
                                            (int)pcVar13);
                if ((0x17fffffff < uVar9 + 0x80000000) || (*in_stack_00000060 != '\0'))
                goto LAB_14027b63a;
                *(int *)pbVar14 = (int)uVar9;
                break;
              case 3:
                uVar9 = thunk_FUN_140e491e8((longlong)pbVar3,(longlong *)&stack0x00000030,10,
                                            (int)pcVar13);
                if ((0x17fffffff < uVar9 + 0x80000000) || (*in_stack_00000030 != ','))
                goto LAB_14027b63a;
                in_stack_00000030 = in_stack_00000030 + 1;
                uVar7 = thunk_FUN_140e491e8((longlong)in_stack_00000030,(longlong *)&stack0x00000030
                                            ,10,(int)pcVar13);
                if ((0x17fffffff < uVar7 + 0x80000000) || (*in_stack_00000030 != '\0'))
                goto LAB_14027b63a;
                *(int *)pbVar14 = (int)uVar9;
                *(int *)(pbVar14 + 4) = (int)uVar7;
                break;
              case 4:
                if (*pbVar3 == 0) {
                  *(undefined8 *)(unaff_RBP + 0x18) = 0;
                  *(undefined8 *)(unaff_RBP + 0x10) = 0;
                  uVar5 = *(undefined8 *)(unaff_RBP + 0x18);
                  *(undefined8 *)pbVar14 = *(undefined8 *)(unaff_RBP + 0x10);
                  *(undefined8 *)(pbVar14 + 8) = uVar5;
                }
                else {
                  iVar1 = FUN_140dc86b0(pbVar3,(undefined1 (*) [32])(unaff_RBP + 0x10));
                  if (iVar1 == 0) goto LAB_14027b63a;
                  uVar5 = *(undefined8 *)(unaff_RBP + 0x18);
                  *(undefined8 *)pbVar14 = *(undefined8 *)(unaff_RBP + 0x10);
                  *(undefined8 *)(pbVar14 + 8) = uVar5;
                }
                break;
              case 5:
                if ((&DAT_14190226c)[uVar11 * 8] == 0) {
                    /* WARNING: Subroutine does not return */
                  FUN_1409cd550();
                }
                psVar8 = (short *)FUN_140e2e910(in_stack_00000068,
                                                (ulonglong)(uint)(&DAT_14190226c)[uVar11 * 8] * 2);
                FUN_1409acae0(psVar8,pbVar3,(ulonglong)(uint)(&DAT_14190226c)[uVar11 * 8],-1);
                *(short **)pbVar14 = psVar8;
                break;
              case 6:
                uVar9 = thunk_FUN_140e4935c((longlong)pbVar3,(longlong *)&stack0x00000070,10);
                if ((0xffff < uVar9) || (*in_stack_00000070 != '\0')) goto LAB_14027b63a;
                *(short *)pbVar14 = (short)uVar9;
                break;
              default:
                    /* WARNING: Subroutine does not return */
                FUN_1409cd700("D:\\Perforce\\Live\\NAEU\\v2\\Code\\Gw2\\Game\\Portal\\PlPack.cpp",
                              0x112,0x141902510,pcVar13);
              }
              *(uint *)(unaff_RBP + -0x6e) =
                   *(uint *)(unaff_RBP + -0x6e) | (&DAT_141902268)[uVar11 * 8];
              goto LAB_14027b63a;
            }
            break;
          }
          uVar10 = (int)uVar11 + 1;
          uVar11 = (ulonglong)uVar10;
          ppuVar12 = ppuVar12 + 4;
        } while (uVar10 < 0x14);
        if ((pbVar3 != (byte *)0x0) && (*pbVar3 != 0)) {
                    /* WARNING: Subroutine does not return */
          FUN_140242c60((short *)&stack0x00000038,unaff_R15D + 1);
        }
      }
LAB_14027b63a:
      plVar2 = (longlong *)(**(code **)(*plVar2 + 0x160))(plVar2,0);
    } while (plVar2 != (longlong *)0x0);
    if (0xfe < unaff_R15D) {
      *(undefined1 *)(unaff_RBP + -4) = 0xff;
      goto LAB_14027b686;
    }
  }
  *(char *)(unaff_RBP + -4) = (char)unaff_R15D;
LAB_14027b686:
  *(ulonglong *)(unaff_RBP + -3) = CONCAT44(unaff_000000ac,unaff_R13D);
  uVar5 = MsgConn::BuildPacketFromSchema
                    (*(short **)(unaff_RBP + -0x80),(uint *)&DAT_142514e00,0x75,unaff_RBP + -0x70,
                     in_stack_00000078);
  **(undefined8 **)(unaff_RBP + -0x78) = uVar5;
  if (CONCAT44(unaff_000000ac,unaff_R13D) == 0) {
                    /* WARNING: Subroutine does not return */
    FUN_140e37790(*(ulonglong *)(unaff_RBP + 0x30) ^ (ulonglong)&stack0x00000000);
  }
                    /* WARNING: Subroutine does not return */
  FUN_1409bf4b0(CONCAT44(unaff_000000ac,unaff_R13D));
}

