#pragma once

#include <string_view> // For std::string_view

namespace kx {
    constexpr std::string_view APP_VERSION = "1.5";

    // Configuration for the target process and function signature
    constexpr std::string_view TARGET_PROCESS_NAME = "Gw2-64.exe";
    constexpr std::string_view MSG_CONN_FLUSH_PACKET_BUFFER_PATTERN = "40 ? 48 83 EC ? 48 8D ? ? ? 48 89 ? ? 48 89 ? ? 48 89 ? ? 4C 89 ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 48 8B ? E8";
    constexpr std::string_view MSG_DISPATCH_STREAM_PATTERN = "48 89 5C 24 ? 4C 89 44 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 83 EC ? 8B 82";
}
