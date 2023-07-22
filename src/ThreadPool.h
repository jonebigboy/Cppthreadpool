#pragma once

#include <mutex>

#include <future>
#include <vector>
#include <thread>
#include <functional>
#include "SafeQueue.h"
#include "SafeQueue.cpp"


//template<typename T>
//class SafeQueue
//{
//public:
//
//
//	bool empty() {
//		std::unique_lock<std::mutex> lock(m_mutex);
//		return m_queue.empty();
//	}
//
//
//	int size() {
//		std::unique_lock<std::mutex> lock(m_mutex);
//		return m_queue.size();
//	}
//
//
//	void enqueue(T& t) {
//		std::unique_lock<std::mutex> lock(m_mutex);
//		m_queue.emplace(t);
//	}
//
//
//	bool dequeue(T& t) {
//		std::unique_lock<std::mutex> lock(m_mutex);
//		if (m_queue.empty()) {
//			return false;
//		}
//		t = std::move(m_queue.front()); //用右值复制不会拷贝。
//		m_queue.pop();
//
//		return true;
//	}
//
//private:
//	std::queue<T> m_queue;
//	std::mutex m_mutex;
//};




class ThreadPool
{

private:
	class ThreadWorker {

	private:
		int m_id; //线程ID

		ThreadPool* m_pool;

	public:
		ThreadWorker(ThreadPool* pool, const int id) : m_pool(pool), m_id(id) {};
		
		void operator()() {
			std::function<void()> func;

			bool dequeueing;

			while (!m_pool->m_shutdown) {
				{
					std::unique_lock<std::mutex> lock(m_pool->m_mutex);

					if (m_pool->m_queue.empty()) {
						m_pool->m_conditional_lock.wait(lock);
					}
					dequeueing = m_pool->m_queue.dequeue(func);

				}

				if (dequeueing) {
					func();
				}

			}


		}
	};

public:

	/*template<typename F, typename... Args>
	auto submit(F&& f, Args &&...args) -> std::future<decltype(f(args...))>;*/

	template<typename F, typename... Args>
	auto submit(F&& f, Args &&...args) -> std::future<decltype(f(args...))>
	{
		std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

		auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

		std::function<void()> warpper_func = [task_ptr]() {
			(*task_ptr)();
		};
		m_queue.enqueue(warpper_func);

		m_conditional_lock.notify_one();

		return task_ptr->get_future();

	}

	

	ThreadPool(const int n_threads );

	ThreadPool(const ThreadPool&) = delete;

	ThreadPool(ThreadPool&&) = delete;

	ThreadPool &operator=(const ThreadPool&) = delete;

	ThreadPool& operator=(ThreadPool&&) = delete;

	~ThreadPool();

	void init() {

		for (int i = 0; i < m_threads.size(); ++i)
		{
			m_threads[i] = std::thread(ThreadWorker(this, i)); // 分配工作线程
		}
	}
	

private:

	bool m_shutdown;

	SafeQueue<std::function<void()>> m_queue;

	std::vector<std::thread> m_threads;

	std::mutex m_mutex;

	std::condition_variable m_conditional_lock;
};





