
void Table_CreateIfMissing(longlong param_1,ulonglong *param_2)

{
  longlong lVar1;
  
  lVar1 = Table_GetOrFindByKey(param_1,param_2);
  if (lVar1 != 0) {
                    /* WARNING: Subroutine does not return */
    FUN_1409cd550();
  }
                    /* WARNING: Subroutine does not return */
  FUN_1409a94a0();
}

