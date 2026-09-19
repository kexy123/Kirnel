#include "fdc.h"
#include "interrupt_desc_table/handle/fdc_interrupts.h"
#include "utils/flow.h"
#include "utils/portcall.h"
#include "vga/out.h"

void yield_fdc_finish(_Bool senseInterrupt)
{
    while (!fdc_op_complete)
    {
        halt();
    }

    fdc_op_complete = 0;

    if (senseInterrupt)
    {
        send_fdc_command(DATA_FIFO, 0x08); // Sense interrupt command.
    }
}

// FDC INITIALIZATION

/// @brief Resets the floppy disk controller's configurations.
void reset_fdc()
{
    // https://wiki.osdev.org/Floppy_Disk_Controller#DOR_bitflag_definitions
    port_call(DIGITAL_OUTPUT_REGISTER, 0b00000000); // Enter reset mode.
    port_call(DIGITAL_OUTPUT_REGISTER, 0b00011100); // Enable the IRQ and DMA.
    yield_fdc_finish(0);

    // Clear the FDC results of each drive from 0 to 3.
    for (int i = 0; i < 4; i++)
    {
        send_fdc_command(DATA_FIFO, 0x08); // Acknowledge interrupt.

        read_fdc_command(DATA_FIFO); // The status of the drive.
        read_fdc_command(DATA_FIFO); // The present cylinder of the drive.
    }
}

/// @brief Initializes part of the floppy disk controller to allow interrupt requests and the DMA.
void specify_fdc()
{
    // https://wiki.osdev.org/Floppy_Disk_Controller#Specify
    send_fdc_command(DATA_FIFO, 0x03); // Specify command.
    send_fdc_command(DATA_FIFO, 0b11011111);
    send_fdc_command(DATA_FIFO, 0x02);
}

/// @brief Calibrates the floppy disk controller's by their cylinder.
void calibrate_fdc()
{
    send_fdc_command(DATA_FIFO, 0x07); // Calibrate command.
    send_fdc_command(DATA_FIFO, 0x00); // Drive 0.
    yield_fdc_finish(1);

    unsigned char status = read_fdc_command(DATA_FIFO);
    unsigned char presentCylinder = read_fdc_command(DATA_FIFO);

    if (presentCylinder != 0)
    {
        // Calibration failed; TODO: undefined behavior.
        while (1)
            ;
    }
}

void init_fdc()
{
    reset_fdc();
    print_ln("FDC RESET. . .");
    specify_fdc();
    print_ln("FDC SPECIFIED. . .");
    calibrate_fdc();
    print_ln("FDC CALIBRATED. . .");
}