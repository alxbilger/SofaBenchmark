#include <benchmark/benchmark.h>

#include <sofa/simulation/init.h>

int main(int argc, char** argv)
{
    char arg0_default[]="benchmark";
    char* args_default=arg0_default;
    if(!argv)
    {
        argc=1;
        argv=&args_default;
    }
    ::benchmark::Initialize(&argc, argv);
    if(::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;

    sofa::simulation::core::init();

    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();

    sofa::simulation::core::cleanup();
    return 0;
}
