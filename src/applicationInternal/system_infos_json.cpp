#include "system_infos_json.h"

// ------------------ Serialiser Commands pour systemInfos -----------------------

/// @brief Sérialise une commande en chaîne JSON.
/// @param systemInfos Données a serializer.
/// @param JsonPretty Indique si la sortie doit être formatée joliment.
/// @return Chaîne JSON représentant la commande.
String serializeSystemInfos(const systemInfos &systemInfos, bool JsonPretty)
{
    String output;
    JsonDocument doc;
    auto root = doc.to<JsonObject>();

    to_json(root, systemInfos);

    if (JsonPretty)
    {
        serializeJsonPretty(doc, output);
    }
    else
    {
        serializeJson(doc, output);
    }
    return output;
}

void to_json(JsonObject &root, const systemInfos &systemInfos)
{
    root["chipModel"]        = systemInfos.chipModel;
    root["chipRev"]          = systemInfos.chipRev;
    root["cpuDefFreq"]       = systemInfos.cpuDefFreq;
    root["cpuFreq"]          = systemInfos.cpuFreq;
    root["chipCores"]        = systemInfos.chipCores;

    root["freeHeap"]         = systemInfos.freeHeap;
    root["totalHeap"]        = systemInfos.totalHeap;
    root["minFreeHeap"]      = systemInfos.minFreeHeap;
    root["maxAllocHeap"]     = systemInfos.maxAllocHeap;
    root["stack"]            = systemInfos.stack;

    root["psramSize"]        = systemInfos.psramSize;
    root["freePsram"]        = systemInfos.freePsram;
    root["minFreePsram"]     = systemInfos.minFreePsram;
    root["maxAllocPsram"]    = systemInfos.maxAllocPsram;

    root["flashSize"]        = systemInfos.flashSize;
    root["flashSpeed"]       = systemInfos.flashSpeed;
    root["flashSectorSize"]  = systemInfos.flashSectorSize;
    root["flashMode"]        = systemInfos.flashMode;

    root["sketchSize"]       = systemInfos.sketchSize;
    root["freeSketchSpace"]  = systemInfos.freeSketchSpace;

    root["fsUsed"]           = systemInfos.fsUsed;
    root["fsTotal"]          = systemInfos.fsTotal;
}
