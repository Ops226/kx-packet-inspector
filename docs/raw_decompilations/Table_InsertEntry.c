
void Table_InsertEntry(longlong param_1,longlong *param_2,undefined4 param_3)

{
  undefined8 *puVar1;
  longlong lVar2;
  undefined1 auStack_98 [72];
  undefined8 local_50;
  undefined8 uStack_48;
  undefined4 local_40;
  ulonglong local_38;
  
  local_38 = DAT_142551900 ^ (ulonglong)auStack_98;
  puVar1 = (undefined8 *)(**(code **)(*param_2 + 0xb0))(param_2,&local_50);
  local_50 = *puVar1;
  uStack_48 = puVar1[1];
  local_40 = param_3;
  lVar2 = FUN_140257cf0(param_1 + 0x40,(uint *)&local_50);
  if (lVar2 == 0) {
                    /* WARNING: Subroutine does not return */
    FUN_1409a94a0();
  }
                    /* WARNING: Subroutine does not return */
  FUN_140e37790(local_38 ^ (ulonglong)auStack_98);
}

