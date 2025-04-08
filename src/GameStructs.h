#pragma once

#include <cstddef> // For std::byte, std::size_t
#include <cstdint> // For std::uint8_t, std::int32_t
#include <type_traits> // For offsetof (used in static_assert)
#include <array>   // For std::array

namespace kx::GameStructs {

    /**
     * @brief Represents the memory layout of the context object passed to the
     *        game's internal message sending function (identified as FUN_1412e9960).
     * @details This structure defines members based on reverse engineering the function,
     *          specifically how it accesses data relative to its 'param_1' argument.
     *          It allows accessing packet data before potential encryption/final processing.
     * @warning This structure definition is based on analysis of specific versions
     *          of the target game (Gw2-64.exe). While it may remain stable across
     *          some game updates, patches that significantly alter internal data
     *          layouts **could** require this definition to be updated to ensure
     *          correct functionality. Community contributions to verify and update
     *          this definition are appreciated.
     */
    struct MsgSendContext {
        // --- Member offsets relative to the start of the structure (param_1) ---

        // Define padding explicitly for clarity on known/unknown regions.
        std::byte padding_0x0[0xD0];

        /**
         * @brief 0xD0: Pointer to the current write position (end) within the packet data buffer.
         * @details This pointer indicates how much data has been written to the buffer located
         *          at PACKET_BUFFER_OFFSET relative to 'this'. The size of the current packet
         *          data is calculated as (currentBufferEndPtr - GetPacketBufferStart()).
         *          The function modifies this pointer, notably resetting it when bufferState == 1
         *          or after processing the buffer in the encryption path.
         */
        std::uint8_t* currentBufferEndPtr; // Corresponds to *(param_1 + 0xd0)

        // Padding between known members.
        std::byte padding_0xD8[0x108 - 0xD8];

        /**
         * @brief 0x108: State variable influencing control flow within the send function.
         * @details Observed values in decompilation (e.g., 1, 3) determine the processing path.
         *          State '1' appears to skip the main processing/encryption and resets
         *          currentBufferEndPtr, suggesting the buffer should not be read in this state.
         *          State '3' is associated with the path that includes encryption calls.
         *          (See the 'if (*(int *)(param_1 + 0x108) == 1)' branch in FUN_1412e9960).
         */
        std::int32_t bufferState; // Corresponds to *(param_1 + 0x108)

        // --- Constants Related to the Context ---

        /**
         * @brief The fixed memory offset from the base address of the MsgSendContext instance
         *        to the start of the actual raw packet data buffer.
         * @details This buffer itself is not directly part of the C++ struct layout.
         */
        static constexpr std::size_t PACKET_BUFFER_OFFSET = 0x398;

        // --- Helper Methods ---

        /**
         * @brief Calculates the pointer to the start of the raw packet data buffer.
         * @return Pointer to the beginning of the packet data.
         */
        std::uint8_t* GetPacketBufferStart() const noexcept {
            // Use C-style cast for pointer arithmetic from 'this'.
            return reinterpret_cast<std::uint8_t*>(
                const_cast<MsgSendContext*>(this)
                ) + PACKET_BUFFER_OFFSET;
        }

        /**
         * @brief Calculates the size of the packet data currently present in the buffer.
         * @return The size in bytes, or 0 if pointers are invalid or end < start.
         */
        std::size_t GetCurrentDataSize() const noexcept {
            const std::uint8_t* bufferStart = GetPacketBufferStart();
            // Basic validation: ensure end pointer is valid and after start pointer.
            if (currentBufferEndPtr == nullptr || currentBufferEndPtr < bufferStart) {
                return 0;
            }
            return static_cast<std::size_t>(currentBufferEndPtr - bufferStart);
        }
    };

    // Static assertions to verify expected offsets at compile time.
    // Helps catch errors if the struct definition or compiler padding changes unexpectedly.
    static_assert(offsetof(MsgSendContext, currentBufferEndPtr) == 0xD0, "Offset mismatch for MsgSendContext::currentBufferEndPtr");
    static_assert(offsetof(MsgSendContext, bufferState) == 0x108, "Offset mismatch for MsgSendContext::bufferState");



    // --- MsgConn Context Offsets (Receive/Dispatch Functions) ---
    // These offsets are relative to the 'pMsgConn' pointer (param_2 / RDX) passed to
    // the dispatcher FUN_1412e9390 and the original receiver FUN_1412ea0c0.

