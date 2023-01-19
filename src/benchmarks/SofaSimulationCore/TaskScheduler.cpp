#include <benchmark/benchmark.h>
#include <sofa/simulation/DefaultTaskScheduler.h>
#include <sofa/simulation/CpuTask.h>
#include <sofa/simulation/MainTaskSchedulerFactory.h>
#include <sofa/simulation/ParallelForEach.h>
#include <sofa/type/Mat.h>
#include <sofa/type/vector_T.h>
#include <utils/RandomValuePool.h>
#include <utils/thread_pool.hpp>

constexpr unsigned int payloadDurationMicroseconds = 100;
constexpr unsigned int maxTaskNumberRange = 1e5;
const auto taskNumberRange = benchmark::CreateRange(1e3, maxTaskNumberRange, 10);
const auto threadNumberRange = benchmark::CreateRange(1, std::thread::hardware_concurrency(), 2);

class EmptyTask : public sofa::simulation::CpuTask
{
public:
    explicit EmptyTask(sofa::simulation::CpuTask::Status* status)
        : sofa::simulation::CpuTask(status) {}

    sofa::simulation::Task::MemoryAlloc run() final
    {
        return sofa::simulation::Task::Stack;
    }
};

void payloadTask()
{
    const auto begin = std::chrono::high_resolution_clock::now();
    long long newId = 0;
    while ( std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - begin).count() < payloadDurationMicroseconds)
    {
        for (int i = 1; i < begin.time_since_epoch().count() % 100; ++i)
        {
            newId += begin.time_since_epoch().count() / i;
            newId %= begin.time_since_epoch().count();
        }
    }
};

class PayloadTask : public sofa::simulation::CpuTask
{
public:
    explicit PayloadTask(sofa::simulation::CpuTask::Status* status)
        : sofa::simulation::CpuTask(status) {}

    sofa::simulation::Task::MemoryAlloc run() final
    {
        payloadTask();
        return sofa::simulation::Task::Stack;
    }
};

template<class TTask>
static void BM_TaskScheduler(benchmark::State &state)
{
    auto *taskScheduler =sofa::simulation::MainTaskSchedulerFactory::createInRegistry();
    assert(taskScheduler != nullptr);

    taskScheduler->init(state.range(1));

    for (auto _ : state)
    {
        state.PauseTiming();

        sofa::type::vector<TTask> tasks;
        tasks.reserve(state.range(0));

        sofa::simulation::CpuTask::Status status;

        state.ResumeTiming();

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            tasks.emplace_back(&status);
            taskScheduler->addTask(&tasks.back());
        }

        taskScheduler->workUntilDone(&status);
    }

    if constexpr (std::is_same_v<TTask, PayloadTask>)
    {
        //The duration in ms that it would take if no overhead: number of tasks * duration of one task / nb of threads
        state.counters["theoryMs"] = benchmark::Counter(state.range(0) * payloadDurationMicroseconds / 1000 / taskScheduler->getThreadCount());
    }
}

BENCHMARK_TEMPLATE(BM_TaskScheduler, EmptyTask)->ArgsProduct(
{
    taskNumberRange, threadNumberRange
})->Threads(1)->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_TaskScheduler, PayloadTask)->ArgsProduct(
{
    taskNumberRange, threadNumberRange
})->Threads(1)->Unit(benchmark::kMillisecond);

static void BM_TaskScheduler_PayloadTask_ParallelizeLoop(benchmark::State &state)
{
    constexpr auto loop = [](const int64_t b)
    {
        payloadTask();
    };

    auto *taskScheduler =sofa::simulation::MainTaskSchedulerFactory::createInRegistry();
    assert(taskScheduler != nullptr);

    taskScheduler->init(state.range(1));

    for (auto _ : state)
    {
        sofa::simulation::parallelForEach(*taskScheduler,
            static_cast<int64_t>(0), state.range(0), loop);
    }

    //The duration in ms that it would take if no overhead: number of tasks * duration of one task / nb of threads
    state.counters["theoryMs"] = benchmark::Counter(state.range(0) * payloadDurationMicroseconds / 1000 / state.range(1));
}

BENCHMARK(BM_TaskScheduler_PayloadTask_ParallelizeLoop)->ArgsProduct(
{
    taskNumberRange,threadNumberRange
})->Threads(1)->Unit(benchmark::kMillisecond);

static void BM_TaskScheduler_PayloadTask_ParallelizeLoopRange(benchmark::State &state)
{
    constexpr auto loop = [](const auto& range)
    {
        for (auto it = range.start; it != range.end; ++it)
        {
            payloadTask();
        }
    };

    auto *taskScheduler =sofa::simulation::MainTaskSchedulerFactory::createInRegistry();
    assert(taskScheduler != nullptr);

    taskScheduler->init(state.range(1));

    for (auto _ : state)
    {
        sofa::simulation::parallelForEachRange(*taskScheduler,
            static_cast<int64_t>(0), state.range(0), loop);
    }

    //The duration in ms that it would take if no overhead: number of tasks * duration of one task / nb of threads
    state.counters["theoryMs"] = benchmark::Counter(state.range(0) * payloadDurationMicroseconds / 1000 / state.range(1));
}

