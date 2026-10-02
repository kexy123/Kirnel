#include "convert/int.h"
#include "out.h"

#define VGA_START ((volatile VGACharacter *)(0x000B8000)) // The pointer of the starting character of the VGA.
#define VGA_WIDTH (80)                                    // The VGA width.
#define VGA_HEIGHT (25)                                   // The VGA height.

/// @brief The current position of where to output text in the VGA.
volatile VGACharacter *vgaBuffer = VGA_START;

/// @brief Executes a carraige return.
void char_cr()
{
    vgaBuffer -= (vgaBuffer - VGA_START) % VGA_WIDTH;
}

/// @brief Executes a line feed.
void char_lf()
{
    for (int i = 0; i < VGA_WIDTH; i++)
    {
        vgaBuffer->Character = ' ';
        vgaBuffer++;
    }
}

void print_c(char character)
{
    switch (character)
    {
    case '\r':
        char_cr();
        break;
    case '\n':
        char_lf();
        break;
    default:
        vgaBuffer->Color = 0x0F;
        vgaBuffer->Character = character;
        vgaBuffer++;
        break;
    }
}

void print_s(const char *message)
{
    while (*message != '\0')
    {
        print_c(*message);

        // Move the message pointer.
        message++;
    }
}

void print_uint(unsigned long num)
{
    const char *result = uint_to_str(num);
    print_s(result);
}

void print_uintx(unsigned long num)
{
    const char *result = uint_to_strx(num, 0);
    print_s("0x");
    print_s(result);
}

void print_bool(_Bool boolean)
{
    if (boolean == 0)
    {
        print_s("False");
    }
    else
    {
        print_s("True");
    }
}

void print_hex(void *source, unsigned long numBytes)
{
    unsigned char *sourcePtr = (char *)source;

    for (unsigned long i = 0; i < numBytes; i++)
    {
        const char *result = uint_to_strx(*sourcePtr, 2);
        print_s(result);
        print_c(' ');

        sourcePtr++;
    }
}

void print_newl()
{
    print_c('\n');
    print_c('\r');
}

void clear_screen()
{
    vgaBuffer = VGA_START;
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        vgaBuffer[i].Character = ' ';
    }
}