    /**
     * @brief Offset to the last processed message ID (Opcode). uint32_t.
     * @details Updated during the dispatch loop FUN_1412e9390. Read for context/debugging.
     */
    inline constexpr std::ptrdiff_t MSGCONN_LAST_MSG_ID_OFFSET = 0x40;

    /**
     * @brief Offset to the pointer holding the current/next message handler info structure. void**.
     * @details This is populated by the handler lookup (FUN_1412e81c0) inside the
     *          dispatcher loop (FUN_1412e9390) and cleared before the dispatcher returns.
     *          Reading this pointer is key to getting message size and handler function.
     */
    inline constexpr std::ptrdiff_t MSGCONN_HANDLER_INFO_PTR_OFFSET = 0x48;

    /**
     * @brief Offset to the base pointer of the internal ring buffer used for received data. byte**.
     * @details Used in conjunction with read/write indices.
     */
    inline constexpr std::ptrdiff_t MSGCONN_BUFFER_BASE_PTR_OFFSET = 0x88;

    /**
     * @brief Offset to the current read index within the internal ring buffer. uint32_t.
     * @details Indicates the position from where the next message data will be read.
     *          Updated by the dispatcher loop.
     */
    inline constexpr std::ptrdiff_t MSGCONN_BUFFER_READ_IDX_OFFSET = 0x94;

    /**
     * @brief Offset to the current write index within the internal ring buffer. uint32_t.
     * @details Indicates the position where new data was last written (or end of current data).
     */
    inline constexpr std::ptrdiff_t MSGCONN_BUFFER_WRITE_IDX_OFFSET = 0xA0;

    /**
     * @brief Offset to the pointer to the base of the fully processed (decrypted, decompressed)
     *        data buffer that the dispatcher (FUN_1412e9390) iterates over. void**.
     * @details The function FUN_1412ed640 likely calculates message data pointers relative to this base.
     */
    inline constexpr std::ptrdiff_t MSGCONN_PROCESSED_BUFFER_BASE_OFFSET = 0xC8; // 200 decimal


    // --- Message Handler Info Structure Offsets ---
    // These offsets are relative to the 'handlerInfoPtr' obtained from
    // MSGCONN_HANDLER_INFO_PTR_OFFSET within the MsgConn context.
    // This structure appears to be 32 bytes (0x20) long based on FUN_1412e81c0.

    /**
     * @brief Offset within the Handler Info struct to the Message ID (Opcode). uint16_t or uint32_t.
     * @details Identifies the type of the message this handler corresponds to.
     *          Assuming uint16_t based on typical GW2 opcodes.
     */
    inline constexpr std::ptrdiff_t HANDLER_INFO_MSG_ID_OFFSET = 0x00;

    /**
     * @brief Offset within the Handler Info struct to the Message Definition Pointer. void**.
     * @details Points to another structure containing details about the message, including its size.
     */
    inline constexpr std::ptrdiff_t HANDLER_INFO_MSG_DEF_PTR_OFFSET = 0x08;

    /**
     * @brief Offset within the Handler Info struct to the Dispatch Type field. int32_t.
     * @details Used in a switch statement within the dispatcher (FUN_1412e9390)
     *          to determine exactly how the handler function is called. (e.g., 0 or 1 observed).
     */
    inline constexpr std::ptrdiff_t HANDLER_INFO_DISPATCH_TYPE_OFFSET = 0x10;

    /**
     * @brief Offset within the Handler Info struct to the Handler Function Pointer. void**.
     * @details Pointer to the actual C++ function that processes this specific message type.
     *          This is the function executed by the `call` instruction in the dispatcher.
     */
    inline constexpr std::ptrdiff_t HANDLER_INFO_HANDLER_FUNC_PTR_OFFSET = 0x18;


    // --- Message Definition Structure Offsets ---
    // These offsets are relative to the 'msgDefPtr' obtained from
    // HANDLER_INFO_MSG_DEF_PTR_OFFSET within the Handler Info structure.

    /**
     * @brief Offset within the Message Definition struct to the size of the message payload. uint32_t.
     * @details Specifies how many bytes constitute the data payload for this message type,
     *          excluding any potential header bytes read earlier by the dispatcher.
     */
    inline constexpr std::ptrdiff_t MSG_DEF_SIZE_OFFSET = 0x20;
}