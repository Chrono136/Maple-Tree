#pragma once
class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	static void start_thread(const std::string &tname, std::thread thrd, bool detach = true);

	static void stop_thread(const std::string &tname);

	static ThreadMap tm_;
};

