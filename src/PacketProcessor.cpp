#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // Include windows.h early to establish platform definitions

// Now include your project headers and standard library headers
#include "PacketProcessor.h"
#include "PacketData.h"
#include "AppState.h"
#include "PacketHeaders.h"
#include "CryptoUtils.h"
#include "GameStructs.h" // Included via PacketProcessor.h but good practice

#include <vector>
#include <chrono>
#include <mutex>
#include <limits>
#include <cstring> // For memcpy

// Anonymous namespace for internal helper functions
namespace {

/**
 * @brief Attempts to decrypt the packet data if RC4 encryption is indicated and possible.
 * @param info The PacketInfo object, potentially modified with decryptedData or error state.
 */
void AttemptDecryption(kx::PacketInfo& info) {
    // Attempt decryption only if state indicates RC4, we have state, and data exists
    if (info.bufferState == 3 && info.rc4State.has_value() && !info.data.empty()) {
        try {
            info.decryptedData = kx::Crypto::rc4_process_copy(info.rc4State.value(), info.data);
            // Decryption call succeeded if no exception was thrown
        } catch (const std::exception& e) {
            char msg[256];
            sprintf_s(msg, sizeof(msg), "[PacketProcessor] Exception during RC4 decryption: %s\n", e.what());
            OutputDebugStringA(msg);
            info.specialType = kx::InternalPacketType::PROCESSING_ERROR; // Mark decryption failure
            info.name = kx::GetSpecialPacketTypeName(info.specialType);
        } catch (...) {
            OutputDebugStringA("[PacketProcessor] Unknown exception during RC4 decryption.\n");
            info.specialType = kx::InternalPacketType::PROCESSING_ERROR; // Mark decryption failure
            info.name = kx::GetSpecialPacketTypeName(info.specialType);
        }
    }
}

} // anonymous namespace

namespace kx::PacketProcessing {

