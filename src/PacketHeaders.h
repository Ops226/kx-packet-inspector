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
        SESSION_TICK = 0x0001,
        PERFORMANCE_RESPONSE = 0x0002,
        MOVEMENT_END = 0x0004,
        PING_RESPONSE = 0x0006,
        JUMP = 0x0009,
        MOVEMENT_WITH_ROTATION = 0x000B,
        HEARTBEAT = 0x0011,
        MOVEMENT = 0x0012,
        CONTEXT_MENU_REQUEST = 0x0014,
        USE_SKILL = 0x0017,
        MOUNT_MOVEMENT = 0x0018,
        LANDED = 0x001A,
        LOGOUT_TO_CHAR_SELECT = 0x0023,
        UNKNOWN_0x0024 = 0x0024,
        UNKNOWN_0x0025 = 0x0025,
        UNKNOWN_0x0028 = 0x0028,
        UNKNOWN_0x002B = 0x002B,
        AGENT_LINK = 0x0036,
        SIMPLE_U32_0x0050 = 0x0050,
        UNKNOWN_0x0051 = 0x0051,
        UNKNOWN_0x005B = 0x005B,
        UNKNOWN_0x005D = 0x005D,
        UNKNOWN_0x005F = 0x005F,
        UNKNOWN_0x0065 = 0x0065,
        UNKNOWN_0x0068 = 0x0068,
        UNKNOWN_0x0069 = 0x0069,
        UNKNOWN_0x006A = 0x006A,
        UNKNOWN_0x006B = 0x006B,
        DESELECT_AGENT = 0x00DD,
        SELECT_AGENT = 0x00E5,
        // Note: Values above 256 (0x0100) won't work with magic_enum
        CHAT_MESSAGE = 0x0100,
        INTERACT_WITH_AGENT = 0x010E,
        INTERACTION_RESPONSE = 0x010F,
        CLIENT_STATE_SYNC = 0x0113,
    };


    // --- Server->Client Header IDs ---
    enum class SMSG_HeaderId : uint16_t {
        AGENT_UPDATE_BATCH = 0x0001,    // Confirmed: Container for many agent-related events
        PLAYER_STATE_UPDATE = 0x0002,   // Confirmed: Periodic state update for the player
        PERFORMANCE_MSG = 0x0008,       // Unconfirmed
        INTERACTION_DIALOGUE = 0x000F,  // Unconfirmed: Multi-part message sequence for NPC dialogue
        UI_MESSAGE = 0x0014,            // Unconfirmed: UI state, notifications, chat, etc.
        PLAYER_DATA_UPDATE = 0x0015,    // Unconfirmed: Currency, inventory, achievements
        AGENT_STATE_BULK = 0x0016,      // Unconfirmed: Bulk agent states/effects
        SKILL_UPDATE = 0x0017,          // Unconfirmed: Skill bar, cooldowns, action results
        CONFIG_UPDATE = 0x001A,         // Unconfirmed: Settings/configuration sync
        AGENT_EFFECT_UPDATE = 0x001C,   // Unconfirmed: Agent effects, status, or interactions
        MAP_DATA_BLOCK = 0x0020,        // Unconfirmed: Map geometry, entities, terrain data
        PET_INFO = 0x0021,              // Unconfirmed: Pet/minion state
        MAP_DETAIL_INFO = 0x0023,       // Unconfirmed: Detailed info for specific map elements
        MAP_LOAD_STATE = 0x0026,        // Unconfirmed: Map loading sequence/status updates
        SERVER_COMMAND = 0x0027,        // Confirmed: Server-initiated command (e.g., ping/perf check)
        AGENT_ATTRIBUTE_UPDATE = 0x0028,// Unconfirmed: Agent stats, buffs/debuffs
        AGENT_APPEARANCE = 0x002B,      // Unconfirmed: Agent visuals, equipment loading
        AGENT_SYNC = 0x0034,            // Unconfirmed: Periodic sync of multiple nearby agents
        AGENT_LINK = 0x0036,            // Unconfirmed: Linking agents (target, relationship?)
        SOCIAL_UPDATE = 0x0039,         // Unconfirmed: Guild, party, friends list update
        TIME_SYNC = 0x003F,             // Confirmed: Periodic server tick/time update
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
            // Return generic unknown name; opcode will be added by formatting function
            return prefix + "UNKNOWN";
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