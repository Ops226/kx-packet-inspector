# Raw Decompilations

This directory contains raw decompiled C code snippets from the Guild Wars 2 client, provided as direct evidence for the architectural documents.

The decompilations are organized by their primary domain:

*   **[/smsg/](./smsg/)**: Functions related to the Server-to-Client message pipeline.
*   **[/cmsg/](./cmsg/)**: Functions related to the Client-to-Server message pipeline.
*   **[/common/](./common/)**: Shared utility functions, such as the schema parsers, used by both pipelines.