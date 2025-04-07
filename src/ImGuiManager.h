#pragma once

#include "PacketData.h"
#include <vector>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

class ImGuiManager {
public:
    static bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd);
    static void NewFrame();
    static void Render(ID3D11DeviceContext* context, ID3D11RenderTargetView* mainRenderTargetView);
    static void RenderUI();
    static void Shutdown();
private:
    static void RenderPacketInspectorWindow(); // Main window function
    // Helper functions for RenderPacketInspectorWindow sections
    static void RenderHints();
    static void RenderInfoSection();
    static void RenderStatusControlsSection();
    static void RenderFilteringSection();
    static void RenderPacketLogSection();
    static void RenderSinglePacketLogRow(const kx::PacketInfo& packet, int display_index);

    // Helpers for RenderPacketLogSection
    static std::vector<kx::PacketInfo> GetFilteredPacketsSnapshot(size_t& out_total_packets);
    static void RenderPacketLogControls(size_t displayed_count, size_t total_count, const std::vector<kx::PacketInfo>& packets_to_render);
    static void RenderPacketListWithClipping(const std::vector<kx::PacketInfo>& packets_to_render);
};
