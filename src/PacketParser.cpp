#include "PacketParser.h"
#include "PacketHeaders.h" // For CMSG_HeaderId enum
#include <cstring> // For memcpy
#include <stdexcept>
#include <sstream> // For formatting the string
#include <iomanip> // For std::fixed, std::setprecision

namespace kx::Parsing {

    // Central dispatcher function
    std::optional<std::string> GetParsedDataTooltipString(const kx::PacketInfo& packet) {
        // Dispatch based on packet header ID and direction
        switch (packet.direction) {
        case kx::PacketDirection::Sent:
            switch (packet.rawHeaderId) {
            case static_cast<uint16_t>(kx::CMSG_HeaderId::MOVEMENT):
                return ParseMovementPacket(packet);
            case static_cast<uint16_t>(kx::CMSG_HeaderId::AGENT_LINK):
                return ParseAgentLinkPacket(packet);
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

    // Specific parser for CMSG_AGENT_LINK packets
    std::optional<std::string> ParseAgentLinkPacket(const kx::PacketInfo& packet) {
        if (packet.direction != kx::PacketDirection::Sent ||
            packet.rawHeaderId != static_cast<uint16_t>(kx::CMSG_HeaderId::AGENT_LINK)) {
            return std::nullopt;
        }

        const std::vector<uint8_t>& data = packet.data;
        constexpr size_t required_size = sizeof(kx::Packets::CMSG_AgentLinkPayload);

        if (data.size() < required_size) {
            return std::nullopt;
        }

        kx::Packets::CMSG_AgentLinkPayload payload;
        std::memcpy(&payload, data.data(), required_size);

        std::stringstream ss;
        ss << "Agent Link Payload:\n"
           << "  Agent ID: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.agentId << std::dec << "\n"
           << "  Parent ID: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.parentId << std::dec << "\n"
           << "  Flags: 0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << payload.flags << std::dec;

        return ss.str();
    }

} // namespace kx::Parsing
