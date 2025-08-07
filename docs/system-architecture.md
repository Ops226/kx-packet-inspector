# System Architecture: Network Message Processing

**Status:** Confirmed (Validated Multi-Protocol Architecture, In-Depth Client-Side Analysis)

## High-Level Model: The Three-Server Architecture

The Guild Wars 2 client does not connect to a single monolithic server. Instead, it maintains simultaneous, specialized connections to at least **three distinct backend server systems**, each handling specific functionalities with its own dedicated protocol. This robust architecture ensures functional separation, optimizes traffic, and enhances security.

The three primary protocols and their corresponding server systems are:

1.  **Login Server (`ls2c`/`c2ls`):**
    *   **Purpose:** Handles initial client authentication, account validation, and character selection. This connection is typically short-lived.
    *   **Typical Port:** Often uses standard web ports (e.g., TCP 443) or specialized login ports.
    *   **Lifecycle:** Established only during the initial client startup and character selection.
    *   **Hand-off Mechanism:** Its final task is to provide the client with the specific network coordinates (IP address and port) of the Game Server and Platform Server to which it should connect for the gameplay session. This is often communicated via a `DispatchToGameServer` message.

2.  **Platform Server (`ps2c`/`c2ps`):**
    *   **Purpose:** Manages all account-level services, social features, and commercial interactions. This includes character inventory/wallet, achievements, friends lists, guilds, LFG (Looking for Group), the Trading Post, and the Gem Store (Wizards Vault). This is the "Portal" system identified in client code.
    *   **Client Component:** Managed by the client's `PortalCli` component, which likely initializes and manages its own secure socket (e.g., `Arena::Services::StsInetSocket`).
    *   **Traffic Examples:** `REQ /Group/GroupInfo`, `/Presence/UserInfo`, `/Game.com.Wallet/WalletInfo`, `MailMsg`, `ChMsg` (Channel Message).
    *   **Lifecycle:** Established after login and remains active in parallel with the Game Server connection throughout the gameplay session.

3.  **Game Server (`gs2c`/`c2gs`):**
    *   **Purpose:** Handles all real-time, in-world gameplay interactions. This encompasses high-frequency updates like player movement, combat events, skill usage, NPC/agent state synchronization, and dynamic map events.
    *   **Client Component:** Primarily managed by the client's `MsgConn` component.
    *   **Traffic Examples:** `SMSG_PLAYER_STATE_UPDATE`, `SMSG_AGENT_UPDATE_BATCH` (0x0001), `CMSG_MOVEMENT`, `CMSG_USE_SKILL`.
    *   **Lifecycle:** Established after login and remains the core connection for gameplay throughout the session.

**Overall Connection Flow:**
1.  Client initiates connection to Login Server (`c2ls`).
2.  Authentication and character selection occur over the Login Server protocol.
3.  Login Server sends connection details for Game and Platform Servers to the client.
4.  Client disconnects from Login Server.
5.  Client establishes simultaneous, persistent connections to both the Game Server (`c2gs`) and Platform Server (`c2ps`). These two connections run in parallel during gameplay.

## The Game Server Protocol (`gs2c`/`c2gs`) In-Depth

This section provides a detailed analysis of the primary gameplay connection, which is the current focus of the KX Packet Inspector's hooking mechanisms. This protocol employs a highly dynamic, multi-stage dispatch and serialization system.

### Unified Process Flow (Game Server)

The `gs2c`/`c2gs` communication pipeline is intricate, featuring both generic, schema-driven paths and highly optimized "Fast Paths" for high-frequency data.

#### Incoming (SMSG) Packet Processing:

1.  **Framing & Initial Processing:** All incoming `gs2c` raw byte streams are managed by `Msg::DispatchStream`. This central function is responsible for:
    *   Reading raw bytes from the network ring buffer.
    *   Identifying individual message frames (packet boundaries).
    *   Performing initial processing, including decryption (e.g., RC4) and decompression, to yield plaintext, framed messages.
    *   It uses a `MsgConn` object (passed as `param_2` in assembly) for context and buffer management. Relevant offsets include: `MSGCONN_BUFFER_BASE_PTR_OFFSET (0x88)`, `MSGCONN_BUFFER_READ_IDX_OFFSET (0x94)`, `MSGCONN_BUFFER_WRITE_IDX_OFFSET (0xA0)`.

2.  **Dispatch Type Evaluation:** For each framed message, `Msg::DispatchStream` retrieves its `Handler Info` structure (from `MsgConn` at offset `0x48`, specifically `*(longlong *)(param_2 + 0x48)`). A key field in this structure is the "Dispatch Type" (at `HandlerInfo+0x10`). This type dictates the subsequent processing path:
    *   **Dispatch Type `0` (Generic Path):** For most packets. Leads to schema-driven parsing and a handler that receives a parsed data tuple.
    *   **Dispatch Type `1` (Fast Path):** For high-frequency packets like `SMSG_AGENT_UPDATE_BATCH` (0x0001). This path optimizes performance by bypassing the generic schema's output and relying on hardcoded processing.

