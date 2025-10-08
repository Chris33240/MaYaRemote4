#include <Arduino.h>
#include <IRsend.h>

void sendRC6Toggle(IRsend IrSender, uint64_t command, short nbits, uint32_t toggle_bit_mask);
void sendRC6Toggle(IRsend IrSender, uint64_t command, short nbits, uint32_t toggle_bit_mask, int toggle);