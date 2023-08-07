#include "ServiceWindowMgr.h"
#include "ServiceQueueMgr.h"
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

string ServiceWindowMgr::WinName(int windowNo) {
  ostringstream ss;
  ss << "窗口" << windowNo;
  return ss.str();
}

bool ServiceWindowMgr::IsWindowExists(int windowNo) {
  return (size_t)windowNo <= windows.size() && windowNo > 0;
}

string ServiceWindowMgr::Add() {
  string id = WinName(windows.size() + 1);
  ServiceWindow window = {.id = id, .state = WindowState::IDLE};
  windows.push_back(window);
  cout << "添加窗口成功，编号为" << quoted(id) << endl;
  return id;
}

void ServiceWindowMgr::Close(int windowNo) {
  string winName = WinName(windowNo);
  if (!IsWindowExists(windowNo)) {
    cout << "关闭" << winName << "失败，该窗口不存在" << endl;
    return;
  }

  ServiceWindow &win = windows[windowNo - 1];
  switch (win.state) {
  case WindowState::IDLE:
    win.state = WindowState::CLOSED;
    cout << "关闭" << winName << "成功" << endl;
    break;
  case WindowState::CLOSED:
    cout << winName << "已是关闭状态" << endl;
    break;
  case WindowState::PROCESSING:
    cout << "关闭" << winName << "失败，该窗口正在处理业务" << endl;
    break;
  }
}

void ServiceWindowMgr::Open(int windowNo) {
  string winName = WinName(windowNo);
  if (!IsWindowExists(windowNo)) {
    cout << "打开" << winName << "失败，该窗口不存在" << endl;
    return;
  }

  ServiceWindow &win = windows[windowNo - 1];
  switch (win.state) {
  case WindowState::IDLE:
  case WindowState::PROCESSING:
    cout << winName << "已是打开状态" << endl;
    break;
  case WindowState::CLOSED:
    win.state = WindowState::IDLE;
    cout << "打开" << winName << "成功" << endl;
    break;
  }
}

void ServiceWindowMgr::Idle(int windowNo) {
  string winName = WinName(windowNo);
  if (!IsWindowExists(windowNo)) {
    cout << "置" << winName << "空闲失败，该窗口不存在" << endl;
    return;
  }

  ServiceWindow &win = windows[windowNo - 1];
  switch (win.state) {
  case WindowState::IDLE:
    cout << winName << "已是空闲状态" << endl;
    break;
  case WindowState::PROCESSING:
    ServiceQueueMgr::Instance()->AppendDoneQueue(win.processingUserNo);
    win.state = WindowState::IDLE;
    cout << "置" << winName << "空闲成功" << endl;
    break;
  case WindowState::CLOSED:
    cout << "置" << winName << "空闲失败，该窗口为关闭状态" << endl;
    break;
  }
}

void ServiceWindowMgr::CallNext(int windowNo) {
  string winName = WinName(windowNo);
  if (!IsWindowExists(windowNo)) {
    cout << winName << "呼叫失败，该窗口不存在" << endl;
    return;
  }

  ServiceWindow &win = windows[windowNo - 1];
  if (win.state == WindowState::CLOSED) {
    cout << winName << "呼叫失败，该窗口为关闭状态" << endl;
    return;
  }

  string userNo = ServiceQueueMgr::Instance()->PopWaitQueue();
  if (win.state == WindowState::IDLE) {
    if (userNo == "") {
      cout << "当前没有用户需要办理业务" << endl;
    } else {
      win.state = WindowState::PROCESSING;
      win.processingUserNo = userNo;
      cout << "请" << userNo << "号到" << winName << "办理业务" << endl;
    }

    return;
  }

  if (win.state == WindowState::PROCESSING) {
    ServiceQueueMgr::Instance()->AppendDoneQueue(win.processingUserNo);

    if (userNo == "") {
      win.state = WindowState::IDLE;
      cout << "当前没有用户需要办理业务" << endl;
    } else {
      cout << win.processingUserNo << "处理完毕, 请" << userNo << "号到"
           << winName << "办理业务" << endl;
      win.processingUserNo = userNo;
    }
  }
}

void ServiceWindowMgr::List() {
  for (ServiceWindow &win : windows) {
    if (win.state == WindowState::PROCESSING) {
      cout << win.id << "\t" << win.processingUserNo << endl;
    } else {
      cout << win.id << "\t" << StateStr(win.state) << endl;
    }
  }
}

std::vector<ServiceWindow> ServiceWindowMgr::GetInProcessWindows() {
  std::vector<ServiceWindow> inProcessWindows;
  for (ServiceWindow &win : windows) {
    if (win.state == WindowState::PROCESSING) {
      inProcessWindows.push_back(win);
    }
  }

  return inProcessWindows;
}

ServiceWindowMgr *ServiceWindowMgr::Instance() { return mgr; }

ServiceWindowMgr *ServiceWindowMgr::mgr = new ServiceWindowMgr;
