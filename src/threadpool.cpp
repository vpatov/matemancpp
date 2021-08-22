#include "threadpool.hpp"
#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <random>
#include <ctime>

ThreadPool::ThreadPool()
{
    m_num_threads = std::thread::hardware_concurrency();
    initialize_threads();
}

ThreadPool::ThreadPool(int num_threads)
{
    if (num_threads >= 0 && num_threads <= 1024)
    {
        m_num_threads = num_threads;
    }
    else
    {
        m_num_threads = std::thread::hardware_concurrency();
    }
    initialize_threads();
}

void ThreadPool::initialize_threads()
{
    for (int i = 0; i < m_num_threads; i++)
    {
        pool.push_back(std::thread(&ThreadPool::spin, this));
    }
}

void ThreadPool::spin()
{
    std::thread::id id = std::this_thread::get_id();
    while (true)
    {

        Task task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);

            cv.wait(lock, [this]
                    { return !task_queue.empty() || should_destroy; });
            if (should_destroy)
            {

#ifdef THREAD_POOL_DEBUG
                {
                    std::unique_lock<std::mutex> lock(stdout_mutex);
                    std::cout << "Thread_" << id << "is exiting... " << std::endl;
                    std::cout << "Task Queue size: " << task_queue.size() << std::endl;
                }
#endif
                return;
            }
            task = task_queue.front();
            task_queue.pop();
        }

        task.m_fn(task.m_arg);

#ifdef THREAD_POOL_DEBUG
        {
            std::unique_lock<std::mutex> lock(stdout_mutex);
            std::cout << "Thread_" << id << std::endl;
        }
#endif
    }
}

void ThreadPool::add_task(Task task)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        task_queue.push(task);
    }
    cv.notify_one();
}

void ThreadPool::destroy_pool(bool abandon_unfinished_tasks)
{
    if (abandon_unfinished_tasks)
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            while (!task_queue.empty())
            {
                task_queue.pop();
            }
        }
    }
    should_destroy = true;
    cv.notify_all();

    for (auto it = pool.begin(); it != pool.end(); it++)
    {
        (*it).join();
    }
}

void test_task_fn(void *arg)
{
    int x = 0;
    for (int i = 0; i < 500; i++)
    {
        x += i;
    }
}

void test()
{
    const Task task(&test_task_fn, NULL);
    ThreadPool thread_pool = ThreadPool(100);

    for (int i = 0; i < 300; i++)
    {
        thread_pool.add_task(task);
        if (i == 100)
        {
            thread_pool.destroy_pool(false);
        }
    }

    char c;
    std::cin.clear();
    std::cin >> c;

    std::cout << "Quitting" << std::endl;
}
