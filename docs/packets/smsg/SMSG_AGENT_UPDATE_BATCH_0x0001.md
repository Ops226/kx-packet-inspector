# SMSG_AGENT_UPDATE_BATCH (0x0001)

**Direction:** Server -> Client
**Status:** Confirmed Container Packet (Complex Dispatch)

## Summary

`SMSG 0x0001` is not a single packet type but a **high-frequency container packet** used to bundle various agent-related events into a single stream. It is one of the most common packets received during gameplay, handling events like agent movement, state changes, and despawns.

Instead of a single handler, this packet uses a multi-stage dispatch system. The initial payload contains a **subtype ID** which is used by the client to call one of many "pre-handler stubs." These stubs, in turn, create an internal game event using a hardcoded **sub-opcode**, which is then processed by a central event system.

## Evidence of Container Nature

The conclusion that this is a container packet is based on several key pieces of evidence from dynamic analysis:

1.  **Multiple Dynamic Handlers:** Live debugging shows that opcode `0x0001` is dispatched to at least six different pre-handler functions.
2.  **Variable Payload Sizes:** Captured logs show this packet with many different sizes (9, 14, 25, 26, 38, 44, 62 bytes), which is inconsistent with a single, fixed-structure packet.
3.  **"Fast Path" Execution:** The packet's processing bypasses the generic `MsgUnpack::ParseWithSchema` function, indicating a highly optimized, custom parsing path is used. The pre-handlers themselves do not parse the data, suggesting they are part of a larger dispatch chain.

## The Full Dispatch Flow

The processing of this packet is a complex, multi-stage process designed for high performance:

1.  **`Msg::DispatchStream` (Initial Dispatch):** The main dispatcher identifies opcode `0x0001`. It performs a minimal parse of the payload to extract a **subtype ID**. Based on this subtype, it calls the appropriate pre-handler stub.

2.  **Pre-Handler Stub (e.g., `Event::PreHandler_Stub_0x88`):** These are extremely simple wrapper functions. Their only purpose is to call the central Event Factory, `Event::Factory_QueueEvent`, passing it a hardcoded **sub-opcode**. They do not read the packet payload.

3.  **Event Factory (`Event::Factory_QueueEvent`):** This function receives the sub-opcode and creates a generic internal "event" object.

4.  **Event Dispatcher (`Event::Dispatcher_ProcessEvent`):** This function takes the newly created event object and its sub-opcode. It uses the sub-opcode as an index into a **Master Event Handler Table** (`DAT_14263eb60`) to find the *real* handler for that specific event.

5.  **Master Event Handler Table (`DAT_14263eb60`):** This is a large, global array of pointers. Critically, it is **populated at runtime** during the game's initialization, making its contents invisible to simple static analysis of the executable file.

6.  **Real Handler Function (The Final Destination):** The function pointer retrieved from the table is finally called. This function contains the actual logic that **manually parses the packet payload** for its specific event type.

## Known Sub-Opcodes and Their Stubs

Our static analysis of the pre-handler stubs provides a definitive mapping of which stub is responsible for which sub-opcode.

| Pre-Handler Stub Function | Hardcoded Sub-Opcode |
| :--- | :--- |
| `Event::PreHandler_Stub_0xD0` | `0xD0` |
| `Event::PreHandler_Stub_0x78` | `0x78` |
| `Event::PreHandler_Stub_0x88` | `0x88` |
| `Event::PreHandler_Stub_0xC0` | `0xC0` |
| `Event::PreHandler_Stub_0xC0_b` | `0xC0` |
| `Event::PreHandler_Stub_0x70` | `0x70` |

## Hypothesized Payload Structure

Based on log analysis, it is highly likely that all `0x0001` variants begin with a common header that includes the subtype used by `Msg::DispatchStream` to select the pre-handler.

```cpp
#pragma pack(push, 1)
// Represents the common header at the start of every 0x0001 payload
struct SmsgAgentUpdateHeader {
    uint16_t subtype;   // The actual event type ID
    uint16_t agentId;   // The agent this update applies to
    uint32_t tick;      // A server-side tick or timestamp for sequencing
};

// Example for a 26-byte movement packet
struct SmsgAgentUpdate_Movement {
    SmsgAgentUpdateHeader header; // subtype would be 0x0040
    float pos_x;
    float pos_y;
    float pos_z;
    // ... plus other data like rotation, etc.
};
#pragma pack(pop)
```

## Unsolved Mysteries & Next Steps

The investigation was paused because the final link in the chain—the contents of the Master Event Handler Table at `DAT_14263eb60`—could not be resolved statically.

The definitive next step to fully reverse engineer this packet is:

1.  **Launch `Gw2-64.exe` from a debugger (e.g., x64dbg).** Attaching after launch is too late, as the table is populated during initialization.
2.  **Set a hardware write breakpoint** on a specific slot in the live memory map for the table (e.g., `LiveBase + OffsetOfTable + (0x88 * 8)`).
3.  This breakpoint will trigger on the exact instruction within the **Master Event Registration Function** that populates the table.
4.  Analyzing this registration function in Ghidra will allow for a complete, static mapping of all sub-opcodes to their real handler functions, finally revealing the parsing logic for every event type.

## Supporting Evidence (Decompilations)

*   `docs/raw_decompilations/common/event_system/Event_PreHandler_Stub_0x88.c`
*   `docs/raw_decompilations/common/event_system/Event_Factory_QueueEvent.c`
*   `docs/raw_decompilations/common/event_system/Event_Dispatcher_ProcessEvent.c`