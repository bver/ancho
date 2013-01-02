#pragma once

#include <deque>
#include "Exceptions.h"


/*============================================================================
 * Abstract command
 */
class ACommand
{
public:
#if _HAS_CPP0X
  typedef std::shared_ptr<ACommand> Ptr;
#else
  typedef std::tr1::shared_ptr<ACommand> Ptr;
#endif

  virtual ~ACommand(){};
  virtual void execute() = 0;
};


/*============================================================================
 * Command queue - method calls not synchronized yet
 */
class CommandQueue 
{
public:
  CommandQueue(bool aAutoExec = false): mAutoExec(aAutoExec)
  {}

  void addCommand(ACommand::Ptr aCommand)
  {
    if (!aCommand) {
      throw EInvalidPointer();
    }
    if (mAutoExec) {
      aCommand->execute();
    } else {
      mCommands.push_back(aCommand);
    }

  }

  void autoExecuteAll() {
    setAutoExec(true);
    executeAll();
  }

  void executeAll()
  {
    while(!mCommands.empty()) {
      mCommands.front()->execute();
      mCommands.pop_front();
    }
  }

  void executeFirst()
  {
    if (mCommands.empty()) {
      throw std::runtime_error("Command queue is empty");
    }
    mCommands.front()->execute();
    mCommands.pop_front();
  }

  void executeLast()
  {
    if (mCommands.empty()) {
      throw std::runtime_error("Command queue is empty");
    }
    mCommands.back()->execute();
    mCommands.pop_back();
  }

  size_t size()const
  {
    return mCommands.size();
  }

  void clear()
  {
    mCommands.clear();
  }

  void setAutoExec(bool aAutoExec)
  {
    mAutoExec = aAutoExec;
  }
protected:
  typedef std::deque<ACommand::Ptr> Container;
  Container mCommands;
  bool mAutoExec;
};