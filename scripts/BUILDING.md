The build process requires CMake and is pretty straightforward:

```bash
# Make sure you're in the source root
cd {{source root}}/

# Create a new build directory if needed and enter it
mkdir build
cd build

# Simplest usage:
cmake ..
# Or, alternatively, if you want to install in Linux system directories:
cmake -DCMAKE_INSTALL_PREFIX=/usr ..

# Build and run any tests
make check

# If you want to do a test run you can supply a destination directory:
mkdir /tmp/test_root
make DESTDIR=/tmp/test_root/ install
find /tmp/test_root

# Install the library in the default location
sudo make install
```

Header only libraries can be used directly through a system include:

```C++
...
#include <{{project name}}/{{header}}.hpp>
...
```
