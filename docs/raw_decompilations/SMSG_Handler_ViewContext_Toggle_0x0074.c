
void SMSG_Handler_ViewContext_Toggle_0x0074(undefined8 param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  undefined **ppuVar3;
  
  if (param_2 != 0x74) {
    return;
  }
  ppuVar3 = VdfContext_Get();
  uVar1 = (**(code **)(*ppuVar3 + 0x168))(ppuVar3);
  if (uVar1 != 0) {
    if (uVar1 == 1) {
      iVar2 = FUN_140a59500(2,0x19,1);
      ppuVar3 = VdfContext_Get();
                    /* WARNING: Could not recover jumptable at 0x00014093f86b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)(*ppuVar3 + 0x170))(ppuVar3,-(iVar2 != 0) & 2,*(code **)(*ppuVar3 + 0x170));
      return;
    }
    if (uVar1 != 2) {
                    /* WARNING: Subroutine does not return */
      FUN_1409cd700("D:\\Perforce\\Live\\NAEU\\v2\\Code\\Gw2\\Game\\View\\Default\\VdfContext.cpp",
                    0xef6,0x141b025e8,(ulonglong)uVar1);
    }
  }
  ppuVar3 = VdfContext_Get();
                    /* WARNING: Could not recover jumptable at 0x00014093f889. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*ppuVar3 + 0x170))(ppuVar3,1,*(code **)(*ppuVar3 + 0x170));
  return;
}

