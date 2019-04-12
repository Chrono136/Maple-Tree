#pragma once
class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	static void StartThread(const std::string &tname, std::thread thrd, bool detach = true);

	static void StopThread(const std::string &tname);

	static ThreadMap tm_;
};