    void ProcessOutgoingPacket(const GameStructs::MsgSendContext* context) {
        // Basic check (hook should ideally ensure non-null, but double-check)
        if (!context) {
            // Log::Error("ProcessOutgoingPacket called with null context."); // Future logger
            OutputDebugStringA("[PacketProcessor] Error: ProcessOutgoingPacket called with null context.\n");
            return;
        }

        try {
            // Skip processing if bufferState indicates the buffer might be invalid or getting reset (state 1).
            if (context->bufferState == 1) {
                return;
            }

            std::uint8_t* packetData = context->GetPacketBufferStart();
            std::size_t bufferSize = context->GetCurrentDataSize();

            // --- Sanity Checks ---
            bool dataIsValid = true;
            if (packetData == nullptr || context->currentBufferEndPtr == nullptr) {
                // Log::Error("Null pointer in MsgSendContext detected during processing.");
                OutputDebugStringA("[PacketProcessor] Error: Null pointer in MsgSendContext detected during processing.\n");
                dataIsValid = false;
            }
            else if (context->currentBufferEndPtr < packetData) {
                // Log::Error("Invalid packet buffer pointers (end < start) in MsgSendContext.");
                OutputDebugStringA("[PacketProcessor] Error: Invalid packet buffer pointers (end < start) in MsgSendContext.\n");
                dataIsValid = false;
            }
            else {
                // Limit size to prevent reading excessive memory.
                constexpr std::size_t MAX_REASONABLE_PACKET_SIZE = 16 * 1024;
                if (bufferSize > MAX_REASONABLE_PACKET_SIZE) {
                    // Log::Error("Outgoing packet size (%zu) exceeds sanity limit (%zu).", bufferSize, MAX_REASONABLE_PACKET_SIZE);
                    char msg[128];
                    sprintf_s(msg, sizeof(msg), "[PacketProcessor] Error: Outgoing packet size (%zu) exceeds sanity limit.\n", bufferSize);
                    OutputDebugStringA(msg);
                    dataIsValid = false;
                }
                // Check against the destination integer type limit.
                else if (bufferSize > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
                    // Log::Error("Outgoing packet size (%zu) exceeds std::numeric_limits<int>::max().", bufferSize);
                    char msg[128];
                    sprintf_s(msg, sizeof(msg), "[PacketProcessor] Error: Outgoing packet size (%zu) exceeds max int.\n", bufferSize);
                    OutputDebugStringA(msg);
                    dataIsValid = false;
                }
            }
            // --- End Sanity Checks ---

            if (dataIsValid && bufferSize > 0) {
                PacketInfo info;
                info.timestamp = std::chrono::system_clock::now();
                info.size = static_cast<int>(bufferSize);
                info.direction = PacketDirection::Sent;
                info.bufferState = context->bufferState;
                info.specialType = InternalPacketType::NORMAL; // Assume normal

                // Copy packet data
                info.data.assign(packetData, packetData + bufferSize);

                // Analyze header
                if (info.data.size() >= 2) {
                    memcpy(&info.rawHeaderId, info.data.data(), sizeof(info.rawHeaderId));
                    info.name = GetPacketName(info.direction, info.rawHeaderId); // Use directional lookup
                } else {
                    info.specialType = InternalPacketType::PACKET_TOO_SMALL;
                    info.rawHeaderId = 0; // Assign a default/sentinel value
                    info.name = GetSpecialPacketTypeName(info.specialType);
                }

                // Check if the resolved name indicates an unknown header ID
                if (info.name.find("_UNKNOWN") != std::string::npos) {
                    info.specialType = InternalPacketType::UNKNOWN_HEADER; // Mark specifically as unknown ID
                }

                // Log the packet
                {
                    std::lock_guard<std::mutex> lock(g_packetLogMutex);
                    g_packetLog.push_back(std::move(info));
                }
            }
            else if (dataIsValid && bufferSize == 0) {
                PacketInfo info;
                info.timestamp = std::chrono::system_clock::now();
                info.size = 0;
                info.direction = PacketDirection::Sent;
                info.bufferState = context->bufferState;
                info.specialType = InternalPacketType::EMPTY_PACKET; // Mark as empty
                info.name = GetSpecialPacketTypeName(info.specialType);
                info.rawHeaderId = 0; // Or some default
                // Log the empty packet info
                {
                    std::lock_guard<std::mutex> lock(g_packetLogMutex);
                    g_packetLog.push_back(std::move(info));
                }
            }
        }
        catch (const std::exception& e) {
            char msg[256];
            sprintf_s(msg, sizeof(msg), "[PacketProcessor] Outgoing packet processing exception: %s\n", e.what());
            OutputDebugStringA(msg);
        }
        catch (...) {
            OutputDebugStringA("[PacketProcessor] Unknown exception during outgoing packet processing.\n");
        }
    }

