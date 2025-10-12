#include <string>
#include <queue>

#ifndef TASKSMANAGER2_H
#define TASKSMANAGER2_H

struct DirectData
{
  bool isEnabled;
  std::string protocolID;
  std::string dataCode;
  std::string bits;
};

struct Payload
{
  bool isEnabled;
  std::string frequency;
  std::string toggleMask;
  std::string repeat;
  std::string raw;
};

struct Task2
{
  std::string taskType;
  std::string commandName;
  DirectData directData;
  Payload payload;
};

class TasksManager2
{
public:
  TasksManager2();
  static void addTask(const std::string &jsonTask);
  static void update();
  static void executeTask(const std::string &jsonTask);

private:

};

#endif // TASKSMANAGER2_H