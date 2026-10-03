#pragma once

/// @brief The segment size type.
typedef enum
{
    /// @brief The segment is in 16-bit protected mode.
    Bits16,

    /// @brief The segment is in 32-bit protected mode.
    Bits32,

    /// @brief The segment is in 64-bit long mode.
    Bits64
} SegmentSize;

/// @brief The attributes that define a segment.
typedef union
{
    struct __attribute__((packed))
    {
        /// @brief The CPU accessed this segment if set.
        unsigned Accessed : 1;

        /// @brief Determines if the code segment can be read. Code segments cannot be written to.
        unsigned Readable : 1;

        /// @brief Determines if the CPU's higher privilege level can run code at this segment with a lower privilege level.
        unsigned Conforming : 1;

        /// @brief Determines if the segment is a code segment (set) or a data segment (clear).
        unsigned Executable : 1;

        /// @brief Determines if the segment is a special system segment (clear) or a code/data segment (set).
        unsigned System : 1;

        /// @brief The CPU privilege level of this segment.
        unsigned PrivilegeLevel : 2;

        /// @brief Must be set in order to use this segment.
        unsigned Present : 1;
    };

    struct __attribute__((packed))
    {
        unsigned : 1;

        /// @brief Determines if the data segment can be written to; otherwise it is read-only.
        unsigned Writeable : 1;

        /// @brief When clear the data segment grows up; otherwise down, which can be used for stacks.
        unsigned Direction : 1;
    };

    /// @brief The raw bits of the attributes.
    unsigned char Raw;
} SegmentAttributes;

/// @brief A segment entry.
typedef struct __attribute__((packed))
{
    /// @brief The lower limit bits that determine the end of this segment. Note that it will be scaled if the granularity is set.
    unsigned short LimitLow;

    /// @brief The lower base bits that determine the start of this segment.
    unsigned BaseLow : 24;

    /// @brief The attributes of this segment entry.
    SegmentAttributes Attributes;

    /// @brief The higher limit bits that determine the end of this segment. Note that it will be scaled if the granularity is set.
    unsigned LimitHigh : 4;

    /// @brief Reserved; always zero.
    unsigned Reserved : 1;

    /// @brief Determines if this segment is a long-mode segment or not. Note that the DescriptorSize must be clear if this is set.
    unsigned LongMode : 1;

    /// @brief Determines if this segment is a 32-bit segment or not.
    unsigned DescriptorSize : 1;

    /// @brief Determines the scale of the limit. When set, the limit is scaled to the page size (4 KiB); otherwise it is in bytes.
    unsigned Granularity : 1;

    /// @brief The higher base bits that determine the start of this segment.
    unsigned char BaseHigh;
} SegmentEntry;

/// @brief The descriptor table. Can have up to 4096 segment entries.
typedef SegmentEntry DescriptorTable[4096];

/// @brief The descriptor that defines the metadata of the global descriptor table.
typedef struct __attribute__((packed))
{
    /// @brief The number of bytes in the descriptor table minus one.
    unsigned short Length;

    /// @brief The starting segment entry in the descriptor table. Note that the starting segment must be a null segment.
    DescriptorTable *SegmentStart;
} TableDescriptorRegister;

/// @brief Generates a descriptor. Should only be initiated once.
void generate_descriptor();

/// @brief Adds a descriptor segment to the table.
/// @param index The location to add the segment at.
/// @param base The starting byte address of the segment.
/// @param limit The ending byte/page address of the segment, which depends on the segment's attributes.
/// @param attributes The segment attributes.
/// @param granular The segment's address is in pages instead of bytes.
/// @param size The segment size type.
void add_segment(unsigned short index, unsigned long base, unsigned long limit, SegmentAttributes attributes, _Bool granular, SegmentSize size);

/// Loads the recently generated global descriptor table to the CPU.
void init_gdt();