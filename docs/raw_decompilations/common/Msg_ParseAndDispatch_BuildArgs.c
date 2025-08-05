
ulonglong *
Msg_ParseAndDispatch_BuildArgs
          (uint *param_1,ulonglong *param_2,ushort *param_3,ulonglong *param_4,ulonglong param_5,
          uint *param_6,undefined4 *param_7,uint *param_8)

{
  byte *pbVar1;
  longlong lVar2;
  char cVar3;
  byte bVar4;
  ushort uVar5;
  uint uVar6;
  ulonglong uVar7;
  ulonglong uVar8;
  undefined8 uVar9;
  ushort *puVar10;
  longlong extraout_RAX;
  longlong extraout_RAX_00;
  longlong extraout_RAX_01;
  ulonglong *puVar11;
  longlong extraout_RAX_02;
  undefined1 *puVar12;
  int iVar13;
  uint uVar14;
  ulonglong uVar15;
  uint uVar16;
  IMAGE_DOS_HEADER *pIVar17;
  ulonglong *local_38 [2];
  
  local_38[0] = param_4;
  if (param_5 == 0) {
                    /* WARNING: Subroutine does not return */
    FUN_1409adab0(2,0x1420de790,0,param_4);
  }
  uVar16 = *param_1;
  if ((uVar16 == 0) || (uVar16 == 0x18)) {
    return param_4;
  }
  do {
    pIVar17 = &IMAGE_DOS_HEADER_140000000;
    lVar2 = (ulonglong)uVar16 * 8;
    if ((*(int *)(&DAT_1420de514 + (ulonglong)uVar16 * 8) != 0) &&
       (param_3 < (ushort *)((ulonglong)*(uint *)(&DAT_1420de510 + lVar2) + *param_2))) {
      return (ulonglong *)pIVar17;
    }
    if (param_5 < (byte *)((ulonglong)*(uint *)(&DAT_1420de510 + lVar2) + (longlong)param_4)) {
                    /* WARNING: Subroutine does not return */
      FUN_1409adab0(2,0x1420de7e0,param_5,&IMAGE_DOS_HEADER_140000000);
    }
    switch(uVar16) {
    case 1:
    case 3:
                    /* WARNING: Subroutine does not return */
      thunk_FUN_1418c47c0(param_4,(undefined8 *)*param_2,2);
    case 2:
      *(byte *)param_4 = *(byte *)*param_2;
      param_4 = (ulonglong *)((longlong)param_4 + 1);
      *param_2 = *param_2 + 1;
      local_38[0] = param_4;
      break;
    case 4:
      pIVar17 = (IMAGE_DOS_HEADER *)param_7;
      uVar9 = FUN_140fd4b40((longlong *)param_2,(byte *)param_3,(longlong *)local_38,param_7);
      param_4 = local_38[0];
      if ((int)uVar9 == 0) {
        return (ulonglong *)pIVar17;
      }
      break;
    case 5:
    case 7:
    case 0x1a:
                    /* WARNING: Subroutine does not return */
      thunk_FUN_1418c47c0(param_4,(undefined8 *)*param_2,8);
    case 6:
    case 0x19:
                    /* WARNING: Subroutine does not return */
      thunk_FUN_1418c47c0(param_4,(undefined8 *)*param_2,4);
    case 8:
                    /* WARNING: Subroutine does not return */
      thunk_FUN_1418c47c0(param_4,(undefined8 *)*param_2,0xc);
    case 9:
    case 0xb:
                    /* WARNING: Subroutine does not return */
      thunk_FUN_1418c47c0(param_4,(undefined8 *)*param_2,0x10);
    case 10:
                    /* WARNING: Subroutine does not return */
      thunk_FUN_1418c47c0(param_4,(undefined8 *)*param_2,0x10);
    case 0xc:
      puVar11 = (ulonglong *)*param_2;
      uVar7 = puVar11[1];
      uVar8 = puVar11[3];
      uVar15 = puVar11[2];
      *param_4 = *puVar11;
      param_4[1] = uVar7;
      param_4[2] = uVar15;
      *(int *)(param_4 + 3) = (int)uVar8;
      param_4 = (ulonglong *)((longlong)param_4 + 0x1c);
      *param_2 = *param_2 + 0x1c;
      local_38[0] = param_4;
      break;
    case 0xd:
      iVar13 = 0;
      for (puVar10 = (ushort *)*param_2; puVar10 < param_3; puVar10 = puVar10 + 1) {
        if (*puVar10 == 0) goto LAB_140fd45ac;
        iVar13 = iVar13 + 1;
      }
      if (puVar10 == param_3) {
        return (ulonglong *)pIVar17;
      }
LAB_140fd45ac:
      if (iVar13 == -1) {
        return (ulonglong *)pIVar17;
      }
      uVar15 = (ulonglong)(iVar13 * 2 + 2);
      if ((ulonglong)param_1[4] * 2 < uVar15) goto LAB_140fd4a95;
      if (param_3 < (ushort *)(*param_2 + uVar15)) {
        return (ulonglong *)pIVar17;
      }
      *param_4 = *param_2;
      param_4 = param_4 + 1;
      *param_2 = *param_2 + uVar15;
      local_38[0] = param_4;
      break;
    case 0xe:
      iVar13 = 0;
      for (puVar10 = (ushort *)*param_2; puVar10 < param_3;
          puVar10 = (ushort *)((longlong)puVar10 + 1)) {
        if ((char)*puVar10 == '\0') goto LAB_140fd4609;
        iVar13 = iVar13 + 1;
      }
      if (puVar10 == param_3) {
        return (ulonglong *)pIVar17;
      }
LAB_140fd4609:
      if (iVar13 == -1) {
        return (ulonglong *)pIVar17;
      }
      uVar16 = iVar13 + 1;
      if (param_1[4] < uVar16) goto LAB_140fd4a95;
      if (param_3 < (ushort *)((ulonglong)uVar16 + *param_2)) {
        return (ulonglong *)pIVar17;
      }
      *param_4 = *param_2;
      param_4 = param_4 + 1;
      *param_2 = *param_2 + (ulonglong)uVar16;
      local_38[0] = param_4;
      break;
    case 0xf:
      puVar10 = (ushort *)((char *)*param_2 + 1);
      if (param_3 < puVar10) {
        return (ulonglong *)pIVar17;
      }
      cVar3 = *(char *)*param_2;
      puVar11 = (ulonglong *)0x0;
      *param_2 = (ulonglong)puVar10;
      if (cVar3 != '\0') {
        uVar16 = param_1[8];
        if (uVar16 == 0) goto LAB_140fd4a95;
        puVar11 = (ulonglong *)FUN_140fd6d70(param_6,uVar16);
        pIVar17 = (IMAGE_DOS_HEADER *)
                  Msg_ParseAndDispatch_BuildArgs
                            (*(uint **)(param_1 + 6),param_2,param_3,puVar11,
                             (longlong)puVar11 + (ulonglong)uVar16,param_6,param_7,(uint *)0x0);
        if (extraout_RAX == 0) {
          return (ulonglong *)pIVar17;
        }
      }
      *param_4 = (ulonglong)puVar11;
      param_4 = param_4 + 1;
      local_38[0] = param_4;
      break;
    case 0x10:
      if (param_1[8] == 0) goto LAB_140fd4a95;
      uVar16 = param_1[8] * param_1[4];
      puVar11 = (ulonglong *)FUN_140fd6d70(param_6,uVar16);
      uVar15 = (ulonglong)uVar16 + (longlong)puVar11;
      *param_4 = (ulonglong)puVar11;
      uVar16 = 0;
      if (param_1[4] != 0) {
        do {
          pIVar17 = (IMAGE_DOS_HEADER *)
                    Msg_ParseAndDispatch_BuildArgs
                              (*(uint **)(param_1 + 6),param_2,param_3,puVar11,uVar15,param_6,
                               param_7,(uint *)0x0);
          if (extraout_RAX_00 == 0) {
            return (ulonglong *)pIVar17;
          }
          uVar16 = uVar16 + 1;
          puVar11 = (ulonglong *)((longlong)puVar11 + (ulonglong)param_1[8]);
        } while (uVar16 < param_1[4]);
      }
LAB_140fd4738:
      param_4 = param_4 + 1;
      local_38[0] = param_4;
      break;
    case 0x11:
      if (param_3 < (byte *)*param_2 + 1) {
        return (ulonglong *)pIVar17;
      }
      if (param_1[8] != 0) {
        bVar4 = *(byte *)*param_2;
        *(byte *)param_4 = bVar4;
        uVar16 = (uint)bVar4;
        if (uVar16 <= param_1[4]) {
          *param_2 = *param_2 + 1;
          uVar6 = param_1[8];
          puVar11 = (ulonglong *)FUN_140fd6d70(param_6,uVar16 * uVar6);
          *(ulonglong **)((longlong)param_4 + 1) = puVar11;
          uVar14 = 0;
          local_38[0] = (ulonglong *)((ulonglong)(uVar16 * uVar6) + (longlong)puVar11);
          if (uVar16 != 0) {
            do {
              pIVar17 = (IMAGE_DOS_HEADER *)
                        Msg_ParseAndDispatch_BuildArgs
                                  (*(uint **)(param_1 + 6),param_2,param_3,puVar11,
                                   (ulonglong)local_38[0],param_6,param_7,(uint *)0x0);
              if (extraout_RAX_01 == 0) {
                return (ulonglong *)pIVar17;
              }
              uVar14 = uVar14 + 1;
              puVar11 = (ulonglong *)((longlong)puVar11 + (ulonglong)param_1[8]);
            } while (uVar14 < bVar4);
          }
LAB_140fd47ec:
          param_4 = (ulonglong *)((longlong)param_4 + 9);
          local_38[0] = param_4;
          break;
        }
      }
      goto LAB_140fd4a95;
    case 0x12:
      if (param_3 < (ushort *)*param_2 + 1) {
        return (ulonglong *)pIVar17;
      }
      if (param_1[8] != 0) {
        uVar5 = *(ushort *)*param_2;
        *(ushort *)param_4 = uVar5;
        uVar16 = (uint)uVar5;
        if (uVar16 <= param_1[4]) {
          *param_2 = *param_2 + 2;
          uVar6 = param_1[8];
          puVar11 = (ulonglong *)FUN_140fd6d70(param_6,uVar16 * uVar6);
          *(ulonglong **)((longlong)param_4 + 2) = puVar11;
          uVar14 = 0;
          local_38[0] = (ulonglong *)((ulonglong)(uVar16 * uVar6) + (longlong)puVar11);
          if (uVar16 != 0) {
            do {
              pIVar17 = (IMAGE_DOS_HEADER *)
                        Msg_ParseAndDispatch_BuildArgs
                                  (*(uint **)(param_1 + 6),param_2,param_3,puVar11,
                                   (ulonglong)local_38[0],param_6,param_7,(uint *)0x0);
              if (extraout_RAX_02 == 0) {
                return (ulonglong *)pIVar17;
              }
              uVar14 = uVar14 + 1;
              puVar11 = (ulonglong *)((longlong)puVar11 + (ulonglong)param_1[8]);
            } while (uVar14 < uVar5);
          }
LAB_140fd489c:
          param_4 = (ulonglong *)((longlong)param_4 + 10);
          local_38[0] = param_4;
          break;
        }
      }
LAB_140fd4a95:
      *param_7 = 1;
LAB_140fd4aa7:
      return (ulonglong *)pIVar17;
    case 0x13:
      if (param_3 < (ushort *)((ulonglong)param_1[4] + *param_2)) {
        return (ulonglong *)pIVar17;
      }
      puVar12 = (undefined1 *)FUN_140fd6d70(param_6,param_1[4]);
      uVar16 = 0;
      *param_4 = (ulonglong)puVar12;
      if (param_1[4] == 0) goto LAB_140fd4738;
      do {
        uVar16 = uVar16 + 1;
        *puVar12 = *(undefined1 *)*param_2;
        *param_2 = *param_2 + 1;
        puVar12 = puVar12 + 1;
      } while (uVar16 < param_1[4]);
      param_4 = param_4 + 1;
      local_38[0] = param_4;
      break;
    case 0x14:
      if (param_3 < (byte *)*param_2 + 1) {
        return (ulonglong *)pIVar17;
      }
      bVar4 = *(byte *)*param_2;
      *(byte *)param_4 = bVar4;
      uVar16 = (uint)bVar4;
      if ((ushort)param_1[4] < uVar16) goto LAB_140fd4a95;
      uVar15 = (ulonglong)uVar16;
      if (param_3 < (ushort *)(*param_2 + 1 + (ulonglong)bVar4)) {
        return (ulonglong *)pIVar17;
      }
      pbVar1 = (byte *)(*param_2 + 1);
      *param_2 = (ulonglong)pbVar1;
      uVar6 = param_1[4];
      if (((param_8 != (uint *)0x0) && (*param_8 = 0xffffffff, pbVar1 != (byte *)0x0)) &&
         ((uVar6 >> 0x10 & 1) != 0)) {
        *param_8 = (uint)*pbVar1;
      }
      puVar12 = (undefined1 *)FUN_140fd6d70(param_6,uVar16);
      *(undefined1 **)((longlong)param_4 + 1) = puVar12;
      if (bVar4 == 0) goto LAB_140fd47ec;
      do {
        *puVar12 = *(undefined1 *)*param_2;
        *param_2 = *param_2 + 1;
        uVar15 = uVar15 - 1;
        puVar12 = puVar12 + 1;
      } while (uVar15 != 0);
      param_4 = (ulonglong *)((longlong)param_4 + 9);
      local_38[0] = param_4;
      break;
    case 0x15:
      if (param_3 < (ushort *)*param_2 + 1) {
        return (ulonglong *)pIVar17;
      }
      uVar5 = *(ushort *)*param_2;
      *(ushort *)param_4 = uVar5;
      uVar16 = (uint)uVar5;
      if ((ushort)param_1[4] < uVar16) goto LAB_140fd4a95;
      uVar15 = (ulonglong)uVar16;
      if (param_3 < (ushort *)(*param_2 + 2 + (ulonglong)uVar5)) {
        return (ulonglong *)pIVar17;
      }
      pbVar1 = (byte *)(*param_2 + 2);
      *param_2 = (ulonglong)pbVar1;
      uVar6 = param_1[4];
      if (((param_8 != (uint *)0x0) && (*param_8 = 0xffffffff, pbVar1 != (byte *)0x0)) &&
         ((uVar6 >> 0x10 & 1) != 0)) {
        *param_8 = (uint)*pbVar1;
      }
      puVar12 = (undefined1 *)FUN_140fd6d70(param_6,uVar16);
      *(undefined1 **)((longlong)param_4 + 2) = puVar12;
      if (uVar5 == 0) goto LAB_140fd489c;
      do {
        *puVar12 = *(undefined1 *)*param_2;
        *param_2 = *param_2 + 1;
        uVar15 = uVar15 - 1;
        puVar12 = puVar12 + 1;
      } while (uVar15 != 0);
      param_4 = (ulonglong *)((longlong)param_4 + 10);
      local_38[0] = param_4;
      break;
    case 0x16:
      goto LAB_140fd4aa7;
    case 0x17:
      *(undefined4 *)param_4 = *(undefined4 *)*param_2;
      param_4 = (ulonglong *)((longlong)param_4 + 4);
      *param_2 = *param_2 + 4;
      local_38[0] = param_4;
    }
    uVar16 = param_1[10];
    param_1 = param_1 + 10;
    if (uVar16 == 0) {
      return (ulonglong *)pIVar17;
    }
    if (uVar16 == 0x18) {
      return (ulonglong *)pIVar17;
    }
  } while( true );
}

