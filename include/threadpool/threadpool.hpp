#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <random>
#include <ctime>
#include <thread>

#ifndef THREAD_POOL_DEBUG
// uncomment line below to enable debug messages
// #define THREAD_POOL_DEBUG
#endif

struct Task
{
    void (*m_fn)(std::string arg) = NULL;
    std::function<void(std::string &)> *m_function = NULL;
    std::string m_arg;

    Task(){};
    Task(void (*fn)(std::string arg), std::string arg) : m_fn(fn), m_arg(arg) {}

    Task(std::function<void(std::string &)> *fn, std::string arg)
    {
        m_function = fn;
        m_arg = arg;
    }

    void execute()
    {
        if (m_fn != NULL)
        {
            m_fn(m_arg);
        }
        else if (m_function != NULL)
        {
            (*(m_function))(m_arg);
        }
        else
        {
            assert(false);
        }
    }
};

class ThreadPool
{
private:
    int m_num_threads;
    std::vector<std::thread> pool;
    std::mutex queue_mutex;
    std::queue<Task> task_queue;
    std::condition_variable cv;
    bool should_wait_for_more_tasks = true;
    static const bool THREAD_POOL_DEBUG = false;

    std::mutex stdout_mutex;

    void initialize_threads();
    void spin();
    bool should_thread_wait();

public:
    ThreadPool();
    ThreadPool(int num_threads);

    void add_task(Task task);
    void join_pool(bool abandon_unfinished_tasks);
    void join_pool();
};

void test();