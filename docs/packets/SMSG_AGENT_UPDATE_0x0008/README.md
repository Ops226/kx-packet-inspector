# SMSG_AGENT_UPDATE (0x0008)

This packet is responsible for communicating updates about agents (players, NPCs, etc.) in the game world.

## Polymorphism

This packet is a prime example of polymorphism in the Guild Wars 2 network protocol. A single opcode, `0x0008`, can represent multiple different packet structures. The game's dispatch system uses the **total packet size** to determine which variant is being sent and routes it to the appropriate handler.

### Known Variants

*   **[Tiny Movement Variant](./tiny-movement-variant.md):** A small, high-frequency packet used for minor positional updates.
*   **Full Agent Spawn Variant:** A large packet used to communicate the full state of a newly spawned agent, including health, appearance, and other attributes. (Documentation for this variant is forthcoming).
