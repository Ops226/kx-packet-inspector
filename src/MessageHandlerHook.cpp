#include <windows.h> // Ensure windows.h is included first for platform definitions
#include "MessageHandlerHook.h"
#include "safetyhook.hpp"    // SafetyHook library for mid-function hooking
#include "PacketProcessor.h" // For ProcessDispatchedMessage
#include "PacketData.h"      // For PacketDirection enum
#include "GameStructs.h"     // For MSGCONN_*, HANDLER_INFO_*, MSG_DEF_* constants

#include <iostream>          // For std::cerr (initialization errors)
#include <iomanip>           // Potentially for hex formatting in debug logs
#include <mutex>             // If needed for shared resources (currently only debug output)
#include <vector>            // Used by PacketInfo (via PacketProcessor)
#include <debugapi.h>        // For OutputDebugStringA (debugging)
#include <cstdio>            // For sprintf_s (formatting debug messages)
#include <exception>         // For std::exception

// Global SafetyHook object for the hook at the primary handler call site.
// SafetyHookMid is used because we need register context access (ctx).
SafetyHookMid g_handlerHook1{};

// TODO: Analyze if the other handler call sites within FUN_1412e9390
//       (e.g., offsets 0x228, 0x385, 0x3E4 relative to function start)
//       also need to be hooked. If so, declare additional SafetyHookMid objects
//       (g_handlerHook2, etc.) and initialize them in InitializeMessageHandlerHooks.
// SafetyHookMid g_handlerHook2{};

// Optional: Mutex if extensive shared state was accessed directly in the hook.
// Currently, processing is delegated, making this less critical here.
// std::mutex g_hookMutex;

/**
 * @brief Hook function executed via SafetyHook's MidHook just before the game calls
 *        a specific message handler function within the main dispatcher loop (FUN_1412e9390).
 * @details This function leverages the SafetyHook context (`ctx`) to read necessary registers (RBX, RBP)
 *          and memory locations relative to game structures (using offsets from GameStructs.h)
 *          to extract the message ID, message data pointer, and message size.
 *          It then delegates the processing and logging of this individual message to
 *          kx::PacketProcessing::ProcessDispatchedMessage.
 *          Execution automatically continues to the original hooked instruction after this function returns.
 * @param ctx SafetyHook context object providing access to the CPU register state
 *            at the time the hook was triggered (e.g., ctx.rbx, ctx.rbp).
*/
void hookHandlerCallSite(SafetyHookContext& ctx)
{
    // Optional: Uncomment for verbose hook entry debugging.
    // OutputDebugStringA("[hookHandlerCallSite] MidHook Entered.\n");

    void* pMsgConn = nullptr;
    void* messageDataPtr = nullptr;
    void* handlerInfoPtr = nullptr;
    uint16_t messageId = 0;
    uint32_t messageSize = 0;

    try {
        // Extract MsgConn pointer from RBX register (as observed in dispatcher assembly)
        pMsgConn = reinterpret_cast<void*>(ctx.rbx);

        // Extract message data pointer (local_50) which is stored on the stack relative to RBP.
        // WARNING: Stack offsets like [RBP - 0x18] can be less stable across compiler versions/updates
        // than struct offsets. If this breaks, re-analyze the assembly immediately preceding the hook site
        // to see if the data pointer (likely prepared for RDX for the call) is available in another register.
        if (ctx.rbp != 0) { // Ensure RBP seems valid before dereferencing relative to it
            messageDataPtr = *reinterpret_cast<void**>(ctx.rbp - 0x18); // Read local_50 from stack
        }
        else {
            // This should not happen in standard function execution but is a safeguard.
            OutputDebugStringA("[hookHandlerCallSite] Error: RBP register is NULL in context! Cannot get message data ptr.\n");
            return;
        }

        if (pMsgConn == nullptr) {
            // This would indicate a serious issue if RBX wasn't holding the context.
            OutputDebugStringA("[hookHandlerCallSite] Error: NULL pMsgConn extracted via RBX register.\n");
            return;
        }
        // Note: messageDataPtr *might* be null for zero-size messages. We proceed but handle it later.


        // Read the pointer to the Handler Info structure from the MsgConn context
        handlerInfoPtr = *reinterpret_cast<void**>(
            static_cast<char*>(pMsgConn) + kx::GameStructs::MSGCONN_HANDLER_INFO_PTR_OFFSET // Expected offset 0x48
            );
        if (handlerInfoPtr == nullptr) {
            // This is potentially expected if the dispatcher loop is between messages,
            // but should not happen immediately before a handler *call*. Log as warning.
            OutputDebugStringA("[hookHandlerCallSite] Warning: NULL handlerInfoPtr in MsgConn (Offset 0x48).\n");
            return;
        }

        // Read Message ID (Opcode) from the Handler Info structure
        messageId = *reinterpret_cast<uint16_t*>(
            static_cast<char*>(handlerInfoPtr) + kx::GameStructs::HANDLER_INFO_MSG_ID_OFFSET // Expected offset 0x00
            );

        // Read the pointer to the Message Definition structure from Handler Info
        void* msgDefPtr = *reinterpret_cast<void**>(
            static_cast<char*>(handlerInfoPtr) + kx::GameStructs::HANDLER_INFO_MSG_DEF_PTR_OFFSET // Expected offset 0x08
            );
        if (msgDefPtr == nullptr) {
            // If there's no definition pointer, we cannot determine the size reliably.
            OutputDebugStringA("[hookHandlerCallSite] Warning: NULL msgDefPtr in Handler Info. Assuming size 0.\n");
            messageSize = 0; // Assume zero size for safety
        }
        else {
            // Read Message Size from the Message Definition structure
            messageSize = *reinterpret_cast<uint32_t*>(
                static_cast<char*>(msgDefPtr) + kx::GameStructs::MSG_DEF_SIZE_OFFSET // Expected offset 0x20
                );
        }

        // Optional: Debug log extracted data before processing
        // char buffer[256];
        // sprintf_s(buffer, sizeof(buffer), "[hookHandlerCallSite] MIDHOOK: MsgID=0x%04x, Size=%u, DataPtr=%p, MsgConn=%p\n",
        //           messageId, messageSize, messageDataPtr, pMsgConn);
        // OutputDebugStringA(buffer);


        // Pass the extracted information to the dedicated processing function.
        kx::PacketProcessing::ProcessDispatchedMessage(
            kx::PacketDirection::Received, // This hook handles received packets
            messageId,
            static_cast<const uint8_t*>(messageDataPtr),
            messageSize,
            pMsgConn                                     // Pass context for potential future analysis
        );

    }
    catch (const std::exception& e) { // Catch standard C++ exceptions (e.g., bad_alloc if logging fails)
        char buffer[256];
        sprintf_s(buffer, sizeof(buffer), "[hookHandlerCallSite] EXCEPTION: %s\n", e.what());
        OutputDebugStringA(buffer);
    }
    catch (...) { // Catch potential Windows SEH exceptions (e.g., memory access violations)
        OutputDebugStringA("[hookHandlerCallSite] UNKNOWN EXCEPTION (Potential Access Violation reading state).\n");
    }

    // SafetyHook ensures execution continues at the original instruction after this function returns.
}


