# System Architecture: Network Message Processing

**Status:** Confirmed (Validated Multi-Protocol Architecture, In-Depth Client-Side Analysis)

## High-Level Model: The Three-Server Architecture

The Guild Wars 2 client does not connect to a single monolithic server. Instead, it maintains simultaneous, specialized connections to at least **three distinct backend server systems**, each handling specific functionalities with its own dedicated protocol. This robust architecture ensures functional separation, optimizes traffic, and enhances security.

The three primary protocols and their corresponding server systems are:

1.  **Login Server (`ls2c`/`c2ls`):**
    *   **Purpose:** Handles initial client authentication, account validation, and character selection. This connection is typically short-lived.
    *   **Client Components:** `GcApi` ([`GcApi_StateMachine.c`](../raw_decompilations/cmsg/login/GcApi_StateMachine.c)), `GcAuthCmd` ([`GcAuthCmd_PacketBuilders.c`](../raw_decompilations/cmsg/login/GcAuthCmd_PacketBuilders.c)).
    *   **Lifecycle:** Established only during the initial client startup and character selection.
    *   **Hand-off Mechanism:** Its final task is to provide the client with the specific network coordinates (IP address and port) of the Game Server and Platform Server to which it should connect for the gameplay session.

2.  **Platform Server (`ps2c`/`c2ps`) - The "Portal":**
    *   **Purpose:** Manages all account-level services, social features, and commercial interactions. This includes character inventory/wallet, achievements, friends lists, guilds, LFG, the Trading Post, and the Gem Store.
    *   **Client Components:** `PortalCli` ([`PortalCli_Constructor.c`](../raw_decompilations/cmsg/portal/PortalCli_Constructor.c), [`PortalCli_Auth.c`](../raw_decompilations/cmsg/portal/PortalCli_Auth.c)), `GcPortal` ([`GcPortal_Router.c`](../raw_decompilations/cmsg/portal/GcPortal_Router.c)).
    *   **Traffic Examples:** `REQ /Group/GroupInfo`, `/Presence/UserInfo`, `/Game.com.Wallet/WalletInfo`, `MailMsg`, `ChMsg`.
    *   **Lifecycle:** Established after login and remains active in parallel with the Game Server connection throughout the gameplay session.

3.  **Game Server (`gs2c`/`c2gs`):**
    *   **Purpose:** Handles all real-time, in-world gameplay interactions. This encompasses high-frequency updates like player movement, combat events, skill usage, NPC/agent state synchronization, and dynamic map events.
    *   **Client Component:** Primarily managed by the client's `MsgConn` component.
    *   **Traffic Examples:** `SMSG_PLAYER_STATE_UPDATE`, `SMSG_AGENT_UPDATE_BATCH` (0x0001), `CMSG_MOVEMENT`, `CMSG_USE_SKILL`.
    *   **Lifecycle:** Established after login and remains the core connection for gameplay throughout the session.

**Overall Connection Flow:**
1.  Client initiates connection to Login Server (`c2ls`).
2.  Authentication and character selection occur. See the **[Login Protocol Analysis](./methodologies/discovery_playbooks/login-protocol-analysis.md)**.
3.  Login Server sends connection details for Game and Platform Servers.
4.  Client disconnects from Login Server.
5.  Client establishes simultaneous, persistent connections to both the Game Server (`c2gs`) and Platform Server (`c2ps`).

---

## The Game Server Protocol (`gs2c`/`c2gs`) In-Depth

This section provides a detailed analysis of the primary gameplay connection, which is the current focus of the KX Packet Inspector's hooking mechanisms. This protocol employs a highly dynamic, multi-stage dispatch and serialization system.

### Incoming (SMSG) Packet Processing:

1.  **Framing & Initial Processing (`Msg::DispatchStream`):** All incoming `gs2c` raw byte streams are managed by [`Msg::DispatchStream`](../raw_decompilations/smsg/Msg_DispatchStream.c). This central function is responsible for reading from the network ring buffer, identifying message frames, and performing initial processing (decryption, decompression) to yield plaintext messages.

