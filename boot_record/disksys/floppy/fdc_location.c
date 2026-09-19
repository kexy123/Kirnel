#include "fdc_location.h"
#include "disksys/structure.h"

void lba_to_chs(LogicalBlockAddress address, unsigned char *cylinder, unsigned char *head, unsigned char *sector)
{
    *cylinder = address / (2 * SECTORS_PER_CYLINDER);
    unsigned char remainder = address % (2 * SECTORS_PER_CYLINDER);

    *head = remainder / SECTORS_PER_CYLINDER;

    *sector = remainder % SECTORS_PER_CYLINDER + 1; // Sectors are 1-indexed.
}