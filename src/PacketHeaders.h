#pragma once

/*
 * NOTE ON SCOPE:
 * This file defines known packet headers (opcodes) for identification purposes.
 * It primarily focuses on Client-to-Server (CMSG) messages identified so far.
 * Server-to-Client (SMSG) messages often share opcodes but can have different structures.
 * The target client potentially contains around ~1700 distinct network messages,
 * falling into roughly ~35 relevant categories. This list represents only a
 * small subset. Contributions for defining more headers are welcome.
 * It also includes special values for representing internal states like encrypted packets.
 */

#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 256

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <utility> // For std::pair
#include <string_view> // For magic_enum results
#include "../libs/MagicEnum/magic_enum.hpp"

#include "PacketData.h" // Required for PacketDirection enum definition

namespace kx {

    // --- Client->Server Header IDs ---
    enum class CMSG_HeaderId : uint16_t {
        MOVEMENT_END = 0x0004,
        JUMP = 0x0009,
        MOVEMENT_WITH_ROTATION = 0x000B,
        HEARTBEAT = 0x0011,
        MOVEMENT = 0x0012,
        USE_SKILL = 0x0017,
        MOUNT_MOVEMENT = 0x0018,
        DESELECT_AGENT = 0x00DD,
        SELECT_AGENT = 0x00E5
        // Note: Values above 256 (0x0100) won't work with magic_enum
    };

    // --- Server->Client Header IDs ---
    enum class SMSG_HeaderId : uint16_t {
        PING_REQUEST = 0x000C,
    };



    // --- Public API ---

    /**
     * @brief Gets a descriptive string name for a packet based on its direction and raw header ID.
     * @param direction The direction of the packet (Sent or Received).
     * @param rawHeaderId The uint16_t header ID read from the packet data.
     * @return std::string The descriptive name (e.g., "CMSG_USE_SKILL") or a formatted unknown string
     *                     (e.g., "SMSG_UNKNOWN [0xABCD]").
     */
    inline std::string GetPacketName(PacketDirection direction, uint16_t rawHeaderId) {
        std::optional<std::string_view> name_sv;
        std::string prefix;

        if (direction == PacketDirection::Sent) {
            prefix = "CMSG_";
            if (auto value = magic_enum::enum_cast<CMSG_HeaderId>(rawHeaderId)) {
                name_sv = magic_enum::enum_name(*value);
            }
        } else {
            prefix = "SMSG_";
            if (auto value = magic_enum::enum_cast<SMSG_HeaderId>(rawHeaderId)) {
                name_sv = magic_enum::enum_name(*value);
            }
        }

        if (name_sv) {
            return prefix + std::string(*name_sv);
        } else {
            // Format unknown header directly
            std::stringstream ss;
            ss << prefix << "UNKNOWN [0x" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(rawHeaderId) << "]";
            return ss.str();
        }
    }

    /**
    * @brief Gets a descriptive string name for a special internal packet type.
    * @param type The InternalPacketType enum value.
    * @return std::string The corresponding name, or "Internal Error" if type not mapped.
    */
    inline std::string GetSpecialPacketTypeName(InternalPacketType type) {
        // Directly return the name provided by magic_enum
        return std::string(magic_enum::enum_name(type));
    }


    /**
     * @brief Provides a list of known CMSG headers for UI population.
     * @return A vector of pairs, where each pair contains the {Header ID, Header Name}.
     */
    inline std::vector<std::pair<uint16_t, std::string>> GetKnownCMSGHeaders() {
        std::vector<std::pair<uint16_t, std::string>> headers;
        constexpr auto entries = magic_enum::enum_entries<CMSG_HeaderId>();
        headers.reserve(entries.size());
        for (const auto& [value, name_sv] : entries) {
            headers.emplace_back(static_cast<uint16_t>(value), "CMSG_" + std::string(name_sv));
        }
        // Consider sorting if needed
        return headers;
    }

    /**
     * @brief Provides a list of known SMSG headers for UI population.
     * @return A vector of pairs, where each pair contains the {Header ID, Header Name}.
     */
    inline std::vector<std::pair<uint16_t, std::string>> GetKnownSMSGHeaders() {
        std::vector<std::pair<uint16_t, std::string>> headers;
        constexpr auto entries = magic_enum::enum_entries<SMSG_HeaderId>();
        headers.reserve(entries.size());
        for (const auto& [value, name_sv] : entries) {
            headers.emplace_back(static_cast<uint16_t>(value), "SMSG_" + std::string(name_sv));
        }

        return headers;
    }

    /**
    * @brief Provides a list of special internal packet types for UI filtering.
    * @return A vector of pairs, where each pair contains the {InternalPacketType, Type Name}.
    */
    inline std::vector<std::pair<InternalPacketType, std::string>> GetSpecialPacketTypesForFilter() {
        std::vector<std::pair<InternalPacketType, std::string>> types;
        constexpr auto entries = magic_enum::enum_entries<InternalPacketType>();
        types.reserve(entries.size()); // Reserve approx size

        for (const auto& [value, name_sv] : entries) {
            // Skip NORMAL type as it's not usually filtered explicitly as a "special" type
            if (value == InternalPacketType::NORMAL) {
                continue;
            }
            // Use the name directly from GetSpecialPacketTypeName (which uses magic_enum)
            types.emplace_back(value, GetSpecialPacketTypeName(value));
        }
        // Note: UNKNOWN_HEADER name is now directly from the enum via GetSpecialPacketTypeName

        // Sort maybe?
        return types;
    }

}