3.  **Parsing & Data Extraction:**
    *   **Generic Path (`MsgUnpack::ParseWithSchema`):** For Dispatch Type `0` packets, `Msg::DispatchStream` calls the schema virtual machine, `MsgUnpack::ParseWithSchema`. This function interprets an opcode-specific schema to transform raw bytes into a structured data tuple in a temporary buffer.
    *   **Fast Path (Hardcoded Parsing):** For Dispatch Type `1` packets (e.g., 0x0001), `Msg::DispatchStream` does **not** rely on `MsgUnpack::ParseWithSchema` for the final payload structure. Instead, the raw payload data (still available in the network buffer) is manually read and processed by hardcoded logic directly within `Msg::DispatchStream`. This involves repeated calls to low-level buffer reading utilities like `FUN_140fd70a0` (e.g., `Arena::Core::Collections::Array`) to extract specific fields (e.g., floats for coordinates, integers for IDs).

4.  **Handler Execution:**
    *   **Generic Handler Dispatch:** For Dispatch Type `0` packets, `Msg::DispatchStream` calls the handler function pointer (from `HandlerInfo+0x18`), passing it the parsed data tuple. (Example: `Marker::Cli::ProcessAgentMarkerUpdate` for `FE 03` messages).
    *   **Fast Path Notification:** For Dispatch Type `1` packets, after `Msg::DispatchStream` has completed its internal parsing and state updates, it calls a registered "notification" stub (e.g., `Event::PreHandler_Stub_0x88`) via `CALL RAX` (e.g., at `140fd1acd`). This stub receives only context arguments and its purpose is to trigger further, decoupled engine events (e.g., through `Event::Factory_QueueEvent`), rather than directly processing packet data.

#### Outgoing (CMSG) Packet Processing:

1.  **Game Logic Initiates:** High-level game logic (e.g., player movement, skill usage, inventory actions) prepares raw data for an outgoing packet.
2.  **Packet Building (`MsgConn::BuildPacketFromSchema`):** The game logic calls `MsgConn::BuildPacketFromSchema` (proposed name for `FUN_140fd4c10`). This is the primary utility for constructing outgoing packets. It takes a schema definition and raw data, then orchestrates the serialization.
3.  **Serialization (`Msg::MsgPack`):** `MsgConn::BuildPacketFromSchema` internally uses `Msg::MsgPack` (proposed name for `FUN_140fd2c70`), which acts as a schema-driven "virtual machine" to write the data into a packet buffer according to the schema's typecodes.
4.  **Queueing & Sending:** The completed packet is then placed into a thread-safe outgoing queue:
    *   **Direct Queue (`MsgConn::QueuePacket`):** For discrete, high-priority events (e.g., heartbeats `0x0005`, agent links `0x0036`), packets are passed directly to `MsgConn::QueuePacket` (`FUN_14104d760`), which in turn calls `MsgConn::EnqueuePacket` (`FUN_141051690`) for low-level queueing.
    *   **Buffered Stream (`MsgConn::FlushPacketBuffer`):** For continuous, high-frequency, or aggregated data (e.g., movement `0x0012`), data is initially written into a common buffer using `MsgConn::WriteToPacketBuffer`. This buffer is periodically flushed by `MsgConn::FlushPacketBuffer` (`FUN_140fd1e80`), which then processes the accumulated data, performing serialization and queueing. The `MsgSendContext` structure (defined in `GameStructs.h`) is critical for accessing buffer state and pointers during this process.

### Key Components of the Game Server Connection

*   **`Msg::DispatchStream`:** (Primary Incoming Orchestrator, `Gw2-64.exe` offset `0xFD18CC`). Manages incoming message stream, handles framing/decryption, and performs dynamic dispatch based on "Dispatch Type" (from `HandlerInfo+0x10`). Contains hardcoded Fast Path parsing.
*   **`MsgUnpack::ParseWithSchema`:** (Generic Schema Parser, `Gw2-64.exe` offset `0xFD43C0`). Interprets bytecode-like schemas to convert raw bytes to structured data tuples.
    *   **Schema Typecodes:** `0x02` (u8), `0x0c` (composite), `0x11` (u8-counted array), `0x12` (u16-counted array), `0x13` (fixed byte array), `0x14` (u8-len byte array), `0x15` (u16-len byte array), `0x17` (u32).
