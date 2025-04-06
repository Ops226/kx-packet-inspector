#pragma once

namespace kx::Packets {

    // Basic structure for movement packet payload (X, Y, Z coordinates)
    // Based on analysis suggesting these are read from offset size - 16.
    struct MovementPayload {
        float x;
        float y;
        float z;
    };

    // Add definitions for other packet payloads here as they are identified.

} // namespace kx::Packets