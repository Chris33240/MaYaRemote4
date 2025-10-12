#pragma once

#include <string>
#include <list>

void init_infraredSender_HAL(void);
void sendIRcode_HAL(int protocol, std::list<std::string> commandPayloads, std::string additionalPayload);

namespace nsIR
{
    void sendIRcode_HAL2(std::list<std::string> commandPayloads, std::string additionalPayload);

    // Fonctions de tests :
    //[[deprecated("Don't use this routine any more. Use the new one instead. 'sendSony()'")]]
    void sendSonyR2_HAL(const std::list<std::string> &commandPayloads);
    //[[deprecated("Don't use this routine any more. Use the new one instead. 'sendRC6()'")]]
    void sendMceT_HAL(const std::list<std::string> &commandPayloads);
}
