#pragma once
#include <string>
#include <vector>

enum class WindowState { IDLE, CLOSED, PROCESSING };
static constexpr const char *StateStr(WindowState s) {
  switch (s) {
  case WindowState::IDLE:
    return "空闲";
  case WindowState::CLOSED:
    return "关闭";
  case WindowState::PROCESSING:
    return "处理中";
  default:
    return "未知";
  }
}

struct ServiceWindow {
  std::string id;
  WindowState state;
  std::string processingUserNo;
};

class ServiceWindowMgr {
public:
  std::string Add();
  void Close(int windowNo);
  void Open(int windowNo);
  void Idle(int windowNo);
  void CallNext(int windowNo);
  void List();
  std::vector<ServiceWindow> GetInProcessWindows();

  static ServiceWindowMgr *Instance();

private:
  static inline std::string WinName(int windowNo);
  inline bool IsWindowExists(int windowNo);

  std::vector<ServiceWindow> windows;
  static ServiceWindowMgr *mgr;
};
