
longlong *
TM_GetEntryOrAux_ByParseCtx_Param2Gate
          (longlong param_1,int param_2,undefined8 param_3,undefined8 param_4)

{
  undefined *puVar1;
  longlong *plVar2;
  
  puVar1 = TM_Table_GetBasePtr();
  plVar2 = (longlong *)TM_Indexer_SelectEntryBySubtype((longlong)puVar1,param_1);
  if ((plVar2 != (longlong *)0x0) && (param_2 != 0)) {
    if (param_2 != 1) {
                    /* WARNING: Subroutine does not return */
      FUN_1409cd700("D:\\Perforce\\Live\\NAEU\\v2\\Code\\Gw2\\Game\\AgentView\\AvBase.cpp",0x3e,
                    0x141ab3c48,param_4);
    }
    plVar2 = (longlong *)(**(code **)(*plVar2 + 0x158))(plVar2);
  }
  return plVar2;
}

