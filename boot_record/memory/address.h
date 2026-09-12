/// @brief Yields the near pointer's address in segment:offset form to store a 20-bit memory address.
/// @param objPtr The pointer of the object.
/// @param segment The returned segment.
/// @param offset The returned offset.
void nearptr_to_seg_off(const void *objPtr, unsigned short *segment, unsigned short *offset);