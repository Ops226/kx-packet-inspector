
void Handle_ConstructFromTuple
               (longlong param_1,ulonglong *param_2,undefined8 param_3,ushort *param_4)

{
  int iVar1;
  longlong *plVar2;
  ulonglong *puVar3;
  undefined8 *puVar4;
  undefined1 auStack_78 [32];
  undefined1 local_58 [16];
  ulonglong local_48;
  
  local_48 = DAT_142551900 ^ (ulonglong)auStack_78;
  plVar2 = (longlong *)FUN_140257a90(param_1 + 0xe0,param_2);
  if (plVar2 == (longlong *)0x0) {
    plVar2 = (longlong *)FUN_1402540e0(param_1 + 0xe0,param_2);
  }
  if ((param_4 != (ushort *)0x0) && (*param_4 != 0)) {
    iVar1 = (**(code **)(*plVar2 + 0xb8))(plVar2);
    if (iVar1 == 0) {
      puVar4 = (undefined8 *)(**(code **)(*plVar2 + 0xb0))(plVar2,local_58);
      FUN_1402584f0(param_1,puVar4,param_4);
    }
    else if (iVar1 == 1) {
      puVar3 = (ulonglong *)(**(code **)(*plVar2 + 0xb0))(plVar2,local_58);
      FUN_1402583b0(param_1,puVar3);
    }
  }
                    /* WARNING: Subroutine does not return */
  FUN_140e37790(local_48 ^ (ulonglong)auStack_78);
}

