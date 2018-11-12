# Reward Algorithm
Reward and difficulty balancing algorithm development

## Fetching the source

To include all submodules, please clone like this:

```sh
git clone --recurse-submodules -j8 git@github.com:Enecuum/reward_algorithm.git
```

To get all submodules for existing clone, use:

```sh
git submodule update --init --recursive
```

## Building

The included build system uses several Linux tools to automate things (`find`, `sed`, etc), in Windows this might work, might not.

Building in the source directory (`cmake .`) is disabled (take a look in a `build` directory to see the world of pain an in-source build will bring you).

The recommended way to build is to create a `build` subdirectory and build from there:

```sh
cd reward_algorithm/
cd neural_network_tools/
mkdir build
cd build
cmake ..
# to explicitly request a debug build:
cmake -DCMAKE_BUILD_TYPE=Debug ..
# idem for release builds:
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compile
make

# Compile and run all tests detected
make check

# Run a test separately
./test_NAME
```

## Installing

The same build system (above) can be used to install the project (headers) to the system include directory (system dependent, but most likely `/usr/local/include/` by default).


```sh
sudo make install
```

For example, the `neural_networks_tools` will be installed to `/usr/local/include/neural_network_tools/{header.hpp}` and can be used in other C++ projects like this:

```C++
#include <neural_network_tools/{header.hpp}>
```

