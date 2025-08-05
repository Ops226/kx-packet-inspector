
undefined8
Selector_MapOrValidate(int param_1,undefined8 param_2,undefined8 param_3,undefined8 param_4)

{
  if (param_1 == 0) {
    return 0;
  }
  if (param_1 == 1) {
    return 1;
  }
  if (param_1 == 2) {
    return 2;
  }
  if (param_1 != 3) {
                    /* WARNING: Subroutine does not return */
    FUN_1409cd700("D:\\Perforce\\Live\\NAEU\\v2\\Code\\Gw2\\Game\\Portal\\PlUtil.cpp",0x104,
                  0x141900438,param_4);
  }
  return 3;
}

