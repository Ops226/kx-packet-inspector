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
            case static_cast<uint8_t>(kx::CMSG_HeaderId::MOVEMENT):
                return ParseMovementPacket(packet);
                // Add cases for other known CMSG packets here
                // case static_cast<uint8_t>(kx::CMSG_HeaderId::SOME_OTHER_PACKET):
                //     return ParseSomeOtherPacket(packet);
            default:
                return std::nullopt; // No parser for this CMSG packet
            }
            break;

        case kx::PacketDirection::Received:
            switch (packet.rawHeaderId) {
                // Add cases for known SMSG packets here
                // case static_cast<uint8_t>(kx::SMSG_HeaderId::SOME_SMSG_PACKET):
                //     return ParseSomeSmsgPacket(packet);
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
            packet.rawHeaderId != static_cast<uint8_t>(kx::CMSG_HeaderId::MOVEMENT)) {
            return std::nullopt; // Not the packet type we're looking for
        }

        const std::vector<uint8_t>& data = packet.data;
        constexpr size_t required_size = sizeof(kx::Packets::MovementPayload);
        constexpr size_t assumed_offset_from_end = 16;

        // Check if packet is large enough based on the assumed offset from the end
        if (data.size() < assumed_offset_from_end) {
             // Consider logging this size mismatch?
            return std::nullopt;
        }

        kx::Packets::MovementPayload payload;
        const uint8_t* dataPtr = data.data();
        size_t size = data.size();
        const uint8_t* floatStart = dataPtr + size - assumed_offset_from_end; // Calculate start based on end

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

    // Implementations for other specific packet parsers go here...

} // namespace kx::Parsing