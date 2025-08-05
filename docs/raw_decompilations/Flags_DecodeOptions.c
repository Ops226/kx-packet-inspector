
uint * Flags_DecodeOptions(uint *param_1,uint param_2)

{
  uint *puVar1;
  
  *param_1 = 0;
  puVar1 = &DAT_1419001c0;
  do {
    if ((puVar1[1] & param_2) != 0) {
      *param_1 = *param_1 | *puVar1;
    }
    puVar1 = puVar1 + 2;
  } while (puVar1 != (uint *)"scope");
  return param_1;
}

