#pragma once

/// @brief The attributes that change depending on whether the segment is a data or code segment.
typedef union
{
    struct __attribute__((packed))
    {
        /// @brief Determines if the code segment can be read. Code segments cannot be written to.
        unsigned Readable : 1;

        /// @brief Determines if the CPU's higher privilege level can run code at this segment with a lower privilege level.
        unsigned Conforming : 1;
    };

    struct __attribute__((packed))
    {
        /// @brief Determines if the data segment can be written to; otherwise it is read-only.
        unsigned Writeable : 1;

        /// @brief When clear the segment grows up; otherwise down, which can be used for stacks.
        unsigned Direction : 1;
    };
} SegmentTypeAttributes;

/// @brief The attributes that define a segment.
typedef union
{
    struct __attribute__((packed))
    {
        /// @brief The CPU accessed this segment if set.
        unsigned Accessed : 1;

        /// @brief The attributes of the data segment.
        SegmentTypeAttributes SegmentAttributes;

        /// @brief Determines if the segment is a code segment (set) or a data segment (clear).
        unsigned Executable : 1;

        /// @brief Determines if the segment is a special system segment (clear) or a code/data segment (set).
        unsigned System : 1;

        /// @brief The CPU privilege level of this segment.
        unsigned PrivilegeLevel : 2;

        /// @brief Must be set in order to use this segment.
        unsigned Present : 1;
    };

    /// @brief The raw bits of the attributes.
    unsigned char Raw;
} SegmentAttributes;

/// @brief The flags that define a segment.
typedef union
{
    struct __attribute__((packed))
    {
        /// @brief Reserved; always zero.
        unsigned Reserved : 1;

        /// @brief Determines if this segment is a long-mode segment or not. Note that the DescriptorSize must be clear if this is set.
        unsigned LongMode : 1;

        /// @brief Determines if this segment is a 16-bit segment or not.
        unsigned DesciptorSize : 1;

        /// @brief Determines the scale of the limit. When set, the limit is scaled to the page size (4 KiB); otherwise it is in bytes.
        unsigned Granularity : 1;
    };

    /// @brief The raw bits of the flags.
    unsigned Raw : 4;
} SegmentFlags;

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

    /// @brief The flags of this segment entry.
    SegmentFlags Flags;

    /// @brief The higher base bits that determine the start of this segment.
    unsigned char BaseHigh;
} SegmentEntry;

/// @brief The descriptor that defines the metadata of the global descriptor table.
typedef struct __attribute__((packed))
{
    /// @brief The number of entries in the descriptor table.
    unsigned short Length;

    /// @brief The starting segment entry in the descriptor table. Note that the starting segment must be a null segment.
    SegmentEntry *SegmentStart;
} TableDescriptorRegister;

/// @brief Generates a descriptor.
void generate_descriptor();

/// @brief Appends a descriptor segment to the table.
/// @param base The starting byte address of the segment.
/// @param limit The ending byte/page address of the segment, which depends on the segment's attributes.
/// @param attributes The segment attributes.
/// @param flags The segment flags.
void append_segment(unsigned long base, unsigned long limit, SegmentAttributes *attributes, SegmentFlags *flags);

/// Loads the recently generated global descriptor table to the CPU.
void init_gdt();