BENCHMARK(BM_TaskScheduler_PayloadTask_ParallelizeLoopRange)->ArgsProduct(
{
    taskNumberRange,threadNumberRange
})->Threads(1)->Unit(benchmark::kMillisecond);

static void BM_TaskScheduler_MatrixMultTransposeTask_ParallelizeLoopRange(benchmark::State &state)
{
    constexpr auto totalsize = maxTaskNumberRange * 3*3 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<sofa::type::Mat<3, 3, float > > vc1;
    std::vector<sofa::type::Mat<3, 3, float > > vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    auto *taskScheduler =sofa::simulation::MainTaskSchedulerFactory::createInRegistry();
    assert(taskScheduler != nullptr);

    taskScheduler->init(state.range(1));

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        sofa::type::Mat<3, 3, float > mat1, mat2;
        for (int a = 0; a < 3; ++a)
        {
            for (int b = 0; b < 3; ++b)
            {
                mat1[a][b] = *it++;
                mat2[a][b] = *it++;
            }
        }
        vc1.push_back(mat1);
        vc2.push_back(mat2);
    }

    for (auto _ : state)
    {
        sofa::simulation::parallelForEachRange(*taskScheduler,
            static_cast<int64_t>(0), state.range(0),
            [&vc1, &vc2](const auto& range)
            {
                for (auto it = range.start; it != range.end; ++it)
                {
                    benchmark::DoNotOptimize(vc1[it].multTranspose(vc2[it]));
                }
            });
    }
}

BENCHMARK(BM_TaskScheduler_MatrixMultTransposeTask_ParallelizeLoopRange)->ArgsProduct(
{
    taskNumberRange,threadNumberRange
})->Threads(1)->Unit(benchmark::kMicrosecond);

static void BM_ThreadPool_EmptyTasks(benchmark::State &state)
{
    constexpr auto emptyTask = [](){};
    for (auto _ : state)
    {
        state.PauseTiming();
        thread_pool pool(state.range(1));
        state.ResumeTiming();

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            pool.push_task(emptyTask);
        }
        pool.wait_for_tasks();
    }
}

static void BM_ThreadPool_PayloadTask(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        thread_pool pool(state.range(1));
        state.ResumeTiming();

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            pool.push_task(payloadTask);
        }
        pool.wait_for_tasks();
    }

    //The duration in ms that it would take if no overhead: number of tasks * duration of one task / nb of threads
    state.counters["theoryMs"] = benchmark::Counter(state.range(0) * payloadDurationMicroseconds / 1000 / state.range(1));
}

static void BM_ThreadPool_PayloadTask_ParallelizeLoop(benchmark::State &state)
{
    constexpr auto loop = [](const unsigned int a, const unsigned int b)
    {
        for (unsigned int i = a; i < b; i++)
        {
            payloadTask();
        }
    };
    for (auto _ : state)
    {
        state.PauseTiming();
        thread_pool pool(state.range(1));
        state.ResumeTiming();

        pool.parallelize_loop(0, state.range(0), loop);

        pool.wait_for_tasks();
    }

    //The duration in ms that it would take if no overhead: number of tasks * duration of one task / nb of threads
    state.counters["theoryMs"] = benchmark::Counter(state.range(0) * payloadDurationMicroseconds / 1000 / state.range(1));
}


static void BM_ThreadPool_MatrixMultTransposeTask_ParallelizeLoop(benchmark::State &state)
{
    constexpr auto totalsize = maxTaskNumberRange * 3*3 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<sofa::type::Mat<3, 3, float > > vc1;
    std::vector<sofa::type::Mat<3, 3, float > > vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        sofa::type::Mat<3, 3, float > mat1, mat2;
        for (int a = 0; a < 3; ++a)
        {
            for (int b = 0; b < 3; ++b)
            {
                mat1[a][b] = *it++;
                mat2[a][b] = *it++;
            }
        }
        vc1.push_back(mat1);
        vc2.push_back(mat2);
    }

    thread_pool pool(state.range(1));

    for (auto _ : state)
    {
        pool.parallelize_loop(0, state.range(0),
            [&vc1, &vc2](const int64_t a, const int64_t& b)
            {
                for (auto it = a; it < b; ++it)
                {
                    benchmark::DoNotOptimize(vc1[it].multTranspose(vc2[it]));
                }
            });
        pool.wait_for_tasks();
    }
}

BENCHMARK(BM_ThreadPool_MatrixMultTransposeTask_ParallelizeLoop)->ArgsProduct(
{
    taskNumberRange,threadNumberRange
})->Threads(1)->Unit(benchmark::kMicrosecond);


BENCHMARK(BM_ThreadPool_EmptyTasks)->ArgsProduct(
{
    taskNumberRange, threadNumberRange
})->Threads(1)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_ThreadPool_PayloadTask)->ArgsProduct(
{
    taskNumberRange,threadNumberRange
})->Threads(1)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_ThreadPool_PayloadTask_ParallelizeLoop)->ArgsProduct(
{
    taskNumberRange,threadNumberRange
})->Threads(1)->Unit(benchmark::kMillisecond);
