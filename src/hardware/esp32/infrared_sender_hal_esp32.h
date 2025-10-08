#pragma once

#include <string>
#include <list>

void init_infraredSender_HAL(void);
void sendIRcode_HAL(int protocol, std::list<std::string> commandPayloads, std::string additionalPayload);

namespace nsIR
{
    void sendIRcode_HAL3(int protocol, std::list<std::string> commandPayloads, std::string additionalPayload);
    void sendRC6Toggle(uint64_t command, short frequency, int toggle_bit_mask);
    void sendRC6Toggle(uint64_t command, short frequency, int toggle_bit_mask, int toggle);

    // Fonctions de tests :
    [[deprecated("Don't use this routine any more. Use the new one instead. 'sendSony()'")]]
    void sendSonyR2(const std::list<std::string> &commandPayloads);
    [[deprecated("Don't use this routine any more. Use the new one instead. 'sendRC6()'")]]
    void sendMceT(const std::list<std::string> &commandPayloads);
}
