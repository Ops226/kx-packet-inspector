# Packet Parser Discovery Playbook (Static RE, No Live Reads)
Date: 2025-08-04

Objective
Provide a concise, repeatable method to discover packet parsers and document them with evidence. This playbook captures patterns verified while finalizing tiny-movement subtype 0 and generalizes them for future subtypes and other packet families.

Scope and Constraints
- Environment: Ghidra CodeBrowser on Gw2-64.exe with full analysis; GhidraMCP bridge available.
- Static only: no live memory reads. Prefer indirect-call callsite evidence over speculative inlining guesses.
- Evidence discipline: capture absolute addresses and short, targeted slices (<=120 lines).

1) Core Concepts and Patterns
A. Indirect vtable dispatch
- Parsers are frequently invoked via virtual calls at fixed vtable offsets.
- For tiny-movement entries:
  - +0x38: parser entry point
  - +0x48: sibling accessor used post-parse in consumers
  - +0x158: aux accessor (used when a specific parameter == 1)
- Hubs/consumers may tail-call JMP [vtable+slot] or CALL [vtable+slot]. These are your canonical anchors.

B. Registration into a table with subtype indexers
- A common pattern: constructors set vtable =&PTR_FUN_xxx and register object into a global dispatch table.
- Index selection is typically via a virtual function (e.g., +0x88), and an indexer reads a key from a parse context structure to pick the entry.
- For tiny-movement:
  - Table base: &DAT_142578428
  - Entries pointer at [table+0x08], count at [table+0x14]
  - Indexer 0x1411417c0: reads parse_ctx+0x0C and selects entries[index] with bounds check

C. Consumer-side semantic footprints
- When a parser’s body isn’t directly materialized, consumer code around the call provides semantics:
  - Flag tests (e.g., TEST of a register/byte)
  - Movement commit paths where parsed values are pushed into subsystems via additional virtual calls
  - Contextual gating (e.g., comparing IDs, performing list iterations)

2) Concrete Anchors (Tiny-Movement seed list)
Use these to pivot quickly.

Hub and consumer
- TM_Hub_ParseTinyMovementPayload_Call (0x1410eb7e0)
  - 1410eb7fb..1410eb809: MOV RCX,[RAX]; MOV RDX,[RCX+0x38]; JMP RDX (tail-call parser +0x38)
- Consumer (0x141446d80)
  - 141447048..141447068: CALL TM_GetEntryOrAux_ByParseCtx_Param2Gate → MOV RDX,[vtable+0x38]; CALL RDX
  - 141447074..14144707b: then MOV RDX,[vtable+0x48]; CALL RDX

Indexer and table
- 0x1411417c0: reads parse_ctx+0x0C; selects entries[index] from [table+0x08] with bounds vs [table+0x14]
- TM_Table_GetBasePtr returns &DAT_142578428

Constructors that register into the table
- 0x1410f6430: vtable =&PTR_FUN_142110ec0; then TM_Table_GetBasePtr() and FUN_1411417e0(table, this)
- 0x1410f6960: vtable =&PTR_FUN_142110c08; then TM_Table_GetBasePtr() and FUN_1411417e0(table, this)

Aux/entry provider gate
- 0x1410f5fb0 (TM_GetEntryOrAux_ByParseCtx_Param2Gate)
  - If param_2 == 1: CALL [vtable+0x158]
  - Otherwise returns the entry object for +0x38 parser call

3) Workflow (Step-by-Step)
Phase A — Anchor discovery
1) Start from a hub/consumer cluster known to parse the target packet family.
   - Example: tiny-movement hub 0x1410eb7e0; consumer 0x141446d80.
2) Locate indirect calls:
   - JMP/CALL [vtable+slot] around the parsing point. Record absolute addresses (2–6 lines).
3) Capture downstream semantics:
   - Immediately after parser calls, record flag tests (TEST bitmasks, AND/OR on state flags) and movement/commit calls that take parser output forward.
   - Example: 0x141446d80@0x141447188: TEST DIL,0x1 → branch; indicates flags bit0 semantics.

Phase B — Registration and subtype identification
1) Find constructors that write a vtable pointer and then call the registration helper with TM_Table_GetBasePtr.
   - 0x1410f6430 → vtbl =&PTR_FUN_142110ec0; registers
   - 0x1410f6960 → vtbl =&PTR_FUN_142110c08; registers
2) Confirm subtype via vfunc +0x88:
   - Decompile the +0x88 virtual. It typically returns the subtype index used by FUN_1411417e0 to map into the table.
