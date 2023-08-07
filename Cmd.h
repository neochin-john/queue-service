#pragma once
#include <list>
#include <string>

class Cmd {
public:
  using Args = std::list<std::string> &;

  struct CmdItem {

    bool Exec(Args args);

    void Help(std::string indent = "");
    CmdItem *QueryCmdItem(Args args);

    std::string cmdStr;
    std::list<CmdItem> subCmds;
    std::string helpStr;
    void (*innerExec)(Args args);
  };

  static void Help(Args args);
  static CmdItem *Instance();

private:
  static CmdItem *cmd;
};
