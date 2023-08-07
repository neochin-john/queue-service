#include "Cmd.h"
#include "ServiceQueueMgr.h"
#include "ServiceWindowMgr.h"
#include <iostream>

static void PrintErrorCmd(std::string cmd = "") {
  std::cout << "命令错误, 使用help " << cmd << "命令查看帮助" << std::endl;
}

bool Cmd::CmdItem::Exec(Args args) {
  if (args.empty()) {
    return false;
  }

  // 当前命令是否匹配，注意防止根节点
  std::string &cmd = args.front();
  if (cmd != this->cmdStr && this->cmdStr != "") {
    // 命令不匹配
    return false;
  }

  if (this->cmdStr != "") {
    // 非根cmd，表示配置成功，只留下参数
    args.pop_front();
  }

  for (auto &subCmd : subCmds) {
    if (subCmd.Exec(args)) {
      return true;
    }
  }

  if (this->innerExec != nullptr) {
    this->innerExec(args);
  }

  return true;
}

void Cmd::CmdItem::Help(std::string indent) {
  std::cout << indent << this->helpStr << std::endl;
  indent += "  ";
  for (auto &subCmd : subCmds) {
    subCmd.Help(indent);
  }
}

void Cmd::Help(Args args) {
  CmdItem *item = Cmd::cmd->QueryCmdItem(args);
  item = item == nullptr ? Cmd::cmd : item;
  item->Help();
}

Cmd::CmdItem *Cmd::CmdItem::QueryCmdItem(Args args) {
  if (args.empty()) {
    return this;
  }

  std::string cmd = args.front();
  if (cmd != this->cmdStr && this->cmdStr != "") {
    return nullptr;
  }

  if (this->cmdStr != "") {
    args.pop_front();
  }

  if (args.empty()) {
    return this;
  }

  for (auto &subCmd : subCmds) {
    CmdItem *item = subCmd.QueryCmdItem(args);
    if (item != nullptr) {
      return item;
    }
  }

  // 子命令不匹配就返回自己
  return this;
}

static Cmd::CmdItem queue_wait_list() {
  return {
      .cmdStr = "list",
      .subCmds = {},
      .helpStr = "list 显示等待队列",
      .innerExec =
          [](Cmd::Args) { ServiceQueueMgr::Instance()->ListWaitQueue(); },
  };
}

static Cmd::CmdItem queue_wait_add_normal() {
  return {
      .cmdStr = "normal",
      .subCmds = {},
      .helpStr = "normal 普通用户取号",
      .innerExec =
          [](Cmd::Args) { ServiceQueueMgr::Instance()->AddNormalUser(); },
  };
}

static Cmd::CmdItem queue_wait_add_vip() {
  return {
      .cmdStr = "vip",
      .subCmds = {},
      .helpStr = "vip VIP用户取号",
      .innerExec = [](Cmd::Args) { ServiceQueueMgr::Instance()->AddVipUser(); },
  };
}

static Cmd::CmdItem queue_wait_add() {
  return {
      .cmdStr = "add",
      .subCmds =
          {
              queue_wait_add_normal(),
              queue_wait_add_vip(),
          },
      .helpStr = "add 用户取号命令",
      .innerExec = [](Cmd::Args) { PrintErrorCmd("queue wait add"); },
  };
}

static Cmd::CmdItem queue_wait() {
  return {
      .cmdStr = "wait",
      .subCmds =
          {
              queue_wait_list(),
              queue_wait_add(),
          },
      .helpStr = "wait 等待队列命令",
      .innerExec = [](Cmd::Args) { PrintErrorCmd("queue wait"); },
  };
}

static Cmd::CmdItem queue_history_list() {
  return {
      .cmdStr = "list",
      .subCmds = {},
      .helpStr = "list 显示已处理队列",
      .innerExec =
          [](Cmd::Args) { ServiceQueueMgr::Instance()->ListHistoryQueue(); },
  };
}

static Cmd::CmdItem queue_history() {
  return {
      .cmdStr = "history",
      .subCmds =
          {
              queue_history_list(),
          },
      .helpStr = "history 历史队列命令",
      .innerExec = [](Cmd::Args) { PrintErrorCmd("queue history"); },
  };
}

