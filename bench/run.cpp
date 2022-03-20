#include <benchmark/benchmark.h>
#include <hyperspace_navigator.hpp>

using namespace hyperspace_navigator;

static void BM_fastestRoute(benchmark::State& state) // NOLINT google-runtime-references
{
    auto dimensionSize = static_cast<uint64_t>(state.range(0));
    uint64_t spaceCells = dimensionSize  * dimensionSize;
    float* space = {new float[spaceCells]{} };
    for (unsigned int i = 0; i < spaceCells; ++i )
    {
        space[i] = static_cast<float>(i);
    }

    for (auto _ : state)
    {
        SpaceMap map = SpaceMap(space, SpaceLayout({dimensionSize, dimensionSize}));
        NavigationPath navigationPath = map.fastestRoute(map.spaceStart(), map.spaceEnd());
    }
    delete[] space;
}

auto main(int argc, char* argv[]) -> int
{
    benchmark::RegisterBenchmark("BM_fastestRoute", BM_fastestRoute)
        ->Threads(2)->Threads(4)->Threads(8)
        ->Arg(3)->Arg(64)->Arg(1024);                   // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}