*   **`MsgConn::BuildPacketFromSchema`:** (Outgoing Packet Builder, proposed name for `FUN_140fd4c10`). High-level utility to prepare CMSG packets from schemas and data.
*   **`Msg::MsgPack`:** (Schema Serialization Engine, proposed name for `FUN_140fd2c70`). Internal function called by `MsgConn::BuildPacketFromSchema` to write data to buffers according to schema.
*   **`MsgConn` Object Context:** (`param_2` in `Msg::DispatchStream`, `param_1` in `MsgSendHook::hookMsgSend`). A central object for managing network state, buffers, and handlers. Key offsets include:
    *   `MSGCONN_HANDLER_INFO_PTR_OFFSET` (0x48): Pointer to the current message's `Handler Info` structure.
    *   `MSGCONN_BUFFER_BASE_PTR_OFFSET` (0x88): Base pointer of the raw network ring buffer.
    *   `MSGCONN_BUFFER_READ_IDX_OFFSET` (0x94): Current read position in the ring buffer.
    *   `MSGCONN_BUFFER_WRITE_IDX_OFFSET` (0xA0): Current write position in the ring buffer.
*   **`Handler Info` Structure:** (Obtained from `MsgConn+0x48`). A 32-byte structure defining how a message is handled. Key offsets:
    *   `HANDLER_INFO_MSG_ID_OFFSET` (0x00): The packet's opcode (e.g., `0x0001`).
    *   `HANDLER_INFO_MSG_DEF_PTR_OFFSET` (0x08): Pointer to the `Message Definition` structure.
    *   `HANDLER_INFO_DISPATCH_TYPE_OFFSET` (0x10): The "Dispatch Type" (`0` for Generic, `1` for Fast Path).
    *   `HANDLER_INFO_HANDLER_FUNC_PTR_OFFSET` (0x18): Pointer to the handler function/notification stub.
*   **`Message Definition` Structure:** (Obtained from `HandlerInfo+0x08`). Defines message properties. Key offset:
    *   `MSG_DEF_SIZE_OFFSET` (0x20): The size of the message payload.
*   **`FUN_140fd70a0` (e.g., `Arena::Core::Collections::Array`):** (Ring Buffer Reader). A core utility function used to extract bytes from circular buffers in `Msg::DispatchStream`.

## The Platform Server Protocol (`ps2c`/`c2ps`) (The "Portal" System)

This connection operates in parallel to the Game Server connection and handles all non-real-time, account-specific interactions.

*   **Client Components:**
    *   **`Gw2::Services::PortalCli::PortalCli`:** (Constructor). Initializes the connection, resolving the server via the DNS name `"cligate"`.
    *   **`Gw2::Services::PortalCli::PortalCliAuth`:** Handles authentication with the Portal backend, involving secure socket creation (`Arena::Services::StsInetSocket`).
    *   **`Gw2::Game::Net::Cli::GcPortal`:** Acts as a router. For certain commands (e.g., `param_2 == 7`), it redirects processing to the `PortalCli` system, bypassing the standard `MsgConn` Game Server queue. This is where requests like `REQ /Group/GroupInfo` or `REQ /Game.com.Wallet/WalletInfo` are initiated.
*   **Traffic Characteristics:** Often involves HTTP-like requests (`REQ`) and responses (`RESP`), handling structured data for UI elements like leaderboards, wallet info, and social graphs.

## The Login Server Protocol (`ls2c`/`c2ls`)

This is a transient connection serving the initial client lifecycle.

*   **Client Components:** Handles modules related to `LoginStatus`, `SelectCharacter`, `PlayInfo`, and `MapInfo`.
*   **Role:** Facilitates user authentication against account credentials, character selection, and initial game world assignment.
*   **Hand-off:** Crucially, it provides the client with the necessary connection details (IP address, port) to connect to the specific Game Server and Platform Server, enabling the client to disconnect from the Login Server and establish the persistent gameplay connections.

## Implications for Reverse Engineering

*   **Multi-Protocol Hooking:** A complete packet capture requires hooking not only the `MsgConn` functions (for `gs2c`/`c2gs` traffic) but also the send/receive methods of the `StsInetSocket` used by the `PortalCli` component (for `ps2c`/`c2ps` traffic) and potentially the ephemeral Login Server connection.
*   **Layered Packet Processing:** `Msg::DispatchStream`'s "Fast Path" for high-frequency packets like `0x0001` means their payload structures are hardcoded and need direct analysis of the `Msg::DispatchStream` function itself, rather than relying on `MsgUnpack::ParseWithSchema`.
*   **Context is King:** The behavior and meaning of data are heavily dependent on the specific protocol (Game, Platform, Login) and the internal "Dispatch Type" used within `Msg::DispatchStream`.
*   **Function Naming:** As demonstrated, leveraging strings from assertions (e.g., `D:\Perforce\...`) and known API patterns is incredibly effective for recovering meaningful function names (`PortalCli`, `Arena::Core::Collections::Array`, `Gw2::Services::Msg::MsgUtil`), which then clarify architectural roles.