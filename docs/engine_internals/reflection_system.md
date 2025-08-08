# Engine Internals: The ArenaNet `hkReflect` System

**Status:** Confirmed (Evidence-Based Analysis)

This document outlines the core principles of the proprietary reflection system used in the Guild Wars 2 game engine. The findings are the result of an extensive forensic analysis of the `Gw2-64.exe` binary.

This system is the blueprint for the game's object model, defining the memory layout, inheritance, and member variables for hundreds of core engine and gameplay classes.

## Core Discovery: A Custom `hkReflect` System

Initial analysis of the binary suggested the use of a standard Havok `hkClass`/`hkClassMember` reflection system. However, this was proven incorrect. The game engine uses a separate, more modern, and likely customized system based on a different Havok namespace: **`hkReflect`**.

*   **Evidence:** The executable is rich with strings referencing this system, such as `hkReflect::Type`, `hkReflect::FieldDecl`, `hkReflect::TypeReg`, `ReflectStrings`, and even `ReflectionInterface.dll`. This confirms that a complete, parallel reflection stack is present.

## Key Architectural Findings

#### 1. System Initialization via a Global Linked List

The `hkReflect` system, and likely other core engine systems, are not initialized via direct, hardcoded function calls. Instead, they are part of a master linked list of systems that are initialized at startup.

*   **Evidence:** A global pointer at `DAT_14293bba0` serves as the head of this list. A constructor function (`FUN_1415834d0`) adds new systems (like the global `ReflectStrings` object) to the front of this list. A separate function (`FUN_1415838f0`) implements a "move-to-front" heuristic, proving this list is actively managed at runtime for performance.
*   **Conclusion:** To find and use a system like the reflection engine, the game's code first gets the head of this list from the global pointer and iterates through it to find the system it needs by name.

#### 2. The Blueprint: A Continuous Block of `ClassInitializer` Data

The definitions for all reflected classes are not scattered throughout the executable. The C++ linker aggregates them into a few large, continuous arrays in the `.rdata` section.

*   **Evidence:** The most successful and complete dump of class data was achieved by treating the data from `0x142607840` to `0x14260BC60` as a single, contiguous array of class definition structures.
*   **Conclusion:** This array is the primary source for the raw class blueprints. A script can iterate through this block to dump every known class in the game.

#### 3. The `MemberInitializer`: Proven by Destructor Code

The layout of the structure describing each class member is not a guess. It is proven by the game's own code for the `ReflectStrings::~ReflectStrings` destructor.

*   **Evidence:** Helper functions used by the destructor iterate through an array of member data in **24-byte (0x18)** increments. Accessor functions within this code read two distinct 8-byte `longlong` values from offsets `+0x08` and `+0x10` within each 24-byte block.
*   **Conclusion:** This is undeniable proof that the `MemberInitializer` (internally likely a `FieldDecl`) is a 24-byte structure with three 8-byte fields.

## The Final, Evidence-Based Data Structures

Based on this forensic analysis, these are the definitive high-level structures for the reflection blueprint.

#### `ClassInitializer` (Size: 40 bytes / 0x28)

This structure serves as the header for a class definition.

| Offset | Data Type | Name | Description |
| :--- | :--- | :--- | :--- |
| `+0x00`| `const char*`| `classNamePtr` | A pointer to a null-terminated string with the class name. |
| `+0x08`| `const char*`| `parentClassNamePtr` | A pointer to the parent's class name string. |
| `+0x10`| `int` | `version` | The version number for this class definition. |
| `+0x14`| `int` | `flags` | Flags or the number of implemented interfaces. |
| `+0x18`| `MemberInitializer*`| `membersPtr` | A pointer to the start of the member definition array. |
| `+0x20`| `int` | `numMembers` | The number of members in the array. |

#### `MemberInitializer` / `FieldDecl` (Size: 24 bytes / 0x18)

This structure describes a single variable within a class.

| Offset | Data Type | Name | Description |
| :--- | :--- | :--- | :--- |
| `+0x00`| `longlong` | `signatureOrType` | A "tagged" value. Can be a small integer (a type ID) or a pointer. |
| `+0x08`| `const char*`| `namePtr` | A pointer to a null-terminated string with the member's name. |
| `+0x10`| `longlong` | `typeDataOrOffset` | A "tagged" value containing type data, flags, and/or the memory offset. |

## The Path Forward: Decoding the Blueprint

The mystery is no longer *where* the data is, but what the raw integer values in the `MemberInitializer` *mean*. The path to a full engine blueprint is now clear:

1.  **Find the "Reader" Code:** The next step is to locate the core engine code that reads these initializer structures. The most likely place is in a function that uses the global system list (`DAT_14293bba0`) to find the `ReflectStrings` or `TypeReg` system and begin processing the class data.
2.  **Decode the Tags:** By analyzing this "reader" code, we can reverse-engineer the `switch` statement or logic that interprets the `signatureOrType` and `typeDataOrOffset` fields. This will allow us to map these raw numbers to actual C++ types (like `int`, `float`, `pointer`) and their final memory offsets within the C++ objects.
3.  **Create the Final Dump:** With the tags decoded, the `KX_ReflectionDumper.py` script can be updated to produce perfect, human-readable C++ struct definitions for the entire game engine.

This document represents a stable, evidence-based foundation for any future research into the Guild Wars 2 engine internals.