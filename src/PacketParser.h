#pragma once

#include "PacketData.h" // For PacketInfo
#include <optional>
#include <string> // For return type std::string

namespace kx::Parsing {

    /**
     * @brief Central dispatcher to get a formatted tooltip string for any known parsed packet.
     * @param packet The PacketInfo object.
     * @return An optional string suitable for display in a tooltip if the packet was parsed,
     *         otherwise std::nullopt.
     */
    std::optional<std::string> GetParsedDataTooltipString(const kx::PacketInfo& packet);

} // namespace kx::Parsing