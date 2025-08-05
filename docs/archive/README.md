# Archive Notes

This folder contains analyses that were superseded by newer reverse engineering findings. They are preserved for historical reference.

Corrections:
- gs2c 0x0008 is an unused performance message, not an agent update.
- Agent-related gs2c messages are in the range 0x31–0x88 (complex; defer until later).
- Incoming dispatch is driven by a MsgCon (MsgConn) dispatch table; no vtables/polymorphic message hierarchy.
