#include "ImGuiManager.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "PacketData.h" // Include for PacketInfo, g_packetLog, g_packetLogMutex
#include "AppState.h"   // Include for UI state, filter state, hook status
#include "GuiStyle.h"  // Include for custom styling functions
#include "FormattingUtils.h"
#include "FilterUtils.h"
#include "PacketHeaders.h" // Need this for iterating known headers
#include "Config.h"

#include <vector>
#include <mutex>
#include <deque>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>   // For formatting time
#include <string>
#include <map>     // For std::map used in filtering
#include <windows.h> // Required for ShellExecuteA
#include <algorithm> // For std::sort if needed later

bool ImGuiManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

    GUIStyle::LoadAppFont();
    GUIStyle::ApplyCustomStyle();

    if (!ImGui_ImplWin32_Init(hwnd)) return false;
    if (!ImGui_ImplDX11_Init(device, context)) return false;

    return true;
}

void ImGuiManager::NewFrame() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render(ID3D11DeviceContext* context, ID3D11RenderTargetView* mainRenderTargetView) {
    ImGui::EndFrame();
    ImGui::Render();
    context->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


// --- Helper Functions for Rendering UI Sections ---

void ImGuiManager::RenderHints() {
    // Hints moved to top
    ImGui::TextDisabled("Hint: Press INSERT to show/hide window.");
    ImGui::TextDisabled("Hint: Press DELETE to unload DLL.");
    ImGui::Separator();
}

void ImGuiManager::RenderInfoSection() {
    // --- Info Section ---
    if (ImGui::CollapsingHeader("Info")) {
        ImGui::Text("KX Packet Inspector by Krixx");
        ImGui::Text("Visit kxtools.xyz for more tools!");
        ImGui::Separator();

        // GitHub Link
        ImGui::Text("GitHub:");
        ImGui::SameLine();
        if (ImGui::Button("Repository")) {
            ShellExecuteA(NULL, "open", "https://github.com/Krixx1337/kx-packet-inspector", NULL, NULL, SW_SHOWNORMAL);
        }

        // kxtools.xyz Link
        ImGui::Text("Website:");
        ImGui::SameLine();
        if (ImGui::Button("kxtools.xyz")) {
             ShellExecuteA(NULL, "open", "https://kxtools.xyz", NULL, NULL, SW_SHOWNORMAL);
        }

        // Discord Link
        ImGui::Text("Discord:");
        ImGui::SameLine();
        if (ImGui::Button("Join Server")) {
             ShellExecuteA(NULL, "open", "https://discord.gg/z92rnB4kHm", NULL, NULL, SW_SHOWNORMAL);
        }
        ImGui::Spacing();
    }
}

void ImGuiManager::RenderStatusControlsSection() {
    // --- Status & Controls Section ---
    if (ImGui::CollapsingHeader("Status")) {
        // Status content
        const char* presentStatusStr = (kx::g_presentHookStatus == kx::HookStatus::OK) ? "OK" : "Failed";
        ImGui::Text("Present Hook: %s", presentStatusStr);

        const char* msgSendStatusStr;
        switch (kx::g_msgSendHookStatus) {
            case kx::HookStatus::OK:       msgSendStatusStr = "OK"; break;
            case kx::HookStatus::Failed:   msgSendStatusStr = "Failed"; break;
            case kx::HookStatus::Unknown:
            default:                       msgSendStatusStr = "Not Found/Hooked"; break;
        }
        ImGui::Text("MsgSend Hook: %s", msgSendStatusStr);

        if (kx::g_msgSendAddress != 0) {
            ImGui::Text("MsgSend Address: 0x%p", (void*)kx::g_msgSendAddress);
            ImGui::SameLine();
            if (ImGui::SmallButton("Copy##SendAddr")) {
                char addrBuf[32];
                snprintf(addrBuf, sizeof(addrBuf), "0x%p", (void*)kx::g_msgSendAddress);
                ImGui::SetClipboardText(addrBuf);
            }
        } else {
            ImGui::Text("MsgSend Address: N/A");
        }

        const char* msgRecvStatusStr;
         switch (kx::g_msgRecvHookStatus) {
            case kx::HookStatus::OK:       msgRecvStatusStr = "OK"; break;
            case kx::HookStatus::Failed:   msgRecvStatusStr = "Failed"; break;
            case kx::HookStatus::Unknown:
            default:                       msgRecvStatusStr = "Not Found/Hooked"; break;
        }
        ImGui::Text("MsgRecv Hook: %s", msgRecvStatusStr);

        if (kx::g_msgRecvAddress != 0) {
            ImGui::Text("MsgRecv Address: 0x%p", (void*)kx::g_msgRecvAddress);
            ImGui::SameLine();
            if (ImGui::SmallButton("Copy##RecvAddr")) {
                char addrBuf[32];
                snprintf(addrBuf, sizeof(addrBuf), "0x%p", (void*)kx::g_msgRecvAddress);
                ImGui::SetClipboardText(addrBuf);
            }
        } else {
            ImGui::Text("MsgRecv Address: N/A");
        }
    }
}

void ImGuiManager::RenderFilteringSection() {
    if (ImGui::CollapsingHeader("Filtering")) {

        // --- Global Direction Filter ---
        ImGui::Text("Show Direction:"); ImGui::SameLine();
        ImGui::RadioButton("All##Dir", reinterpret_cast<int*>(&kx::g_packetDirectionFilterMode), static_cast<int>(kx::DirectionFilterMode::ShowAll)); ImGui::SameLine();
        ImGui::RadioButton("Sent##Dir", reinterpret_cast<int*>(&kx::g_packetDirectionFilterMode), static_cast<int>(kx::DirectionFilterMode::ShowSentOnly)); ImGui::SameLine();
        ImGui::RadioButton("Received##Dir", reinterpret_cast<int*>(&kx::g_packetDirectionFilterMode), static_cast<int>(kx::DirectionFilterMode::ShowReceivedOnly));
        ImGui::Separator();

        // --- Header/Type Filter Mode ---
        ImGui::Text("Filter Mode:"); ImGui::SameLine();
        ImGui::RadioButton("Show All Types", reinterpret_cast<int*>(&kx::g_packetFilterMode), static_cast<int>(kx::FilterMode::ShowAll)); ImGui::SameLine();
        ImGui::RadioButton("Include Checked", reinterpret_cast<int*>(&kx::g_packetFilterMode), static_cast<int>(kx::FilterMode::IncludeOnly)); ImGui::SameLine();
        ImGui::RadioButton("Exclude Checked", reinterpret_cast<int*>(&kx::g_packetFilterMode), static_cast<int>(kx::FilterMode::Exclude));

        // --- Checkbox Section (only if mode is Include/Exclude) ---
        if (kx::g_packetFilterMode != kx::FilterMode::ShowAll) {
            ImGui::Separator();
            ImGui::BeginChild("FilterCheckboxRegion", ImVec2(0, 150.0f), true);
            ImGui::Indent();

            // Group Checkboxes
            if (ImGui::TreeNode("Sent Headers (CMSG)")) {
                for (auto& pair : kx::g_packetHeaderFilterSelection) {
                    if (pair.first.first == kx::PacketDirection::Sent) {
                        uint8_t headerId = pair.first.second;
                        bool& selected = pair.second;
                        std::string name = kx::GetPacketName(kx::PacketDirection::Sent, headerId); // Get name again for display
                        ImGui::Checkbox(name.c_str(), &selected);
                    }
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Received Headers (SMSG)")) {
                // Check if any SMSG headers are actually defined besides placeholder
                bool hasSmsgHeaders = false;
                for (const auto& pair : kx::g_packetHeaderFilterSelection) {
                    if (pair.first.first == kx::PacketDirection::Received) {
                        hasSmsgHeaders = true;
                        break;
                    }
                }

                if (hasSmsgHeaders) {
                    for (auto& pair : kx::g_packetHeaderFilterSelection) {
                        if (pair.first.first == kx::PacketDirection::Received) {
                            uint8_t headerId = pair.first.second;
                            bool& selected = pair.second;
                            std::string name = kx::GetPacketName(kx::PacketDirection::Received, headerId);
                            ImGui::Checkbox(name.c_str(), &selected);
                        }
                    }
                }
                else {
                    ImGui::TextDisabled(" (No known SMSG headers defined yet)");
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Special Types")) {
                for (auto& pair : kx::g_specialPacketFilterSelection) {
                    kx::InternalPacketType type = pair.first;
                    bool& selected = pair.second;
                    std::string name = kx::GetSpecialPacketTypeName(type);
                    ImGui::Checkbox(name.c_str(), &selected);
                }
                ImGui::TreePop();
            }

            ImGui::Unindent();
            ImGui::EndChild();
        }
        ImGui::Separator();
    }
    ImGui::Spacing();
}

void ImGuiManager::RenderPacketLogSection() {
    // Create a snapshot of packets to render to avoid holding lock during ImGui rendering
    std::vector<kx::PacketInfo> packets_to_render;
    size_t total_packets = 0;
    {
    	// Lock scope for accessing global log and creating copy
        std::lock_guard<std::mutex> lock(kx::g_packetLogMutex);
        total_packets = kx::g_packetLog.size(); // Get total count first
        std::vector<int> filtered_indices = kx::Filtering::GetFilteredPacketIndices(kx::g_packetLog);

        packets_to_render.reserve(filtered_indices.size());
        for (int index : filtered_indices) {
            // Double-check index validity against the *current* size under lock
            if (index >= 0 && static_cast<size_t>(index) < kx::g_packetLog.size()) {
                packets_to_render.push_back(kx::g_packetLog[index]); // Copy the packet
            }
        }
    } // Mutex released here

    // --- Statistics Display ---
    ImGui::Text("Packet Log (Showing: %zu / Total: %zu)", packets_to_render.size(), total_packets);

    // --- Log Controls ---
    if (ImGui::Button("Clear Log")) {
        std::lock_guard<std::mutex> lock(kx::g_packetLogMutex);
        kx::g_packetLog.clear();
    }

    ImGui::SameLine();
    if (ImGui::Button("Copy All")) {
        if (!packets_to_render.empty()) {
            std::stringstream ss;
            for (const auto& packet : packets_to_render) {
                ss << kx::Utils::FormatFullLogEntryString(packet) << "\n";
            }
            ImGui::SetClipboardText(ss.str().c_str());
        }
    }

    ImGui::SameLine();
    ImGui::Checkbox("Pause Capture", &kx::g_capturePaused);
    // --- End Log Controls ---

    ImGui::Spacing();

    ImGui::BeginChild("PacketLogScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    // Use clipper only if there are items to display in our copy
    if (!packets_to_render.empty()) 
    {
        ImGuiListClipper clipper;
        clipper.Begin(packets_to_render.size());
		while (clipper.Step()) 
        {
			// No lock needed here, operating on packets_to_render copy
			for (int display_index = clipper.DisplayStart; display_index < clipper.DisplayEnd; ++display_index)
            {
	            // Access the copied packet directly by display_index
	            // Index validation is implicitly handled by iterating up to packets_to_render.size()
	            const auto& packet = packets_to_render[display_index];

	            // Generate display string
	            std::string displayLogEntry = kx::Utils::FormatDisplayLogEntryString(packet);

	            ImGui::PushID(display_index);

	            // --- Color Coding ---
	            ImVec4 textColor;
	            if (packet.direction == kx::PacketDirection::Sent) {
	                textColor = ImVec4(0.4f, 0.7f, 1.0f, 1.0f); // Light Blue for Sent
	            } else { // Received
	                textColor = ImVec4(0.4f, 1.0f, 0.7f, 1.0f); // Light Green for Received
	            }
	            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
	            // --- End Color Coding ---

	            // Display read-only text.
	            float buttonWidth = ImGui::CalcTextSize("Copy").x + ImGui::GetStyle().FramePadding.x * 2.0f + ImGui::GetStyle().ItemSpacing.x;
	            ImGui::PushItemWidth(-buttonWidth);
	            ImGui::InputText("##Pkt", (char*)displayLogEntry.c_str(), displayLogEntry.size() + 1, ImGuiInputTextFlags_ReadOnly);
	            ImGui::PopItemWidth();
	            ImGui::PopStyleColor();

	            ImGui::SameLine();

	            // Copy button: generate and copy full log entry string on click.
	            if (ImGui::SmallButton("Copy")) {
	                std::string fullLogEntry = kx::Utils::FormatFullLogEntryString(packet);
	                ImGui::SetClipboardText(fullLogEntry.c_str());
	            }

	            ImGui::PopID();
			}
		}

        clipper.End();
    }

    // Auto-scroll to bottom if scrollbar is already at the end.
    if (!packets_to_render.empty() && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}


// --- Main Window Rendering Function ---

void ImGuiManager::RenderPacketInspectorWindow() {
    // Check if the window should be closed (user clicked 'x').
    if (!kx::g_isInspectorWindowOpen) {
        return;
    }

    std::string windowTitle = "KX Packet Inspector v";
    windowTitle += kx::APP_VERSION;

    // Set minimum window size constraints before calling Begin
    ImGui::SetNextWindowSizeConstraints(ImVec2(350.0f, 200.0f), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::Begin(windowTitle.c_str(), &kx::g_isInspectorWindowOpen);

    RenderHints();
    RenderInfoSection();
    RenderStatusControlsSection();
    RenderFilteringSection();
    RenderPacketLogSection();

    ImGui::End();
}


void ImGuiManager::RenderUI()
{
    // Only render the inspector window if the visibility flag is set
    if (kx::g_showInspectorWindow) {
        RenderPacketInspectorWindow();
    }

    // You can add other UI elements here if needed
    // ImGui::ShowDemoWindow(); // Keep demo window for reference if needed during dev
}

void ImGuiManager::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
