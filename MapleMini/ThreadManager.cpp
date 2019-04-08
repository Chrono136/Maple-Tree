#include "stdafx.h"
#include "ThreadManager.h"


ThreadMap ThreadManager::tm_;

ThreadManager::ThreadManager()
{
}


ThreadManager::~ThreadManager()
{
}

void ThreadManager::start_thread(const std::string &tname, std::thread thrd, bool detach)
{
	if (detach)
		thrd.detach();

	tm_[tname] = std::move(thrd);
}

void ThreadManager::stop_thread(const std::string &tname)
{
	ThreadMap::const_iterator it = tm_.find(tname);
	if (it != tm_.end()) {
		it->second.std::thread::~thread();
		tm_.erase(tname);
	}
}