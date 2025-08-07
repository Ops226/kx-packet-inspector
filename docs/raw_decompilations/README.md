# Raw Decompilations

This directory contains raw decompiled C code snippets from the Guild Wars 2 client, provided as direct evidence for the architectural documents and packet analyses.

The decompilations are organized by their primary domain or subsystem:

*   **[/smsg/](./smsg/)**: Functions primarily related to the Server-to-Client message pipeline.
*   **[/cmsg/](./cmsg/)**: Functions primarily related to the Client-to-Server message pipeline.
*   **[/common/](./common/)**: Shared utility functions, such as schema parsers, used by multiple pipelines or subsystems.
*   **[/web_interface/](./web_interface/)**: Functions related to the game's embedded web browser interface (e.g., Trading Post, Gem Store).
*   **[/engine_core/](./engine_core/)**: Core game engine components, high-level dispatchers, and fundamental system handlers.
