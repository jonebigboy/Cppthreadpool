#pragma once

#include <queue>
#include <mutex>

template<typename T>
class SafeQueue
{
public:

	
	bool empty();

	int size();

	void enqueue(T& t);

	bool dequeue(T& t);


private:
	std::queue<T> m_queue;
	std::mutex m_mutex;
};







