#include "threadpool/threadpool.hpp"
#include "util.hpp"
#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <random>
#include <ctime>

// TODO take inspiration from the threadpool impl vanya sent you, for hints on how to
// accept more varied functions into the threads

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
        pool.push_back(std::thread(&ThreadPool::spin, this));
}

// predicate which returns ​false if the waiting should be continued.
bool ThreadPool::should_thread_wait()
{
    return task_queue.empty() && should_wait_for_more_tasks;
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
                    { return !should_thread_wait(); });

            if (!should_wait_for_more_tasks && task_queue.empty())
            {
                if (THREAD_POOL_DEBUG)
                {
                    std::unique_lock<std::mutex> lock(stdout_mutex);
                    std::cout << "Thread_" << id << "is exiting... " << std::endl;
                    std::cout << "Task Queue size: " << task_queue.size() << std::endl;
                }
                return;
            }
            task = task_queue.front();
            task_queue.pop();
        }
        task.execute();

        if (THREAD_POOL_DEBUG)
        {
            std::unique_lock<std::mutex> lock(stdout_mutex);
            std::cout << "Thread_" << id << std::endl;
        }
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

/*
 Waits for already started tasks to complete, and joins the threads.
 // TODO proper destructor for the threadpool, proper shut down of threads
*/
void ThreadPool::join_pool(bool abandon_unfinished_tasks)
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
    should_wait_for_more_tasks = false;
    cv.notify_all();

    for (auto it = pool.begin(); it != pool.end(); it++)
    {
        if (THREAD_POOL_DEBUG)
        {
            std::unique_lock<std::mutex> lock(stdout_mutex);
            std::cout << "Waiting for thread_" << it->get_id() << "to finish." << std::endl;
        }
        it->join();
    }
}

void ThreadPool::join_pool()
{
    join_pool(false);
}

void test_task_fn(std::string arg)
{
    int x = 0;
    for (int i = 0; i < 500; i++)
    {
        x += i;
    }
}

void test()
{
    const Task task(&test_task_fn, "asd");
    ThreadPool thread_pool = ThreadPool(100);

    for (int i = 0; i < 300; i++)
    {
        thread_pool.add_task(task);
        if (i == 100)
        {
            thread_pool.join_pool(false);
        }
    }

    char c;
    std::cin.clear();
    std::cin >> c;

    std::cout << "Quitting" << std::endl;
}
