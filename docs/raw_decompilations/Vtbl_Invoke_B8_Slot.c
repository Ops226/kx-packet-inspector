
void Vtbl_Invoke_B8_Slot(longlong *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00014025369b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(*param_1 + 0xb8))();
  return;
}

