#include "SafeQueue.h"

template<typename T>
bool SafeQueue<T>::empty()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.empty();
}

template<typename T>
int SafeQueue<T>::size()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	return m_queue.size();

}

template<typename T>
void SafeQueue<T>::enqueue(T& t)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.emplace(t);
}

template<typename T>
bool SafeQueue<T>::dequeue(T& t)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	if (m_queue.empty()) {
		return false;
	}
	t = std::move(m_queue.front()); //用右值复制不会拷贝。
	m_queue.pop();

	return true;
}

