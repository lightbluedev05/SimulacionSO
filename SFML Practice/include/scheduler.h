#pragma once
#include <list>

//#define OS_DEBUG

namespace os_sim {
	struct PCB;
}

struct IScheduler {
	virtual void onProcessStart(os_sim::PCB& process);
	virtual void onProcessExecution(os_sim::PCB& process, std::list<os_sim::PCB>& pcbQueue) = 0;
	virtual void returnToQueue(os_sim::PCB& process, std::list<os_sim::PCB>& pcbQueue) = 0;
	virtual void selectProcess(std::list<os_sim::PCB>& pcbQueue);
	virtual ~IScheduler() = default;

	void outputMetrics(os_sim::PCB& process);
};