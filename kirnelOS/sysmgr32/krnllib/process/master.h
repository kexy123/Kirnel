#pragma once

/// @brief A user process.
typedef struct Process Process;

#include "filesys/master.h"
#include "memmgr/paging/paging.h"
#include "thread/master.h"

struct __attribute__((packed)) Process
{
    /// @brief The ID of the process; 0 if invalid.
    unsigned long ProcessID;

    /// @brief The physical location of the process's page directory.
    PageDirectory *PageDirectory;

    /// @brief The main thread of the process.
    Thread *MainThread;
};

/// @brief Creates a process from the given executable and intializes resoources.
/// @param executable The file path of the executable.
/// @return The created process.
Process *create_process(FilePath executable);