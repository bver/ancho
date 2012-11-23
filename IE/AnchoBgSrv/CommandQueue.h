#pragma once

#include <atlcomcli.h>
#include <deque>
#include <WinBase.h>


/*============================================================================
 * Abstract command
 */
class ACommand: public IUnknown
{
public:
  typedef CComPtr<ACommand> Ptr;

  virtual ~ACommand(){};

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID   riid, LPVOID * ppvObj)
  {
    // Always set out parameter to NULL, validating it first.
    if (!ppvObj) {
      return E_INVALIDARG;
    }
    *ppvObj = NULL;
    if (riid == IID_IUnknown) {
      // Increment the reference count and return the pointer.
      *ppvObj = (LPVOID)this;
      AddRef();
      return NOERROR;
    }
    return E_NOINTERFACE;
  }

  ULONG STDMETHODCALLTYPE AddRef()
  {
    InterlockedIncrement(&mRef);
    return mRef;
  }

  ULONG STDMETHODCALLTYPE Release()
  {
    ULONG refCount = InterlockedDecrement(&mRef);
    if (0 == mRef) {
      delete this;
    }
    return refCount;
  }
protected:
  ACommand(){};

  volatile ULONG mRef;
};

class AQueuedCommand: public ACommand
{
public:
  typedef CComPtr<AQueuedCommand> Ptr;

  void operator()()
  {
    execute();
  }

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

  void addCommnad(AQueuedCommand::Ptr aCommand)
  {
    if (!aCommand) {
      throw std::runtime_error("Invalid pointer to command");
    }
    if (mAutoExec) {
      aCommand->execute();
    } else {
      mCommands.push_back(aCommand);
    }

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
    if (mAutoExec) {
      executeAll();
    }
  }
protected:
  typedef std::deque<AQueuedCommand::Ptr> Container;
  Container mCommands;
  bool mAutoExec;
};