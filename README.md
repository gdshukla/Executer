# Executer
A simple task executer. Task commands can be specified in a json file and given as input to the program.

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
