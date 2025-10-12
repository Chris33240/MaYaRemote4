#include <Arduino.h>
#include "commandHandler.h"
//#include "interfaces/fileInfo.h"
#include "interfaces/hardwarePresenter.h"

void updateCommandsFromFiles(std::map<std::string, commandData2> &commands, int &errorCount);

String makeFileCommandName(std::string commandName);
std::pair<std::string, commandData2> loadCommand(const std::string &commandName);
std::pair<std::string, commandData2> loadCommand3(const String &path);
bool isValidCommand(const std::pair<std::string, commandData2> &pair);

void saveCommand(const std::string commandName, commandData2 command);
void saveCommand2(const std::string commandName, commandData2 command, const String &path);
void deleteCommandFile(std::map<std::string, commandData2> &commands, const std::string commandName);

void loadCommandFiles(std::map<std::string, commandData2> &commands, int &errorCount, const String &dirname = "/", uint8_t levels = 0);
void loadCommandsKeysFromFiles(std::set<std::string> &commandsKeys, int &errorCount, const String &dirname = "/", uint8_t levels = 0);
void deleteAllFiles(int &errorCount, const String &dirname = "/", uint8_t levels = 0);
void deleteFile(const FileInfo &fileInfo, int &errorCount);
void printFilesInfo(const String &dirname = "/", uint8_t levels = 0);
void printFileInfo(const FileInfo &fileInfo);
[[deprecated("Don't use this routine any more. Use the new one instead. 'printFilesInfo()'")]]
void listDir2(const String &dirname, uint8_t levels, FileCallback action);
//void updateCommandFiles(int &errorCount, const String &dirname, uint8_t levels);