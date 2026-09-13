#include "out.h"

#define VGA_START ((VGACharacter *)(0xB8000)) // The pointer of the starting character of the VGA.
#define VGA_WIDTH (80)                        // The VGA width.
#define VGA_HEIGHT (25)                       // The VGA height.

/// @brief The current position of where to output text in the VGA.
VGACharacter *vgaBuffer = VGA_START;

void print_s(const char *message)
{
    while (*message != '\0')
    {
        vgaBuffer->Color = 0x0F;
        vgaBuffer->Character = *message;

        // Move the vgaBuffer and message.
        vgaBuffer++;
        message++;
    }
}

void print_c(char character)
{
    vgaBuffer->Color = 0x0F;
    vgaBuffer->Character = character;
    vgaBuffer++;
}

void print_uint(unsigned short num)
{
    char result[5];

    // Base-10 conversion.
    short i = 0;
    do
    {
        result[i] = '0' + (num % 10);
        num /= 10;
        i++;
    } while (num > 0);

    // Print the digits in reverse order.
    while (i > 0)
    {
        i--;
        print_c(result[i]);
    }
}

void print_newl()
{
    unsigned short numSpaces = VGA_WIDTH - (vgaBuffer - VGA_START) % VGA_WIDTH;
    for (unsigned short i = 0; i < numSpaces; i++)
    {
        print_c(' ');
    }
}