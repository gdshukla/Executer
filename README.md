# Executer
A simple task executer. Task commands can be specified in a json file and given as input to the program.

Task Scheduler runs a threadpool of threads equal to number of cores available on system. Thread count is equal to `std::thread::hardware_concurrency()` value of the system. When a task is added scheduler automatically starts it immediately. If there are more taks than number of cores, shceduler waits for a task to finish before starting a new task. 

Threads are created only once during startup and same threads are resued to launch different tasks.

# Creating a new command task
Command task class must be inherited from ICommand interface.
```
class my_task: public ICommand
{
  ...
}
```

## Adding functionality to command task

### Constructor
It should initialize `ICommand` interface with name of the command. This name is used to identify the task in `commands.json` file
```
my_task(): ICommand("my_task"), ... {}
```

### JSON Argument parser
It must implement `parseArgs` member to parse arguments from `commands.json` file and to initialize class member variables.
*TODO:: Add error checking in case invalid arguments are supplied*
```
bool parseArgs(nlohmann::json& j) {
  m_start = j.contains("start") ? j["start"].get<int>() : 0;
  ...
  return true;
}
```
### Command worker
It must implement `operator()()` function which contains the actual command implementation. Currently its return type is limited to integer only.
Before returning save result to `Task::m_result` member.
*TODO: Add support for generic return types*
```
int operator()() {
  int result = 0;
  // command implementation
  Task::m_result = result;
  return result;
}
```
## Adding command task to program
JSON parsing and command object initialization is done in function `loadCommands` in `Executer.cpp` file.
Below last `else if` block, add code to load and initialize command task
```
  ...
  else if(name == "my_task")
  {
    cmd = std::make_unique<my_task>();
  }
  ...
```

## Commands available
- help or h: Display help text
- stats or s: display overall task stats
- load or l: load new json file to add tasks to run
```
>>>load commands.json
```
- display or d: display stats about a certain task
```
>>>d 1
Status: 1
Result: 3628800
Started at: 00:02:08.466
Started at: 00:02:09.371
```
- quit or q: quit the program

# How to run the program
Run executer.exe from terminal. Additionally json file containing command details can be passed as argument
```
>executer commands.json
```
Once program starts, it displays a `>>>` prompt to run in-program commands

# Class details:
## Queue (queue.hpp)
Implements a mutex protected queue. This is used to hold the tasks

## Stats (stats.hpp)
Contains overall task statistics. It is implemented as a singleton.

## IArguments (arguments.h)
Json argument interface. It is an abstract class

## Scheduler (scheduler.cpp/hpp)
Scheduler implemented as singleton. It implements a threadpool of 4 threads. 

## ITask/basic_tasl/Task (task.hpp)
ITask is task interface.
basic_task inherits from ITask and implements some basic task functionality
Task inherits from basic_task and implements functionality to setup `std::function<int(void)>` as worker task

## utilities.hpp
- print: Mutex protected print function to display strings on console
- toTime: Converts system_clock::time_point to hh:mm:ss.mmm format

## commands.h
Implements 2 simple example commands - adder and factorial
- adder: adds numbers starting deom M-start till m_end. To simulate long running command, it add m_delay milliseconds delay between each iteration.
- factorial: calculates factorial of m_num number. To simulate long running command, it add m_delay milliseconds delay between each iteration.
