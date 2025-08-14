#include "PacketParser.h"
#include "PacketHeaders.h"
#include "parsers/ParseDeselectAgentPacket.h"
#include "parsers/ParseHeartbeatPacket.h"
#include "parsers/ParseInteractWithAgentPacket.h"
#include "parsers/ParseInteractionResponsePacket.h"
#include "parsers/ParseLogoutPacket.h"
#include "parsers/ParseMovementPacket.h"
#include "parsers/ParsePerformanceResponsePacket.h"
#include "parsers/ParsePlayerStateUpdatePacket.h"
#include "parsers/ParseSelectAgentPacket.h"
#include "parsers/ParseServerCommandPacket.h"
#include "parsers/ParseSessionTickPacket.h"
#include "parsers/ParseTimeSyncPacket.h"

namespace kx::Parsing {

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

} // namespace kx::Parsing


