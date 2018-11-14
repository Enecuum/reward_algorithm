# Reward Algorithm
Reward and difficulty balancing algorithm development

## Fetching the source

```sh
git clone git@github.com:Enecuum/reward_algorithm.git
```

## Building

The included build system uses several Linux tools to automate things (`find`, `sed`, etc), in Windows this might work, might not.

Building in the source directory (`cmake .`) is disabled (take a look in a `build` directory to see the world of pain an in-source build will bring you).

The recommended way to build is to create a `build` subdirectory and build from there:

```sh
cd reward_algorithm/
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
