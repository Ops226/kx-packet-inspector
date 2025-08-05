
void MsgQueue_Dispatcher(undefined4 *param_1,undefined *param_2)

{
  longlong lVar1;
  longlong *plVar2;
  undefined8 *puVar3;
  undefined8 *puVar4;
  
  lVar1 = *(longlong *)(param_1 + 2);
  puVar3 = (undefined8 *)FUN_140259370(param_1);
  *puVar3 = 0;
  if (lVar1 == 0) {
    puVar3[1] = *(undefined8 *)(param_1 + 2);
    puVar3[2] = 0;
    if (*(longlong *)(param_1 + 2) == 0) {
      *(undefined8 **)(param_1 + 4) = puVar3;
    }
    else {
      *(undefined8 **)(*(longlong *)(param_1 + 2) + 0x10) = puVar3;
    }
    *(undefined8 **)(param_1 + 2) = puVar3;
  }
  else {
    puVar3[2] = *(undefined8 *)(lVar1 + 0x10);
    puVar3[1] = lVar1;
    if (*(longlong *)(lVar1 + 0x10) == 0) {
      *(undefined8 **)(param_1 + 2) = puVar3;
      *(undefined8 **)(lVar1 + 0x10) = puVar3;
    }
    else {
      *(undefined8 **)(*(longlong *)(lVar1 + 0x10) + 8) = puVar3;
      *(undefined8 **)(lVar1 + 0x10) = puVar3;
    }
  }
  plVar2 = (longlong *)puVar3[1];
  while (plVar2 != (longlong *)0x0) {
    puVar4 = (undefined8 *)FUN_140259370(param_1);
    *puVar4 = 0;
    puVar4[2] = plVar2;
    puVar4[1] = plVar2[1];
    if (plVar2[1] == 0) {
      *(undefined8 **)(param_1 + 4) = puVar4;
    }
    else {
      *(undefined8 **)(plVar2[1] + 0x10) = puVar4;
    }
    plVar2[1] = (longlong)puVar4;
    if (*plVar2 != 0) {
      (*(code *)param_2)();
    }
    if (puVar3[2] == 0) {
      *(undefined8 *)(param_1 + 2) = puVar3[1];
    }
    else {
      *(undefined8 *)(puVar3[2] + 8) = puVar3[1];
    }
    if (puVar3[1] == 0) {
      *(undefined8 *)(param_1 + 4) = puVar3[2];
    }
    else {
      *(undefined8 *)(puVar3[1] + 0x10) = puVar3[2];
    }
    puVar3[1] = *(undefined8 *)(param_1 + 6);
    *(undefined8 **)(param_1 + 6) = puVar3;
    puVar3 = puVar4;
    plVar2 = (longlong *)puVar4[1];
  }
  if (puVar3[2] == 0) {
    *(undefined8 *)(param_1 + 2) = puVar3[1];
  }
  else {
    *(undefined8 *)(puVar3[2] + 8) = puVar3[1];
  }
  if (puVar3[1] == 0) {
    *(undefined8 *)(param_1 + 4) = puVar3[2];
  }
  else {
    *(undefined8 *)(puVar3[1] + 0x10) = puVar3[2];
  }
  puVar3[1] = *(undefined8 *)(param_1 + 6);
  *(undefined8 **)(param_1 + 6) = puVar3;
  return;
}

