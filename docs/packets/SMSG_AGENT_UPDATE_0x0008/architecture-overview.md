# Tiny-Movement Architecture Overview
Date: 2025-08-04

Purpose
Provide a version-resilient, public-safe overview of the tiny-movement parsing architecture observed in Gw2-64.exe. This document emphasizes patterns and interfaces over brittle absolute addresses, to help readers rediscover components after game updates.

Scope and Constraints
- Static reverse engineering only (no live reads).
- Examples avoid referencing proprietary symbols; use functional nicknames and offsets instead (e.g., “+0x38 parser”).
- When absolute addresses are mentioned in companion docs, treat them as examples; re-find via patterns described here.

High-Level Components
- Provider: Offers access to tiny-movement parse context and a readiness/gate method.
- Entry (Subtype Class): One per movement variant; owns the parser and aux methods. Chosen by an index from a context and registered into a global table.
- Table & Indexer: Holds pointers to entry objects; indexer maps a subtype index (from parse context) to an entry pointer.
- Hub: Given a provider/context, resolves the entry and dispatches into the entry’s parser.
- Consumer(s): Call into the provider/hub, invoke the entry parser (+0x38), sometimes a sibling method (+0x48), then route parsed data into downstream systems (e.g., movement commit paths).

Key Interfaces (stable offsets)
- Entry vtable:
  - +0x38: Parser entry point for the subtype (“TM_SubtypeX_Parse”).
  - +0x48: Sibling accessor/method commonly used immediately after parsing.
  - +0x88: Subtype index virtual (used during registration to select the table slot).
  - +0x158: Aux accessor (used in a branch when an auxiliary fetch is requested).
- Provider vtable (observed usage):
  - +0x80: Returns tiny-movement parse context pointer.
  - +0x38: Readiness/gate check before parsing and routing.

Global Table & Indexer Pattern
- Table data layout (pattern):
  - [table+0x08] → entries pointer (array of entry pointers).
  - [table+0x14] → count.
- Indexer logic (pattern):
  - Reads subtype index from parse context (e.g., parse_ctx+0x0C).
  - Ensures index < count, returns entries[index].
- Registration pattern (constructor):
  - Writes vtable =&PTR_FUN_xxx into [this].
  - Calls Table_GetBasePtr, then Registration_Helper(table, this).
  - Registration helper queries this->vfunc(+0x88) to compute the slot index, and writes “this” into entries[index].

Dispatch Flow (typical)
1) Resolve entry object:
   - Provider → get parse context; indexer selects entry from the table.
2) Dispatch parser:
   - Hub/consumer performs indirect call to entry->vfunc(+0x38).
3) Optional sibling:
   - Consumer often calls entry->vfunc(+0x48) immediately after parse.
4) Downstream handling:
   - Parsed results/flags affect subsequent calls (e.g., flag tests, movement commit calls into other subsystems).

Consumer-Side Semantic Clues
- Flag bytes:
  - Consumers often test a bit in a register/byte after parsing (e.g., TEST reg, 0x1). Treat as evidence for a flags field in the parsed packet.
- Movement commit:
  - Post-parse code frequently performs virtual calls that accept indices or scaled values; trace which values are passed to identify field scales (e.g., Q format).

Version Drift and Re-Find Strategy
- Do not rely on absolute addresses; re-find via:
  - Indirect vtable calls: search for CALL/JMP [vtable+0x38] near the tiny-movement consumer/hub. Take a short disassembly slice and confirm patterns.
  - Registration constructors: search for constructors that set a vtable pointer and then call Table_GetBasePtr + Registration_Helper. Walk to the vfunc(+0x88) implementation to learn subtype indices.
  - Indexer fingerprint: look for code reading parse_ctx+0x0C (or another fixed offset), bounds checking against [table+0x14], and indexing into [table+0x08].
- Save anchors as patterns, not static RVAs, and tag findings with a “build/version” note.

Naming Conventions (public-safe)
- Entry class: TM_SubtypeX_Entry (if X is known from vfunc+0x88), or TM_Entry_vtbl_XXXXXXXX if referring to a known vtable label hash/id.
- Methods:
  - +0x38 → TM_SubtypeX_Parse
  - +0x48 → TM_SubtypeX_Sibling
  - +0x88 → TM_SubtypeX_GetIndex
  - +0x158 → TM_SubtypeX_Aux
- Hubs/Consumers:
  - TM_Hub_ParseTinyMovementPayload_Call
  - TM_Consumer_… (use a short functional nickname plus an address tag in private notes if needed)

Deliverables to Produce Per Subtype
- Field table: offset, type, name, scale/bit layout.
- Evidence lines: absolute addresses with ≤120 lines of disasm/decompile context that show how each field is consumed/validated.
- Final C++:
  - Struct with raw types.
  - Safe parser with bounds checks, subtype gating, and exact conversions.
- Integration note:
  - Where to hook in your packet inspector.
  - Minimal logging for validation (e.g., print dx/dy/dz/flags once per N packets).

Appendix — Re-Find Checklist
- Consumer/hub:
  - Find an indirect virtual CALL/JMP to [vtable+0x38] that correlates with movement parsing; take a short slice.
- Registration constructor:
  - Look for vtable store → Table_GetBasePtr → Registration_Helper; follow vfunc(+0x88) to learn the subtype index.
- Indexer:
  - Identify parse_ctx field access (commonly a fixed offset), check vs count, and retrieval from entries array.
- Sibling/Aux:
  - After parse, record calls to [vtable+0x48] and consider branches calling [vtable+0x158] in certain modes.

This overview is meant to remain stable across versions by emphasizing patterns and interfaces rather than fixed addresses.
