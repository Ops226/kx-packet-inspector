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

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <utility> // For std::pair
#include <string_view> // For magic_enum results
#include "../MagicEnum/magic_enum.hpp"

#include "PacketData.h" // Required for PacketDirection enum definition

namespace kx {

    // --- Client->Server Header IDs ---
    enum class CMSG_HeaderId : uint8_t {
        CHAT_SEND_MESSAGE = 0xF9,
        USE_SKILL = 0x17,
        MOVEMENT = 0x12,
        MOVEMENT_WITH_ROTATION = 0x0B,
        MOVEMENT_END = 0x04,
        JUMP = 0x09,
        HEARTBEAT = 0x11,
        SELECT_AGENT = 0xDE,
        DESELECT_AGENT = 0xD6,
        MOUNT_MOVEMENT = 0x18,
        // Add more CMSG IDs as they are identified
    };

    // --- Server->Client Header IDs ---
    enum class SMSG_HeaderId : uint8_t {
        // Example (replace with actual identified IDs)
        // AGENT_UPDATE       = 0x??,
        // CHAT_RECEIVE       = 0x??,
        // SKILL_RESULT       = 0x??,
        PLACEHOLDER = 0x00 // Remove or replace once real IDs are found
    };



    // --- Public API ---

    /**
     * @brief Gets a descriptive string name for a packet based on its direction and raw header ID.
     * @param direction The direction of the packet (Sent or Received).
     * @param rawHeaderId The uint8_t header ID read from the packet data.
     * @return std::string The descriptive name (e.g., "CMSG_USE_SKILL") or a formatted unknown string
     *                     (e.g., "SMSG_UNKNOWN [0xAB]").
     */
    inline std::string GetPacketName(PacketDirection direction, uint8_t rawHeaderId) {
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
            ss << prefix << "UNKNOWN [0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(rawHeaderId) << "]";
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
    inline std::vector<std::pair<uint8_t, std::string>> GetKnownCMSGHeaders() {
        std::vector<std::pair<uint8_t, std::string>> headers;
        constexpr auto entries = magic_enum::enum_entries<CMSG_HeaderId>();
        headers.reserve(entries.size());
        for (const auto& [value, name_sv] : entries) {
            headers.emplace_back(static_cast<uint8_t>(value), "CMSG_" + std::string(name_sv));
        }
        // Consider sorting if needed
        return headers;
    }

    /**
     * @brief Provides a list of known SMSG headers for UI population.
     * @return A vector of pairs, where each pair contains the {Header ID, Header Name}.
     */
    inline std::vector<std::pair<uint8_t, std::string>> GetKnownSMSGHeaders() {
        std::vector<std::pair<uint8_t, std::string>> headers;
        constexpr auto entries = magic_enum::enum_entries<SMSG_HeaderId>();
        headers.reserve(entries.size());
        bool onlyPlaceholder = (entries.size() == 1 && entries[0].first == SMSG_HeaderId::PLACEHOLDER);

        for (const auto& [value, name_sv] : entries) {
            // Skip placeholder only if it's the *only* entry
            if (onlyPlaceholder && value == SMSG_HeaderId::PLACEHOLDER) {
                continue;
            }
            headers.emplace_back(static_cast<uint8_t>(value), "SMSG_" + std::string(name_sv));
        }
        // Consider sorting
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