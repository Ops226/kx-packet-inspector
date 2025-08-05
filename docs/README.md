# KX Packet Inspector Documentation

Welcome to the official documentation for the KX Packet Inspector project.

This repository aims to provide a powerful tool for understanding and analyzing network communication in Guild Wars 2. This documentation serves as a central hub for all reverse engineering findings, methodologies, and technical insights gained during the development of this tool.

## Documentation Structure

*   **[System Architecture](./system-architecture.md):** A high-level overview of the entire network message dispatch system, covering both incoming (SMSG) and outgoing (CMSG) packets.
*   **[Packets](./packets):** Detailed, evidence-backed analysis of specific packet families and their variants. This section now includes both [Server-to-Client (SMSG) packets](./packets/smsg/README.md) and [Client-to-Server (CMSG) packets](./packets/cmsg/README.md).
*   **[Methodologies](./methodologies):** Guides and playbooks for reverse engineering the game client. This now includes separate playbooks for [SMSG Packet Discovery](./methodologies/smsg-parser-discovery-playbook.md) and [CMSG Packet Discovery](./methodologies/cmsg-parser-discovery-playbook.md).
*   **[Raw Decompilations](./raw_decompilations):** Organized raw decompiled code snippets used as evidence for the architectural descriptions.

## Getting Started

*   To understand the overall network architecture, start with **[System Architecture](./system-architecture.md)**.
*   For a quick overview of all documented packets, see the main **[Packet Reference](./packets/README.md)**.
*   For detailed information on specific SMSG packets, see the [SMSG Packets directory](./packets/smsg).
*   For detailed information on specific CMSG packets, see the [CMSG Packets directory](./packets/cmsg).
*   For the step-by-step process of discovering SMSG packet parsers, consult the **[SMSG Parser Discovery Playbook](./methodologies/smsg-parser-discovery-playbook.md)**.
*   For the step-by-step process of discovering CMSG packet builders, consult the **[CMSG Parser Discovery Playbook](./methodologies/cmsg-parser-discovery-playbook.md)**.
*   As a supplementary technique for validating findings with live memory, see the **[Memory Correlation Workflow](./methodologies/memory-correlation-workflow.md)**.

We hope this documentation provides valuable insights and aids in your exploration of Guild Wars 2's internals.