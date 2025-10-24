#pragma once
#include "scheduler.h"

struct RRScheduler : public IScheduler {
	void onProcessStart(os_sim::PCB& process) override;
	void onProcessExecution(os_sim::PCB& process, std::list<os_sim::PCB>& pcbQueue) override;
	void returnToQueue(os_sim::PCB& process, std::list<os_sim::PCB>& pcbQueue) override;
};