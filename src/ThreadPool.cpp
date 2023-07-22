#include "ThreadPool.h"



//template<typename F, typename... Args>
//auto ThreadPool::submit(F&& f, Args &&...args) -> std::future<decltype(f(args...))>
//{
//	std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
//
//	auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
//
//	std::function<void()> warpper_func = [task_ptr]() {
//		(*task_ptr)();
//	};
//	m_queue.enqueue(warpper_func);
//
//	m_conditional_lock.notify_one();
//
//	return task_ptr->get_future();
//
//}


ThreadPool::ThreadPool(const int n_threads = 4)
	:m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false)
{

}

ThreadPool::~ThreadPool()
{
	m_shutdown = true;
	m_conditional_lock.notify_all();

	for (int i=0;i<m_threads.size();i++)
	{
		if (m_threads[i].joinable()) { //是否在等待
			m_threads[i].join();//等待结束
		}
	}
}