/**
 * @brief Initializes the SafetyHook MidHook at the identified message handler call site(s).
 * @param dispatcherFuncAddress The runtime base address of the dispatcher function (FUN_1412e9390).
 * @return true on success, false on failure.
 */
bool InitializeMessageHandlerHooks(uintptr_t dispatcherFuncAddress) {
    if (dispatcherFuncAddress == 0) {
        std::cerr << "[MessageHandlerHook] Error: Initialize called with null dispatcher address." << std::endl;
        return false;
    }

    // Calculate the absolute address for the hook.
    // This offset targets the `MOV RDX,[RBP-18]` instruction immediately preceding
    // the first type of handler call (`CALL RAX` at 0x1412e95ad).
    constexpr ptrdiff_t HOOK_OFFSET_1 = 0x219;
    uintptr_t hookSite1 = dispatcherFuncAddress + HOOK_OFFSET_1;

    std::cout << "[MessageHandlerHook] Attempting to install MidHook at primary call site: 0x" << std::hex << hookSite1 << std::dec << std::endl;

    // Use SafetyHook's MidHook factory function
    auto builder = safetyhook::MidHook::create(
        reinterpret_cast<void*>(hookSite1),
        hookHandlerCallSite
    );

    if (!builder) {
        std::cerr << "[MessageHandlerHook] Failed to create SafetyHook MidHook builder (check address/permissions?)." << std::endl;
        return false;
    }

    // Move the created hook into the global variable for management
    // This activates the hook.
    g_handlerHook1 = std::move(*builder);

    if (!g_handlerHook1) {
        std::cerr << "[MessageHandlerHook] Failed to finalize SafetyHook MidHook object after move." << std::endl;
        // Builder might have failed post-creation checks or during move construction.
        return false;
    }

    // TODO: If analysis reveals messages are missed, investigate other call sites
    // (e.g., dispatcherFuncAddress + 0x228) and install additional hooks (g_handlerHook2, etc.)
    // using the same pattern: safetyhook::MidHook::create(...), std::move(...)

    std::cout << "[MessageHandlerHook] MidHook installed successfully at primary site." << std::endl;
    return true;
}

/**
 * @brief Cleans up and removes the installed SafetyHook MidHook(s).
 * @details Assigning an empty SafetyHookMid object triggers the destructor,
 *          which automatically uninstalls the hook.
 */
void CleanupMessageHandlerHooks() {
    // Destroy the hook object to uninstall the hook automatically via RAII.
    if (g_handlerHook1) {
        g_handlerHook1 = {}; // Assign empty object, triggers destructor of old object
        std::cout << "[MessageHandlerHook] Hook 1 cleaned up." << std::endl;
    }
    // Add similar cleanup for g_handlerHook2 etc. if implemented
}