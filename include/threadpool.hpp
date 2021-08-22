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
    void (*m_fn)(void *);
    void *m_arg;

    Task() {}
    Task(void (*fn)(void *), void *arg) : m_fn(fn), m_arg(arg) {}
};

class ThreadPool
{
private:
    int m_num_threads;
    std::vector<std::thread> pool;
    std::mutex queue_mutex;
    std::queue<Task> task_queue;
    std::condition_variable cv;
    bool should_destroy = false;

#ifdef THREAD_POOL_DEBUG
    std::mutex stdout_mutex;
#endif

    void initialize_threads();

    void spin();

public:
    ThreadPool();
    ThreadPool(int num_threads);

    void add_task(Task task);
    void destroy_pool(bool abandon_unfinished_tasks);
};

void test();