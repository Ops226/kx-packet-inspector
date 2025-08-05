
void Gs2c_OpcodeRouter(longlong param_1,undefined4 param_2,undefined4 param_3)

{
  *(undefined4 *)(param_1 + 0xab0) = param_2;
  *(undefined4 *)(param_1 + 0xab4) = param_3;
                    /* WARNING: Subroutine does not return */
  FUN_140250880((undefined4 *)(param_1 + 0xad8),Vtbl_Invoke_B8_Slot);
}