    void ProcessIncomingPacket(int currentState,
        const std::uint8_t* buffer,
        std::size_t size,
        const std::optional<GameStructs::RC4State>& capturedRc4State)
    {
        // Basic checks: Only return if buffer is null. Allow zero-size packets otherwise.
        if (buffer == nullptr && size == 0) {
             return;
        }

        // Check size limits (similar to outgoing)
        if (size > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
            char msg[128];
            sprintf_s(msg, sizeof(msg), "[PacketProcessor] Error: Incoming packet size (%zu) exceeds max int.\n", size);
            OutputDebugStringA(msg);
            return; // Don't process excessively large packets
        }
        constexpr std::size_t MAX_REASONABLE_PACKET_SIZE = 16 * 1024; // Reuse or define separately
        if (size > MAX_REASONABLE_PACKET_SIZE) {
            char msg[128];
            sprintf_s(msg, sizeof(msg), "[PacketProcessor] Error: Incoming packet size (%zu) exceeds sanity limit.\n", size);
            OutputDebugStringA(msg);
            return;
        }

        try {
            PacketInfo info;
            info.timestamp = std::chrono::system_clock::now();
            info.size = static_cast<int>(size);
            info.direction = PacketDirection::Received;
            info.bufferState = currentState;
            info.rc4State = capturedRc4State; // Store captured state
            info.specialType = InternalPacketType::NORMAL; // Assume normal initially
            info.rawHeaderId = 0; // Default

            // Copy original data (only if size > 0 and buffer is valid)
            if (size > 0 && buffer != nullptr) {
                info.data.assign(buffer, buffer + size);
            } else {
                // Ensure data vector is empty if size is 0 or buffer is null
                info.data.clear();
            }

            // Step 1: Attempt Decryption (modifies info if applicable)
            AttemptDecryption(info);

            // Step 2: Analyze Packet Content (sets final type and name)
            // This logic runs only if AttemptDecryption didn't set PROCESSING_ERROR
            if (info.specialType != InternalPacketType::PROCESSING_ERROR) {
                const std::vector<uint8_t>& dataToAnalyze = info.GetDisplayData();

                // Case 1: State was 3 (RC4 expected) but we don't have decrypted data
                if (info.bufferState == 3 && !info.decryptedData.has_value()) {
                    info.specialType = InternalPacketType::ENCRYPTED_RC4;
                    // Provide more specific names based on why decryption didn't happen
                    if (!info.rc4State.has_value()) {
                        info.name = "Encrypted (RC4 State Read Fail)";
                    } else if (info.data.empty()) { // Check original data empty
                        info.name = "Encrypted (Empty RC4 Packet)";
                    } else {
                        // Decryption attempted but failed (exception caught in AttemptDecryption sets PROCESSING_ERROR)
                        // or state wasn't captured correctly. If we reach here without PROCESSING_ERROR,
                        // it implies a logic gap or unexpected state. Use generic name.
                        info.name = GetSpecialPacketTypeName(info.specialType); // Generic "ENCRYPTED_RC4"
                    }
                }
                // Case 2: The data buffer to analyze is empty
                else if (dataToAnalyze.empty()) {
                    // This covers originally empty packets and packets that decrypt to empty
                    info.specialType = InternalPacketType::EMPTY_PACKET;
                    info.name = GetSpecialPacketTypeName(info.specialType);
                    info.rawHeaderId = 0; // Ensure header is 0 for empty packets
                }
                // Case 3: Normal processing (plaintext or successfully decrypted, and not empty)
                else {
                    if (dataToAnalyze.size() >= 2) { // Check size for 2-byte header
                        info.specialType = InternalPacketType::NORMAL; // Assume normal first
                        memcpy(&info.rawHeaderId, dataToAnalyze.data(), sizeof(info.rawHeaderId));
                        info.name = GetPacketName(info.direction, info.rawHeaderId);

                        // Refine type *only if* the header ID is unknown after lookup
                        if (info.name.find("_UNKNOWN") != std::string::npos) {
                            info.specialType = InternalPacketType::UNKNOWN_HEADER;
                        }
                    } else { // Packet too small for 2-byte header
                        info.specialType = InternalPacketType::PACKET_TOO_SMALL;
                        info.rawHeaderId = 0; // Assign a default/sentinel value
                        info.name = GetSpecialPacketTypeName(info.specialType);
                    }
                }
            }

            // Step 3: Log the packet info
            {
                std::lock_guard<std::mutex> lock(g_packetLogMutex);
                g_packetLog.push_back(std::move(info));
            }
        }
        catch (const std::exception& e) {
            char msg[256];
            sprintf_s(msg, sizeof(msg), "[PacketProcessor] Incoming packet processing exception: %s\n", e.what());
            OutputDebugStringA(msg);
        }
        catch (...) {
            OutputDebugStringA("[PacketProcessor] Unknown exception during incoming packet processing.\n");
        }
    }

} // namespace kx::PacketProcessing