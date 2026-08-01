#include <SPIFFS.h>
#include "setup.h"

#ifdef FORMAT_FILE_SYSTEM_IF_FAILED
// Le define spécifique au système de fichier n'est plus utilisé
// #define FORMAT_SPIFFS_SYSTEM_IF_FAILED FORMAT_FILE_SYSTEM_IF_FAILED
#endif

namespace nsSPIFFS
{
    void format();
    bool mount();
    void unMount();
    void printFreeSpace();
    unsigned int getUsedBytes();
    unsigned int getTotalBytes();
}