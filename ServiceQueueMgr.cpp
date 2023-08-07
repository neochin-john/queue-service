#include "ServiceQueueMgr.h"
#include "ServiceWindowMgr.h"
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

#define QUEUE_NUMBER_FIX_WIDTH 3
#define VIP_USER_PREFIX 'A'
#define NORMAL_USER_PREFIX 'C'

std::string ServiceQueueMgr::UserNo(char prefix, int id) {
  ostringstream ss;
  ss << prefix << setw(QUEUE_NUMBER_FIX_WIDTH) << setfill('0') << id;
  return ss.str();
}

bool ServiceQueueMgr::IsVipNo(std::string userNo) {
  return userNo[0] == VIP_USER_PREFIX;
}

string ServiceQueueMgr::AddNormalUser() {
  string userNo = UserNo(NORMAL_USER_PREFIX, ++normalUserTotal);
  waitQueue.push_back(userNo);
  cout << "您的号码为" << userNo << endl;
  return userNo;
}

string ServiceQueueMgr::AddVipUser() {
  string userNo = UserNo(VIP_USER_PREFIX, ++vipUserTotal);
  cout << "您的号码为" << userNo << endl;

  auto iter = waitQueue.begin();
  for (int offset = 0; iter != waitQueue.end(); iter++) {
    if (IsVipNo(*iter)) {
      offset = 0;
      continue;
    }

    if (offset++ >= 2) {
      break;
    }
  }

  waitQueue.insert(iter, userNo);
  return userNo;
}

string ServiceQueueMgr::PopWaitQueue() {
  if (waitQueue.empty()) {
    return "";
  }

  string front = waitQueue.front();
  waitQueue.pop_front();
  return front;
}

void ServiceQueueMgr::AppendDoneQueue(string userNo) {
  doneQueue.push_back(userNo);
}

void ServiceQueueMgr::ListWaitQueue() {
  cout << "当前有" << waitQueue.size() << "个用户排队:" << endl;
  for (auto &number : waitQueue) {
    cout << number << endl;
  }
}

void ServiceQueueMgr::ListHistoryQueue() {
  cout << "已有" << doneQueue.size() << "个用户处理完毕:" << endl;
  for (auto &number : doneQueue) {
    cout << number << endl;
  }
}

void ServiceQueueMgr::ListProcessingQueue() {
  std::vector<ServiceWindow> windows =
      ServiceWindowMgr::Instance()->GetInProcessWindows();
  cout << "有" << windows.size() << "个用户正在办理业务:" << endl;
  for (auto &window : windows) {
    cout << window.id << "\t" << window.processingUserNo << endl;
  }
}

ServiceQueueMgr *ServiceQueueMgr::mgr = new ServiceQueueMgr();
ServiceQueueMgr *ServiceQueueMgr::Instance() { return mgr; }
