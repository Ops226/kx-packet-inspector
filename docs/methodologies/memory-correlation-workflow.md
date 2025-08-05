# The Memory Correlation Workflow

This document describes the Memory Correlation Method, a foundational technique for reverse engineering game packets. 

While the project now primarily relies on more advanced static analysis (see the [Packet Parser Discovery Playbook](./packet-parser-discovery-playbook.md)), this method is an excellent starting point for beginners and remains a powerful tool for validating findings from static analysis.

## Core Philosophy

While static analysis in Ghidra is instrumental in mapping the system's architecture, the extreme levels of indirection (vtables, callbacks, jump tables) can make it an unreliable method for finding specific packet parsers. The Memory Correlation Method provides a practical, dynamic approach to deduce a packet's structure.

This workflow is the foundation of all further research:

1.  **Capture:** Use `kx-packet-inspector` to get a clean log of the target packet.
2.  **Freeze:** Use a debugger (e.g., Cheat Engine) to pause the game at a relevant moment.
3.  **Find:** Locate the corresponding game objects (e.g., the player character) in live memory.
4.  **Compare:** Correlate the raw hex data of the captured packet with the live values in memory to deduce the packet's structure.
