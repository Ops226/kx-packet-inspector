
undefined8 TM_Indexer_SelectEntryBySubtype(longlong param_1,longlong param_2)

{
  if ((param_2 != 0) && (*(uint *)(param_2 + 0xc) < *(uint *)(param_1 + 0x14))) {
    return *(undefined8 *)(*(longlong *)(param_1 + 8) + (ulonglong)*(uint *)(param_2 + 0xc) * 8);
  }
  return 0;
}

