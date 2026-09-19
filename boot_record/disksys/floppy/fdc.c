#include "fdc.h"
#include "interrupt_desc_table/handle/fdc_interrupts.h"
#include "utils/flow.h"
#include "utils/portcall.h"

/// @brief Waits until the floppy disk controller has completed its operation and raised an IRQ6.
void yield_fdc_finish()
{
    while (!fdc_op_complete)
    {
        halt();
    }

    fdc_op_complete = 0;
}

// FDC INITIALIZATION

/// @brief Resets the floppy disk controller's configurations.
void reset_fdc()
{
    // https://wiki.osdev.org/Floppy_Disk_Controller#DOR_bitflag_definitions
    port_call(DIGITAL_OUTPUT_REGISTER, 0b00000000); // Enter reset mode.
    port_call(DIGITAL_OUTPUT_REGISTER, 0b00011100); // Enable drive 0 and the IRQ and DMA.
    yield_fdc_finish();

    // Clear the FDC results.
    for (int i = 0; i < 4; i++)
    {
        send_fdc_command(DATA_FIFO, 0x08); // Acknowledge interrupt.

        port_read(DATA_FIFO);
        port_read(DATA_FIFO);
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
    yield_fdc_finish();

    send_fdc_command(DATA_FIFO, 0x08); // Acknowledge interrupt.

    unsigned char status = port_read(DATA_FIFO);
    unsigned char presentCylinder = port_read(DATA_FIFO);

    if (presentCylinder != 0)
    {
        // Calibration failed; UNDEFINED BEHAVIOR!
        while (1)
            ;
    }
}

void init_fdc()
{
    reset_fdc();
    specify_fdc();
    calibrate_fdc();
}