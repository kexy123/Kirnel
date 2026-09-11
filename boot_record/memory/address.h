/// @brief Yields the pointer's address in segment:offset form to store a 20-bit memory address.
/// @param objPtr The pointer of the object.
/// @param segment The returned segment.
/// @param offset The returned offset.
void ptr_to_seg_off(const void *objPtr, unsigned short *segment, unsigned short *offset);