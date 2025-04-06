#pragma once

#include "PacketData.h" // For PacketInfo
#include "PacketStructures.h" // For MovementPayload etc.
#include <optional>
#include <vector>
#include <string> // For return type std::string

namespace kx::Parsing {

    /**
     * @brief Central dispatcher to get a formatted tooltip string for any known parsed packet.
     * @param packet The PacketInfo object.
     * @return An optional string suitable for display in a tooltip if the packet was parsed,
     *         otherwise std::nullopt.
     */
    std::optional<std::string> GetParsedDataTooltipString(const kx::PacketInfo& packet);

    /**
     * @brief Attempts to parse the payload of a known movement packet.
     * @param packet The PacketInfo object containing the packet data.
     * @return An optional containing a formatted string representation of the payload
     *         if parsing is successful and applicable, otherwise std::nullopt.
     */
    std::optional<std::string> ParseMovementPacket(const kx::PacketInfo& packet);

    // Declarations for other packet parsers can be added here later.
    // e.g., std::optional<kx::Packets::SomeOtherPayload> ParseSomeOtherPacket(const kx::PacketInfo& packet);

} // namespace kx::Parsing