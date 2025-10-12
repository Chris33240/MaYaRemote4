/// @file TasksManager2.cpp
/// @brief Gestionnaire de tâches pour éviter une surcharge mémoire due au Bluetooth.
/// @details Cette classe permet de gérer une file de tâches JSON.
/// L’objectif est d’éviter les surcharges de pile BTC_TASK (stack overflow) lorsque les tâches
/// sont exécutées dans les threads Bluetooth en déléguant l'éxécutuon au gestionnaire de taches de maniere asynchrone.
/// Un mutex est utilisé pour se protéger des accès concurrent à la file de tâches
/// à l'intérieur des fonctions "add" et "update".
#include <mutex>
#include "TasksManager2.h"
#include "commandHandler.h"
#include "commands_tasks_json.h"
#include "system_info.h"

std::queue<std::string> tasks2;
std::mutex mtxTasks2; // Mutex pour protéger la variable `tasks` et sa fonction callback.

/// @brief Constructeur par défaut de TasksManager2.
TasksManager2::TasksManager2() {}

/// @brief Ajoute une tâche dans la file.
/// @param jsonTask Tâche au format JSON.
void TasksManager2::addTask(const std::string &jsonTask)
{
    std::lock_guard<std::mutex> lock(mtxTasks2); // Verrouille le mutex pour protéger la variable `tasks`.
    tasks2.push(jsonTask);
    // pGlobalStatusLED->startBlinkingForDuration(BLINK_LED_EXECUTE_INTERVAL, BLINK_LED_EXECUTE_DURATION);
    Serial.print(F("Added task: "));
    Serial.println(jsonTask.c_str());
}

/// @brief Exécute les tâches de la file dans l’ordre.
/// @details Chaque tâche est retirée après son exécution.
void TasksManager2::update()
{
    std::lock_guard<std::mutex> lock(mtxTasks2); // Verrouille le mutex pour protéger la variable `tasks`.
    //  Traiter les tâches dans l'ordre
    while (!tasks2.empty())
    {
        executeTask(tasks2.front());
        tasks2.pop(); // retirer la tache aprés son execution
    }
}

/// @brief Exécute une tâche donnée.
/// @param jsonTask Tâche en format JSON à exécuter.
void TasksManager2::executeTask(const std::string &jsonTask)
{
    Task2 task = deserializeTask2(jsonTask.c_str());
    std::string taskType = task.taskType;
    std::string commandName = task.commandName;

    bool hasDirectData = task.directData.hasData;
    std::string protocol = task.directData.protocolID;
    std::string dataCode = task.directData.dataCode;
    std::string bits = task.directData.bits;

    bool hasAdditionalPayload = task.additionalPayload.hasData;
    std::string frequency = task.additionalPayload.frequency;
    std::string toogleMask = task.additionalPayload.toggleMask;
    std::string repeat = task.additionalPayload.repeat;
    std::string raw = task.additionalPayload.raw;

    bool hasCommandData = task.commandData.hasData;
    // Serial.printf("Executing task: taskType: '%s' commandName: '%s'\r\n", taskType.c_str(), commandName.c_str());
    // std::string jsonTask = R"({"taskType":"EXECUTE","commandName":"IR_4_0xA90_1","directData":{"protocol":"3","data":"238"},"addPayload":{"frequency":"36","toggleMask":"0x0","repeat":"2"}})";
    Serial.printf("Executing jsonTask: '%s'\r\n", jsonTask.c_str());

    // std::string additionnalPayload = R"({"addPayload": {"frequency": "36", "toggleMask": "0x8000", "repeat": "", "raw": "0"}})";
    std::string additionnalPayload2 = serializeAdditionnalPayload2(task.additionalPayload, false).c_str();

    // boolean directSend = !protocol.empty() && !dataCode.empty();

    if (taskType == "EXECUTE")
    {
        if (!hasDirectData)
        {
            //--> CHARACTERISTIC_COMMAND2_UUID, --> CHARACTERISTIC_EXECUTE_COMMAND_UUID
            // Mode "NORMAL", execute une commande enregistrée.
            Serial.println("-- Mode: EXECUTE COMMAND");
            executeRegisteredCommand(commandName, additionnalPayload2);
        }
        else
        {
            // --> CHARACTERISTIC_COMMAND2_UUID
            // Mode DIRECT SEND, mode spécial pour l'envoide signaux IR sans données ni fichiers de commande prés-enregistrées.
            Serial.println("-- Mode: DIRECT SEND");
            if (protocol.empty() || dataCode.empty())
            {
                Serial.printf("unable to send IR with protocol:%s, dataCode: %s", protocol, dataCode);
                return;
            }
            commandData2 commandData;
            commandData = makeCommandData2(IR, "WRITE", "w", {protocol, "", "", bits.c_str(), "", "", "", "", "", "", "", dataCode.c_str()});
            executeUnregisteredCommand(commandData, additionnalPayload2);
        }
    }
    else if (taskType == "DELETE")
    {
        //--> CHARACTERISTIC_COMMAND2_UUID, --> CHARACTERISTIC_DELETE_COMMAND_UUID
        Serial.println("-- Mode: DELETE COMMAND");
        unregisterCommand(commandName);
        // deleteCommandFile2(commandName);
    }
    else if (taskType == "ADDING" && hasCommandData)
    {
        //--> CHARACTERISTIC_COMMAND2_UUID
        Serial.println("-- Mode: ADDING COMMAND");
        registerCommand(task.commandName, task.commandData, true, false);
    }
}