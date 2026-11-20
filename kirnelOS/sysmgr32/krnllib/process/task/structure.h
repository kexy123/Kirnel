#pragma once

#include "process/thread/master.h"

/// @brief Specification of the [task state segment](https://wiki.osdev.org/Task_State_Segment#Protected_Mode).
typedef struct __attribute__((packed))
{
    unsigned long PreviousTaskSegmentSelector;
    
    /// @brief The stack pointer to go to for privilege level change to ring 0.
    void *ESP0;

    /// @brief The stack segment to use for privilege level change to ring 0.
    unsigned long SS0;

    /// @brief The stack pointer to go to for privilege level change to ring 1.
    void *ESP1;

    /// @brief The stack segment to use for privilege level change to ring 1.
    unsigned long SS1;

    /// @brief The stack pointer to go to for privilege level change to ring 2.
    void *ESP2;

    /// @brief The stack segment to use for privilege level change to ring 2.
    unsigned long SS2;

    unsigned long CR3;
    void *EIP;
    EFlags EFLAGS;

    unsigned long EAX, ECX, EDX, EBX;
    unsigned long ESP, EBP;
    unsigned long ESI, EDI;

    unsigned long ES, CS, SS, DS, FS, GS;
    unsigned long LDTR;

    unsigned DebugTrap : 1;
    unsigned : 15;
    unsigned short IOBitmapOffset;

    unsigned long SSP;
} TaskStateSegment;

/// @brief Information about the task state segment.
typedef struct {
    /// @brief The selector for the task state segment.
    const unsigned long Selector;

    /// @brief The location of the task state segment.
    TaskStateSegment const *TSS;
} TSSObject;

/// @brief Creates a task state segment in memory and appends it into the GDT.
/// @return The location of the task state segment.
TSSObject create_tss();

/// @brief Loads the task state segment.
/// @param object The task state segment to load.
__attribute__((__cdecl__)) extern void load_tss(TSSObject object);