3) Optionally enumerate vtable slots (+0x38/+0x158) to obtain concrete callee addresses.
   - If MCP cannot materialize, keep callsites as canonical anchors and proceed with consumer-based reversal.

Phase C — Reverse the parser (evidence-first)
1) If concrete +0x38 callee is available:
   - Decompile/disassemble and enumerate buffer reads (offsets, sizes, signedness), scales (e.g., Q8.8).
2) If not:
   - Use consumer evidence: extract flag semantics, movement deltas/scales, and how commits occur downstream.
3) Record each field with an evidence line (absolute address and 1–3 lines of decomp/disasm).

Phase D — Produce deliverables
1) Field table: offset, type, name, scale/bit-layout, each with evidence lines.
2) Final C++:
   - Struct with types and raw fields
   - Safe parser: bounds checks, subtype guard, conversions (e.g., Q8.8 → float)
3) Integration notes for kx-packet-inspector:
   - Where to add in PacketParser.cpp
   - Minimal logging for validation

4) Evidence Recording Rules
- Always capture absolute addresses with nearby code (<=120 lines per slice).
- Prefer minimal, clear slices that show:
  - The indirect vtable call at the hub/consumer anchor
  - The flag test/bit operation or commit point
  - The indexer reading the subtype (parse_ctx+0x0C) and selecting entries
- If a vtable concrete target is resolved, add it with a one-liner referencing the constructor storing the vtable and the vtable slot address.

5) Naming Conventions
- Classes:
  - TM_SubtypeX_Entry if subtype index is known (X = decimal subtype)
  - TM_Entry_vtbl_142110ec0 if naming must be tied to vtable symbol
- Methods:
  - +0x38 → TM_SubtypeX_Parse
  - +0x48 → TM_SubtypeX_Sibling
  - +0x88 → TM_SubtypeX_GetIndex
  - +0x158 → TM_SubtypeX_Aux
- Hubs/Consumers:
  - TM_Hub_ParseTinyMovementPayload_Call (0x1410eb7e0)
  - TM_Consumer_141446d80

6) Quick Start Checklist for a New Subtype
- Identify hub/consumer callsites that call [vtable+0x38]:
  - Record absolute addresses (2–6 lines each).
- Find a constructor that registers into the same table:
  - Record vtable pointer set and the call to FUN_1411417e0.
- Confirm subtype via +0x88 virtual:
  - If subtype==target, enumerate slots or proceed with consumer-based reversal.
- Produce:
  - Field table with evidence lines
  - Safe, bounds-checked parser with a subtype guard
  - Integration notes and minimal logging

7) Tiny-Movement Subtype 0 Summary (as an example template)
- Parser (+0x38) anchors:
  - 0x141446d80@14144705e..68; 0x1410eb7e0@1410eb7fb..09
- Aux (+0x158) callsite:
  - 0x1410f5fb0@1410f5ff9..ff (when param_2 == 1)
- Indexer/table:
  - 0x1411417c0 uses parse_ctx+0x0C; [table+0x08] entries; [table+0x14] count
- Field table (Pass 1):
  - 0x10 int16 dx_q88 (1/256), 0x12 int16 dy_q88 (1/256), 0x14 int16 dz_q88 (1/256), 0x16 u8 flags (bit0 observed by TEST DIL,0x1)
- Evidence:
  - Flag bit test: 0x141447188
  - Post-parse movement pipeline using vtable +0x118/+0x108/+0xF8 and commit at 0x14144727a
- Parser and integration:
  - Safe C++ parser delivered; PacketParser.cpp notes and minimal logging included

8) Troubleshooting Tips
- If indirect callee addresses don’t materialize in MCP:
  - Keep anchors and consumer-derived evidence; proceed with spec and parser.
- If constructors aren’t obvious:
  - Search for patterns: writing &PTR_FUN_xxx into [RCX] and calling TM_Table_GetBasePtr + FUN_1411417e0.
- If subtype isn’t clear:
  - Decompile likely +0x88; cross-check index range against table count [table+0x14].

Appendix — Useful Addresses and Symbols (Tiny-Movement)
- Hub: 0x1410eb7e0
- Consumer: 0x141446d80
- Indexer: 0x1411417c0
- Table: &DAT_142578428; [table+0x08] entries; [table+0x14] count
- Registration constructors:
  - 0x1410f6430 → vtbl =&PTR_FUN_142110ec0
  - 0x1410f6960 → vtbl =&PTR_FUN_142110c08
- Aux/entry provider: 0x1410f5fb0 (param_2 == 1 → +0x158)
