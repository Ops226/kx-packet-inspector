
void Parse_TinyMovementPayload(longlong param_1)

{
  longlong *plVar1;
  
  plVar1 = (longlong *)FUN_1410f5f90(param_1);
  if (plVar1 != (longlong *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0001410eb75c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar1 + 0x38))(plVar1,*(code **)(*plVar1 + 0x38));
    return;
  }
  return;
}

