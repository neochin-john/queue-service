#pragma once
#include <list>
#include <string>

class ServiceQueueMgr {
public:
  std::string AddNormalUser();
  std::string AddVipUser();
  std::string PopWaitQueue();
  void AppendDoneQueue(std::string userNo);

  void ListWaitQueue();
  void ListHistoryQueue();
  void ListProcessingQueue();

  static ServiceQueueMgr *Instance();

private:
  static std::string UserNo(char prefix, int id);
  static inline bool IsVipNo(std::string userNo);

  int vipUserTotal = 0;
  int normalUserTotal = 0;
  std::list<std::string> waitQueue;
  std::list<std::string> doneQueue;

  static ServiceQueueMgr *mgr;
};