static Cmd::CmdItem queue_processing_list() {
  return {
      .cmdStr = "list",
      .subCmds = {},
      .helpStr = "list 显示正在处理业务的队列",
      .innerExec =
          [](Cmd::Args) { ServiceQueueMgr::Instance()->ListProcessingQueue(); },
  };
}

static Cmd::CmdItem queue_processing() {
  return {
      .cmdStr = "processing",
      .subCmds =
          {
              queue_processing_list(),
          },
      .helpStr = "processing 正在处理业务队列命令",
      .innerExec = [](Cmd::Args) { PrintErrorCmd("queue processing"); },
  };
}

static Cmd::CmdItem queue() {
  return {
      .cmdStr = "queue",
      .subCmds =
          {
              queue_wait(),
              queue_history(),
              queue_processing(),
          },
      .helpStr = "queue 队列命令",
      .innerExec = [](Cmd::Args) { PrintErrorCmd("queue"); },
  };
}

static Cmd::CmdItem service_list() {
  return {
      .cmdStr = "list",
      .subCmds = {},
      .helpStr = "list 显示服务窗口",
      .innerExec = [](Cmd::Args) { ServiceWindowMgr::Instance()->List(); },
  };
}

static Cmd::CmdItem service_add() {
  return {
      .cmdStr = "add",
      .subCmds = {},
      .helpStr = "add 添加服务窗口",
      .innerExec = [](Cmd::Args) { ServiceWindowMgr::Instance()->Add(); },
  };
}

static void ServiceCall(Cmd::Args args, std::string helpStr,
                        void (ServiceWindowMgr::*func)(int windowId)) {
  if (args.empty()) {
    PrintErrorCmd(helpStr);
    return;
  }

  try {
    (ServiceWindowMgr::Instance()->*func)(std::stoi(args.front()));
  } catch (...) {
    PrintErrorCmd(helpStr);
  }
}

static Cmd::CmdItem service_close() {
  return {
      .cmdStr = "close",
      .subCmds = {},
      .helpStr = "close windowId 关闭服务窗口",
      .innerExec =
          [](Cmd::Args args) {
            ServiceCall(args, "service close", &ServiceWindowMgr::Close);
          },
  };
}

static Cmd::CmdItem service_open() {
  return {
      .cmdStr = "open",
      .subCmds = {},
      .helpStr = "open windowId 打开服务窗口",
      .innerExec =
          [](Cmd::Args args) {
            ServiceCall(args, "service open", &ServiceWindowMgr::Open);
          },
  };
}

static Cmd::CmdItem service_idle() {
  return {
      .cmdStr = "idle",
      .subCmds = {},
      .helpStr = "idle windowId 置服务窗口空闲",
      .innerExec =
          [](Cmd::Args args) {
            ServiceCall(args, "service idle", &ServiceWindowMgr::Idle);
          },
  };
}

static Cmd::CmdItem service_callnext() {
  return {
      .cmdStr = "callnext",
      .subCmds = {},
      .helpStr = "callnext windowId 窗口叫号",
      .innerExec =
          [](Cmd::Args args) {
            ServiceCall(args, "service callnext", &ServiceWindowMgr::CallNext);
          },
  };
}

static Cmd::CmdItem service() {
  return {
      .cmdStr = "service",
      .subCmds =
          {
              service_list(),
              service_add(),
              service_close(),
              service_open(),
              service_idle(),
              service_callnext(),
          },
      .helpStr = "service 服务命令",
      .innerExec = [](Cmd::Args) { PrintErrorCmd("service"); },
  };
}

static Cmd::CmdItem help() {
  return {
      .cmdStr = "help",
      .subCmds = {},
      .helpStr = "help [service|...] 查看帮助",
      .innerExec = [](Cmd::Args args) { Cmd::Help(args); },
  };
}

Cmd::CmdItem *Cmd::cmd = new Cmd::CmdItem{
    .cmdStr = "",
    .subCmds =
        {
            service(),
            queue(),
            help(),
        },
    .helpStr = "命令帮助",
    .innerExec =
        [](Cmd::Args args) {
          if (!args.empty()) {
            PrintErrorCmd();
          }
        },
};

Cmd::CmdItem *Cmd::Instance() { return Cmd::cmd; }