2.  **Dispatch Type Evaluation:** For each message, `Msg::DispatchStream` retrieves its `Handler Info` structure. A key field in this structure is the "Dispatch Type" (`HandlerInfo+0x10`), which dictates the processing path:
    *   **Dispatch Type `0` (Generic Path):** For most packets. Leads to schema-driven parsing.
    *   **Dispatch Type `1` (Fast Path):** For high-frequency packets like `SMSG_AGENT_UPDATE_BATCH` (0x0001). This path uses hardcoded logic for performance.

3.  **Parsing & Data Extraction:**
    *   **Generic Path (`MsgUnpack::ParseWithSchema`):** For Type `0` packets, `Msg::DispatchStream` calls the schema virtual machine, [`MsgUnpack::ParseWithSchema`](../raw_decompilations/common/MsgUnpack_ParseWithSchema.c), to transform raw bytes into a structured data tuple.
    *   **Fast Path (Hardcoded Parsing):** For Type `1` packets, `Msg::DispatchStream` contains hardcoded logic to manually parse the payload directly from the network buffer.

4.  **Handler Execution:**
    *   **Generic Handler Dispatch:** `Msg::DispatchStream` calls the handler function pointer (from `HandlerInfo+0x18`), passing it the parsed data tuple. (e.g., [`Marker::Cli::ProcessAgentMarkerUpdate`](../raw_decompilations/smsg/Marker_Cli_ProcessAgentMarkerUpdate.c)).
    *   **Fast Path Notification:** After internal processing, `Msg::DispatchStream` calls a notification stub (e.g., [`Event::PreHandler_Stub_0x88`](../raw_decompilations/common/event_system/Event_PreHandler_Stub_0x88.c)) which may queue a new event via [`Event::Factory_QueueEvent`](../raw_decompilations/common/event_system/Event_Factory_QueueEvent.c).

### Outgoing (CMSG) Packet Processing:

1.  **Game Logic Initiates:** High-level game logic prepares raw data for an outgoing packet.
2.  **Packet Building (`MsgConn::BuildPacketFromSchema`):** The game logic calls [`MsgConn::BuildPacketFromSchema`](../raw_decompilations/cmsg/MsgConn_BuildPacketFromSchema.c), the primary utility for constructing outgoing packets.
3.  **Serialization (`Msg::MsgPack`):** `BuildPacketFromSchema` internally uses [`Msg::MsgPack`](../raw_decompilations/cmsg/Msg_MsgPack.c), a schema-driven "virtual machine" to write data into a buffer.
4.  **Queueing & Sending:** The completed packet is placed into an outgoing queue:
    *   **Direct Queue (`MsgConn::QueuePacket`):** For discrete events (e.g., heartbeats, agent links), packets are passed to [`MsgConn::QueuePacket`](../raw_decompilations/cmsg/MsgConn_QueuePacket.c), which calls the lower-level [`MsgConn::EnqueuePacket`](../raw_decompilations/cmsg/MsgConn_EnqueuePacket.c).
    *   **Buffered Stream (`MsgConn::FlushPacketBuffer`):** For continuous data (e.g., movement), data is written into a buffer and periodically flushed by [`MsgConn::FlushPacketBuffer`](../raw_decompilations/cmsg/MsgConn_FlushPacketBuffer.c).

---

## The Platform Server Protocol (`ps2c`/`c2ps`) (The "Portal" System)

This connection handles all non-real-time, account-specific interactions. See the **[Portal Dispatch Analysis](./methodologies/discovery_playbooks/portal-dispatch-analysis.md)** for a full breakdown.

*   **Key Functions:**
    *   [`Portal::DispatchMessage`](../raw_decompilations/smsg/Portal_DispatchMessage.c): The central `switch` statement that routes incoming Portal messages.
    *   [`Portal::DynamicHandler_Entry`](../raw_decompilations/smsg/Portal_DynamicHandler_Entry.c): The entry point called by the main network layer to hand off messages to the Portal system.

