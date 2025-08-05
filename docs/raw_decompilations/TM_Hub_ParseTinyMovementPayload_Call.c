
void TM_Hub_ParseTinyMovementPayload_Call(longlong param_1)

{
  longlong lVar1;
  longlong *plVar2;
  
  lVar1 = (*(code *)**(undefined8 **)(param_1 + 8))(param_1 + 8);
  plVar2 = (longlong *)FUN_1410f5f90(lVar1);
  if (plVar2 != (longlong *)0x0) {
                    /* WARNING: Could not recover jumptable at 0x0001410eb809. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(*plVar2 + 0x38))(plVar2,*(code **)(*plVar2 + 0x38));
    return;
  }
  return;
}

