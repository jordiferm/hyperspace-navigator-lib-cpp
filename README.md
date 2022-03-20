# Hyperspace Navigator Lib

C++ header-only C++ library for N dimensional space pilots only. 

_“Any planet is 'Earth' to those that live on it.”_
**― Isaac Asimov, Pebble in the Sky**


![hyperspace-navigator-logo](https://github.com/jordiferm/hyperspace-navigator-stack1/blob/7711d6ba9691583ff6ca8c783e4ec480c2742b91/assets/logo.png)


[![Build Status](https://travis-ci.com/<url_here>)](https://travis-ci.com/mapbox/hpp-skel)
[![codecov](https://codecov.io/gh/<url_here>)](https://codecov.io/gh/mapbox/hpp-skel)


# Get started docs

## Usage

```cpp
#include <hyperspace_navigator.hpp>

using namespace hyperspace_navigator;

// Setup the space map 
float space[3 * 3] = {
    0.F, 1.F, 3.F,
    5.F, 2.F, 8.F,
    1.F, 5.F, 6.F
};

// SpaceLayout indicates the dimensions of your space
SpaceMap map = SpaceMap(space, SpaceLayout({3,3}));

// Calculate the fastest route is easy 
NavigationPath navigationPath = map.fastestRoute(map.spaceStart(), map.spaceEnd());

// Calculate the time required for a navigationPath 
float navigationTime = map.time(navigationPath);

std::clog << navigationPath;
std::clog << navigationTime;
```

## Test

```shell
# build test binaries
make

# run tests
make test
```

The default test binaries will be built in release mode. You can make Debug test binaries as well:

```shell
make clean
make debug
make test
```

Enable additional sanitizers to catch hard-to-find bugs, for example:

```shell
export LDFLAGS="-fsanitize=address,undefined,integer"
export CXXFLAGS="-fsanitize=address,undefined,integer"

make
```


## Benchmarks
Before start your interspace journey, make sure your spacecraft is fast enough doing: 

```shell
# run bench tests
make bench
```

This runs some capacity tests using [Google Benchmark](https://github.com/google/benchmark) to measure performance, it contains the following tests:
- `BM_fastestRoute/3`: Calculates the fastest route in a 2D space map with 3x3 Cells
- `BM_fastestRoute/64`: Calculates the fastest route in a 2D space map with 64x64 Cells
- `BM_fastestRoute/1024`: Calculates the fastest route in a 2D space map with 1024x1024 Cells  

#### Some notes on Google Benchmark results:
- Number of Iterations is automatically set, based on how many iterations it takes to obtain sufficient data for the bench.
- Results provide both wall time and CPU time. You may notice, the more threads used for the expensive code (up until the number of available cores on the machine), the longer CPU time will be and the shorter wall time will be. This is because the more code is shared between threads, the faster it can run in total time; the more threads doing work, the more processes running, therefore longer CPU time.

#### Compiler Optimization
To obtain a true benchmark, it may be necessary to prevent the compiler from optimizing away a value or expression. The skeleton uses Google Benchmark's internal functions to manage this. See https://github.com/google/benchmark#preventing-optimisation for more details.

#### Benchmark result sample 

```bash
2022-03-19 21:38:11
Running ./cmake-build/bench-tests
Run on (8 X 2400 MHz CPU s)
CPU Caches:
  L1 Data 32K (x4)
  L1 Instruction 32K (x4)
  L2 Unified 262K (x4)
  L3 Unified 6291K (x1)
----------------------------------------------------------------------
Benchmark                               Time           CPU Iterations
----------------------------------------------------------------------
BM_fastestRoute/3/threads:2          7920 ns      15836 ns      42910
BM_fastestRoute/3/threads:4          4186 ns      16728 ns      35992
BM_fastestRoute/3/threads:8          3824 ns      30193 ns      23320
BM_fastestRoute/64/threads:2      3674018 ns    7346023 ns         88
BM_fastestRoute/64/threads:4      1950977 ns    7800276 ns         76
BM_fastestRoute/64/threads:8      1997692 ns   15757833 ns         48
BM_fastestRoute/1024/threads:2 1041880072 ns 2078345500 ns          2
BM_fastestRoute/1024/threads:4  628989272 ns 2499747250 ns          4
BM_fastestRoute/1024/threads:8  697410764 ns 5297485500 ns          8

```


## Publishing

TODO: Explain how to publish with [Mason](https://github.com/mapbox/mason), the C++ packaging manager. 

## Versioning

This library is semantically versioned using the /include/hyperspace_navigator/version.cpp file. This defines a number of macros that can be used to check the current major, minor, or patch versions, as well as the full version string.

Here's how a downstream client would check for a particular version to use specific API methods
```cpp
#if HYPERSPACE_NAVIGATOR_VERSION_MAJOR > 2
// use version 2 api
#else
// use older verion apis
#endif
```

Here's how to check the version string
```cpp
std::cout << "version: " << HYPERSPACE_NAVIGATOR_VERSION_STRING << "/n";
// => version: 0.2.0
```

And lastly, mathematically checking for a specific version:
```cpp
#if HYPERSPACE_NAVIGATOR_VERSION_CODE > 20001
// use feature provided in v2.0.1
#endif
```

#TODO
* Describe why not used NDimension array libs like: 
  * https://www.cs.cornell.edu/~saeed/lite/html/group__array.html 
  * or Eigen: https://eigen.tuxfamily.org/index.php?title=Main_Page
  * or https://ndarray.github.io/ndarray/ndarrayTutorial.html
* Assumed array layout in memory: https://eli.thegreenplace.net/2015/memory-layout-of-multi-dimensional-arrays

# Contributing and License

Help us to improve the space navigation experience. 
TODO: <<Explain how to contribute>>

space-navigator is licensed under [GPL V3](https://www.gnu.org/licenses/gpl-3.0.en.html).
Everyone is permitted to copy and distribute verbatim copies of this license document, but changing it is not allowed. 

See [CONTRIBUTING](CONTRIBUTING.md) and [LICENSE](https://github.com/jordiferm/hyperspace-navigator-stack1/blob/f501affd3c323d23d30cfb1af7b39b3e758c0078/LICENSE) for more info.
