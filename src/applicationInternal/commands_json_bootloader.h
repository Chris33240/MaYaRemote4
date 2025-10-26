#include <Arduino.h>

/* Dans cet exemple, nous utilisons la syntaxe R"( )" pour définir une chaîne de caractères brute (raw string literal) en C++,
ce qui nous permet d'inclure des sauts de ligne et des guillemets sans avoir à les échapper

    // Données JSON stockées directement dans le code source
    std::string json_string = R"(
        {
            "nom": "John Doe",
            "age": 30,
            "ville": "Paris"
        }
    )";

    // Parsing de la chaîne JSON
    //json data = json::parse(json_string);
*/

/*
Le fichier commmands.json créer à partir de la chaine "commands_json" ci-aprés contient toutes les commandes
pouvant être exécutées par un processus dédié à l'exécution de ces commandes.
Certaines de commandes sont utilisé par le programme et ne peuvent pas être effacées,
ce comportement est déterminé principalement pour certaines valeurs du paramètre "commandHandler". 
*/

// const char json_string[] PROGMEM = R"(
// {
//     "nom": "John Doe",
//     "age": 30,
//     "ville": "Paris"
// }
// )";

// Be carfull : Command Key is sorted by ascendencing in map object 'commands2'
const String commands_json_bootloader = R"(
{
    "LIST_COMMANDS": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "LISTDIR_COMMANDS": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "TEST_NOTIFY": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "DELETE_ALL_FILES_CMD": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "IR_CAPTURE": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    }
}
)";

/*
const String commands_json_bootloader = R"(
{
    "LIST_COMMANDS_R": {
    "commandHandler": "0",
    "requestType": "READ",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "LIST_COMMANDS": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "LISTDIR_COMMANDS": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "FREE_HEAP": {
    "commandHandler": "0",
    "requestType": "READ",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "HIGH_WATER_MARK": {
    "commandHandler": "0",
    "requestType": "READ",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "TICK_RATE": {
    "commandHandler": "0",
    "requestType": "READ",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "FS_USED_BYTES": {
    "commandHandler": "0",
    "requestType": "READ",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "FS_TOTAL_BYTES": {
    "commandHandler": "0",
    "requestType": "READ",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "TEST_NOTIFY": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "DELETE_ALL_FILES_CMD": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "IR_CAPTURE": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
    ]
    },
    "SPECIALE_1": {
    "commandHandler": "0",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
        "payload1",
        "payload2",
        "payload3"
    ]
    },
    "SCENE_2": {
    "commandHandler": "1",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
        "payload4",
        "payload5"
    ]
    },
    "GUI_3": {
    "commandHandler": "3",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
        "payload6"
    ]
    },
    "MQTT_4": {
    "commandHandler": "4",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
        "payload7",
        "payload8"
    ]
    },
    "BLEKB_5": {
    "commandHandler": "5",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
        "payload9"
    ]
    },
    "IR_4_0xA90": {
    "commandHandler": "3",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
        "4",
        "false",
        "0xA90",
        "12",
        "105",
        "false",
        "2388, 590,  1198, 590,  600, 590,  1200, 588,  602, 590,  1198, 590,  602, 590,  602, 590,  1198, 590,  602, 590,  600, 592,  602, 590,  602, 25698,  2390, 590,  1198, 590,  602, 592,  1198, 590,  602, 592,  1196, 594,  598, 592,  600, 590,  1198, 590,  602, 590,  600, 590,  602, 590,  602, 25684,  2390, 588,  1198, 592,  600, 592, 1198, 590,  600, 590,  1198, 588,  602, 590,  602, 590,  1198, 590,  602, 590,  602, 588,  602, 588,  602, 25694,  2392, 162,  186, 122,  1312, 592,  596, 596,  1172, 614,  600, 594,  1174, 614,  600, 592,  598, 592,  1194, 594,  576, 618,  596, 596,  572, 620,  572",
        "",
        "0x90, 0x0A",
        "0x1",
        "0x15",
        "0xA90"
    ]
    },
    "IR_2_0xC800F040C": {
    "commandHandler": "3",
    "requestType": "WRITE",
    "attributs": "",
    "commandPayloads": [
        "2",
        "false",
        "0xC800F040C",
        "36",
        "67",
        "false",
        "2700, 850, 450, 450, 450, 450, 450, 850, 450, 900, 1350, 850, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 400, 450, 450, 450, 450, 900, 450, 450, 450, 450, 400, 450, 900, 450, 450, 450, 450, 450, 450, 450, 450, 900, 850, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 900, 450, 400, 950, 400, 450, 450",
        "",
        "0x0C, 0x04, 0x0F, 0x80, 0x0C",
        "0xC800F04",
        "0xC",
        "0xC800F040C"
    ]
    }
}
)";
*/

