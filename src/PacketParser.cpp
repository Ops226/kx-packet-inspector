#include "PacketParser.h"
#include "PacketHeaders.h" // For CMSG_HeaderId enum
#include <cstring> // For memcpy
#include <stdexcept>
#include <sstream> // For formatting the string
#include <iomanip> // For std::fixed, std::setprecision

namespace kx::Parsing {

    // Forward declarations
    std::optional<std::string> ParseMovementPacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParsePlayerStateUpdatePacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParseTimeSyncPacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParsePerformanceResponsePacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParseDeselectAgentPacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParseLogoutPacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParseSelectAgentPacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParseInteractWithAgentPacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParseServerCommandPacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParseSessionTickPacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParseHeartbeatPacket(const kx::PacketInfo& packet);
    std::optional<std::string> ParseInteractionResponsePacket(const kx::PacketInfo& packet);

    // Central dispatcher function
    std::optional<std::string> GetParsedDataTooltipString(const kx::PacketInfo& packet) {
        // Dispatch based on packet header ID and direction
        switch (packet.direction) {
        case kx::PacketDirection::Sent:
            switch (packet.rawHeaderId) {
            case static_cast<uint16_t>(kx::CMSG_HeaderId::SESSION_TICK):
                return ParseSessionTickPacket(packet);
            case static_cast<uint16_t>(kx::CMSG_HeaderId::PERFORMANCE_RESPONSE):
                return ParsePerformanceResponsePacket(packet);
            case static_cast<uint16_t>(kx::CMSG_HeaderId::HEARTBEAT):
                return ParseHeartbeatPacket(packet);
            case static_cast<uint16_t>(kx::CMSG_HeaderId::MOVEMENT):
                return ParseMovementPacket(packet);
            case static_cast<uint16_t>(kx::CMSG_HeaderId::LOGOUT_TO_CHAR_SELECT):
                return ParseLogoutPacket(packet);
            case static_cast<uint16_t>(kx::CMSG_HeaderId::DESELECT_AGENT):
                return ParseDeselectAgentPacket(packet);
            case static_cast<uint16_t>(kx::CMSG_HeaderId::SELECT_AGENT):
                return ParseSelectAgentPacket(packet);
            case static_cast<uint16_t>(kx::CMSG_HeaderId::INTERACT_WITH_AGENT):
                return ParseInteractWithAgentPacket(packet);
            case static_cast<uint16_t>(kx::CMSG_HeaderId::INTERACTION_RESPONSE):
                return ParseInteractionResponsePacket(packet);
            default:
                return std::nullopt; // No parser for this CMSG packet
            }
            break;

        case kx::PacketDirection::Received:
            switch (packet.rawHeaderId) {
            case static_cast<uint16_t>(kx::SMSG_HeaderId::PLAYER_STATE_UPDATE):
                return ParsePlayerStateUpdatePacket(packet);
            case static_cast<uint16_t>(kx::SMSG_HeaderId::TIME_SYNC):
                return ParseTimeSyncPacket(packet);
            case static_cast<uint16_t>(kx::SMSG_HeaderId::SERVER_COMMAND):
                return ParseServerCommandPacket(packet);
            default:
                return std::nullopt; // No parser for this SMSG packet
            }
            break;

        default:
            return std::nullopt; // Should not happen
        }
    }

    // Specific parser for Movement packets, returns formatted string
    std::optional<std::string> ParseMovementPacket(const kx::PacketInfo& packet) {
        // Check if it's a known CMSG movement packet ID
        if (packet.direction != kx::PacketDirection::Sent ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::MOVEMENT)) {
            return std::nullopt; // Not the packet type we're looking for
        }

        const std::vector<uint8_t>& data = packet.data;
        constexpr size_t assumed_offset_from_end = 16; // Based on documentation

        // Check if packet is large enough based on the assumed offset from the end
        if (data.size() < assumed_offset_from_end) {
            return std::nullopt;
        }

        kx::Packets::MovementPayload payload;
        const uint8_t* floatStart = data.data() + data.size() - assumed_offset_from_end;

        // Safely copy the float data
        std::memcpy(&payload.x, floatStart, sizeof(float));
        std::memcpy(&payload.y, floatStart + sizeof(float), sizeof(float));
        std::memcpy(&payload.z, floatStart + 2 * sizeof(float), sizeof(float));

