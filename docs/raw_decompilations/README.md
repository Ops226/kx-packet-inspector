# Raw Decompilations

This directory contains raw decompiled C code snippets from the Guild Wars 2 client. These files are provided as direct evidence and detailed context for understanding the game's network message processing system, as described in the [System Architecture](../../system-architecture.md) document.

The decompilations here are specifically chosen to illustrate key components of the dynamic message dispatch system:

*   **`Gs2c_SrvMsgDispatcher.c`**: The primary server message dispatcher.
*   **`Msg_BuildArgs_FromSchema.c`**: The function responsible for building message arguments from schemas.
*   **`Msg_ParseAndDispatch_BuildArgs.c`**: The schema virtual machine that parses raw bytes into structured data tuples.
*   **`Gs2c_PostParseDispatcher.c`**: An example of a dynamic post-parse handler.
*   **`SMSG_PostParse_Handler_Type03FE.c`**: Another example of a dynamic post-parse handler, specifically referenced in the [Packet Parser Discovery Playbook](../../methodologies/packet-parser-discovery-playbook.md).

These files are intended for advanced analysis and provide the low-level details that back the high-level architectural descriptions.