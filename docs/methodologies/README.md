# Methodologies

This directory contains detailed documentation on the methodologies and workflows used for reverse engineering the Guild Wars 2 client, particularly focusing on network packet analysis.

*   **[Hooking Implementation Details](./hooking-implementation.md)**: Describes the practical implementation of the packet hooking mechanism.
*   **[Memory Correlation Workflow](./memory-correlation-workflow.md)**: Details a foundational technique for deducing packet structures by correlating live memory values with captured packet data.
*   **[Packet Parser Discovery Playbook (SMSG Dynamic Analysis Workflow)](./smsg-parser-discovery-playbook.md)**: Provides a concise, repeatable method for discovering Server-to-Client (SMSG) packet structures based on dynamic analysis.
*   **[Packet Parser Discovery Playbook (CMSG Workflow)](./cmsg-parser-discovery-playbook.md)**: Provides a concise, repeatable method for discovering Client-to-Server (CMSG) packet structures using a static-first approach.

These documents are crucial for understanding the approach taken in this project and for replicating or extending the research.