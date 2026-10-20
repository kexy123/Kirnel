#pragma once

/// @brief A thread that contains an execution context and state.
typedef struct Thread Thread;

#include "process/master.h"

/// @brief The [EFLAGS](https://wiki.osdev.org/CPU_Registers_x86#EFLAGS_Register).
typedef struct __attribute__((packed))
{
    unsigned CarryFlag : 1;

    unsigned : 1;

    unsigned ParityFlag : 1;

    unsigned : 1;

    unsigned ZeroFlag : 1;

    unsigned : 1;

    unsigned SignFlag : 1;

    unsigned : 1;

    unsigned Trap : 1;

    unsigned InterruptEnabledFlag : 1;

    unsigned DirectionFlag : 1;

    unsigned OverflowFlag : 1;

    unsigned PrivilegeLevel : 2;

    unsigned NestedTaskFlag : 1;

    unsigned : 1;

    unsigned ResumeFlag : 1;

    unsigned Virtual8086ModeFlag : 1;

    unsigned AlignmentCheck : 1;

    unsigned VirtualInterruptFlag : 1;

    unsigned VirtualInterruptPending : 1;

    unsigned CanUseCPUIDInstruction : 1;

    unsigned : 10;
} EFlags;

/// @brief All the necessary registers to store a CPU context.
typedef struct
{
    // Move these manually.

    /// @brief A segment register.
    unsigned short DS, ES, FS, GS;

    // In order of the POPAD instruction: https://intel.github.io/SDM/instruction/POPA-POPAD.html

    /// @brief A pointer register.
    unsigned long EDI, ESI;

    /// @brief The stack function base pointer.
    void *EBP;

    /// @brief Unused.
    void *Unused;

    /// @brief A general CPU register.
    unsigned long EBX, EDX, ECX, EAX;

    // In order of the IRETD instruction: https://intel.github.io/SDM/instruction/IRET-IRETD-IRETQ.html

    /// @brief The instruction pointer.
    void *EIP;

    /// @brief The code segment selector.
    unsigned short CS;

    /// @brief The execution flags.
    EFlags EFLAGS;

    /// @brief The stack pointer.
    void *ESP;

    /// @brief The stack segment selector.
    unsigned short SS;
} CPUContext;

typedef enum : unsigned char
{
    /// @brief The thread cannot run. Note that either one of the first and last bits indicates that the thread cannot run.
    THREAD_STATE_NOT_RUNNABLE = 0x81,

    /// @brief The thread was terminated.
    THREAD_STATE_NOT_ALIVE = 0x80,

    /// @brief The thread can be executed.
    THREAD_STATE_READY = 0x00,

    /// @brief The thread is currently being executed by the CPU.
    THREAD_STATE_RUNNING = 0x01,

    /// @brief The thread is currently sleeping and should wake up upon a dedicated wake-up time or from another thread.
    THREAD_STATE_SLEEP = 0x03,

    /// @brief The thread aborted and/or finished and should be reused or cleaned up.
    THREAD_STATE_TERMINATED = 0xFF,
} ThreadState;

struct __attribute__((packed)) Thread
{
    /// @brief The process that the thread is in.
    Process *Master;

    /// @brief The state of the CPU in that thread.
    CPUContext Context;

    /// @brief The current state of the thread.
    ThreadState State;

    /// @brief The previous thread of the same dead/alive state.
    Thread *Previous;

    /// @brief The next thread of the same dead/alive state.
    Thread *Next;
};

/// @brief Creates a runnable thread for the given process at the starting instruction and stack frame.
/// @param parentProcess The process this thread is attached to.
/// @param atInstruction The starting instruction location of the thread.
/// @param stack The stack frame for the thread.
/// @return The new Thread object.
Thread *create_thread(Process *parentProcess, void *atInstruction, void *stack);

/// @brief Runs the given thread.
/// @param thread The thread to execute.
void run_thread(Thread *thread);