        // Format the payload into a string
        std::stringstream ss;
        ss << "Movement Payload:\n"
           << "  X: " << std::fixed << std::setprecision(2) << payload.x << "\n"
           << "  Y: " << std::fixed << std::setprecision(2) << payload.y << "\n"
           << "  Z: " << std::fixed << std::setprecision(2) << payload.z;

        return ss.str();
    }

    // Specific parser for SMSG_PLAYER_STATE_UPDATE packets
    std::optional<std::string> ParsePlayerStateUpdatePacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Received ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::SMSG_HeaderId::PLAYER_STATE_UPDATE)) {
            return std::nullopt;
        }

        const std::vector<uint8_t>& data = packet.data;
        constexpr size_t required_size = sizeof(kx::Packets::SMSG_PlayerStateUpdatePayload);

        if (data.size() < required_size) {
            return std::nullopt;
        }

        kx::Packets::SMSG_PlayerStateUpdatePayload payload;
        std::memcpy(&payload, data.data(), required_size);

        std::stringstream ss;
        ss << "Player State Update Payload:\n"
           << "  Hdr: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.hdr << std::dec << "\n"
           << "  Mode/Ix: 0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(payload.mode_or_ix) << std::dec << "\n"
           << "  Tick Lo: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.tick_lo << std::dec << "\n"
           << "  Millis/K: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.millis_or_k << std::dec << "\n"
           << "  World/ID: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.world_or_id << std::dec << "\n"
           << "  Flags: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.flags << std::dec;

        return ss.str();
    }

    // Specific parser for SMSG_TIME_SYNC packets
    std::optional<std::string> ParseTimeSyncPacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Received ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::SMSG_HeaderId::TIME_SYNC)) {
            return std::nullopt;
        }

        const std::vector<uint8_t>& data = packet.data;
        constexpr size_t required_size = 10; // Both variants are 10 bytes

        if (data.size() < required_size) {
            return std::nullopt;
        }

        uint16_t type_discriminator;
        std::memcpy(&type_discriminator, data.data(), sizeof(uint16_t));

        std::stringstream ss;
        ss << "Time Sync Payload:\n";

        if (type_discriminator == 0x050F) { // Variant A: Cadence/Tick
            kx::Packets::SMSG_TimeSyncTickPayload payload;
            std::memcpy(&payload, data.data(), required_size);
            ss << "  Variant: Cadence/Tick\n"
               << "  Type: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.type << std::dec << "\n"
               << "  Time Lo: 0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << payload.time_lo << std::dec << "\n"
               << "  Time Hi: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.time_hi << std::dec << "\n"
               << "  Flags/ID: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.flags_or_id << std::dec;
        } else if (type_discriminator == 0x050D) { // Variant B: Seed/Epoch
            kx::Packets::SMSG_TimeSyncSeedPayload payload;
            std::memcpy(&payload, data.data(), required_size);
            ss << "  Variant: Seed/Epoch\n"
               << "  Type: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.type << std::dec << "\n"
               << "  Seed: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.seed << std::dec << "\n"
               << "  Millis: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.millis << std::dec << "\n"
               << "  World/ID: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.world_or_id << std::dec << "\n"
               << "  Flags: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.flags << std::dec;
        } else {
            ss << "  Unknown Time Sync Variant (Type: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << type_discriminator << std::dec << ")";
        }

        return ss.str();
    }

    std::optional<std::string> ParsePerformanceResponsePacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Sent ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::PERFORMANCE_RESPONSE)) {
            return std::nullopt;
        }
        const auto& data = packet.data;
        if (data.size() == 3) {
            return "Performance Response (Heartbeat Variant)";
        }
        if (data.size() >= 6) {
            uint32_t perf_value;
            std::memcpy(&perf_value, data.data() + 2, sizeof(uint32_t));
            std::stringstream ss;
            ss << "Performance Response Payload:\n"
               << "  Perf Value: " << perf_value;
            return ss.str();
        }
        return "Performance Response (Unknown Variant)";
    }

    std::optional<std::string> ParseDeselectAgentPacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Sent ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::DESELECT_AGENT)) {
            return std::nullopt;
        }
        if (packet.data.size() == 3 && packet.data[2] == 0x00) {
            return "Deselect Agent: OK";
        }
        return "Deselect Agent: (Malformed)";
    }

    std::optional<std::string> ParseLogoutPacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Sent ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::LOGOUT_TO_CHAR_SELECT)) {
            return std::nullopt;
        }
        if (packet.data.size() == 2) {
            return "Logout to Character Select";
        }
        return "Logout to Character Select: (Malformed)";
    }

    std::optional<std::string> ParseSelectAgentPacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Sent ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::SELECT_AGENT)) {
            return std::nullopt;
        }

        const std::vector<uint8_t>& data = packet.data;
        // Opcode (2) + AgentID (2) + Unknown (2) + AgentID (2)
        constexpr size_t required_size = 8;

        if (data.size() < required_size) {
            return std::nullopt;
        }

        uint16_t agentId, unknown, agentId_repeat;
        std::memcpy(&agentId, data.data() + 2, sizeof(uint16_t));
        std::memcpy(&unknown, data.data() + 4, sizeof(uint16_t));
        std::memcpy(&agentId_repeat, data.data() + 6, sizeof(uint16_t));

        std::stringstream ss;
        ss << "Select Agent Payload:\n"
           << "  Agent ID: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << agentId << std::dec;
        if (agentId != agentId_repeat) {
            ss << " (Mismatch: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << agentId_repeat << std::dec << ")";
        }
        ss << "\n  Unknown: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << unknown << std::dec;

        return ss.str();
    }

    std::optional<std::string> ParseInteractWithAgentPacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Sent ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::INTERACT_WITH_AGENT)) {
            return std::nullopt;
        }

        const std::vector<uint8_t>& data = packet.data;
        constexpr size_t required_size = 4; // Opcode (2) + CommandID (2)

        if (data.size() < required_size) {
            return std::nullopt;
        }

        uint16_t commandId;
        std::memcpy(&commandId, data.data() + 2, sizeof(uint16_t));

        std::string command_desc = "Unknown";
        switch (commandId) {
            case 0x0001: command_desc = "Continue/Next"; break;
            case 0x0002: command_desc = "Select Option"; break;
            case 0x0004: command_desc = "Exit Dialogue"; break;
        }

        std::stringstream ss;
        ss << "Interact With Agent Payload:\n"
           << "  Command ID: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << commandId
           << " (" << command_desc << ")";

        return ss.str();
    }

    std::optional<std::string> ParseServerCommandPacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Received ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::SMSG_HeaderId::SERVER_COMMAND)) {
            return std::nullopt;
        }

        const std::vector<uint8_t>& data = packet.data;
        if (data.size() < 2) {
            return "Server Command: (Too small)";
        }

        uint16_t subtype;
        std::memcpy(&subtype, data.data(), sizeof(uint16_t));

        std::stringstream ss;
        ss << "Server Command Payload:\n"
           << "  Subtype: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << subtype << std::dec;

        if (subtype == 0x0003) {
            ss << " (Performance Trigger)";
        } else if (subtype == 0x0004) {
            ss << " (Ping Response Trigger)";
            if (data.size() >= 6) {
                uint32_t value;
                std::memcpy(&value, data.data() + 2, sizeof(uint32_t));
                ss << "\n  Value: 0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << value << std::dec;
            }
        }

        return ss.str();
    }

    std::optional<std::string> ParseSessionTickPacket(const kx::PacketInfo& packet) {
        if (packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::SESSION_TICK) || packet.data.size() < 6) {
            return std::nullopt;
        }
        uint32_t timestamp;
        std::memcpy(&timestamp, packet.data.data() + 2, sizeof(uint32_t));
        std::stringstream ss;
        ss << "Session Tick Payload:\n"
           << "  Timestamp: " << timestamp;
        return ss.str();
    }

    std::optional<std::string> ParseInteractionResponsePacket(const kx::PacketInfo& packet) {
        if (packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::INTERACTION_RESPONSE) || packet.data.size() < 3) {
            return std::nullopt;
        }
        if (packet.data[2] == 0x01) {
            return "Interaction Response: OK";
        }
        return "Interaction Response: (Malformed)";
    }

    std::optional<std::string> ParseHeartbeatPacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Sent ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::HEARTBEAT)) {
            return std::nullopt;
        }

        if (packet.data.size() < 4) {
            return "Heartbeat: (Malformed, too small)";
        }

        uint16_t value;
        // The data vector includes the opcode, so the value is at offset 2.
        std::memcpy(&value, packet.data.data() + 2, sizeof(uint16_t));

        std::stringstream ss;
        ss << "Heartbeat Payload:\n"
           << "  Value: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << value;

        return ss.str();
    }

} // namespace kx::Parsing