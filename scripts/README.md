# Scripts for managing C++ header-only libraries

## Required software

```bash
# Linux bash tools (find, sed, etc)
git
CMake # Generates build, test, and documentation files
doxygen # Used to convert source comments to documentation files
latex # Used by Doxygen
python # Convert GCC RTL call graph information (`-fdump-rtl-expand`) to GraphViz .dot files
graphviz # Generates pretty call graph pictures
```

## Usage

This set of scripts is mean to be usable as-is and as a template.
As-is use can be done through Git submodule:

```bash
mkdir my_library
cd my_library/

git init

# Obtain your local copy of the scripts and enable remote updating
git submodule add -b master https://github.com/StefanHamminga/cpp_lib_scripts scripts

# A slightly more extended C++ tailored ignore file
ln -s scripts/.gitignore

# Use the default CMake configuration
ln -s scripts/CMakeLists.txt

mkdir test build
```

Optionally select a license.

To use the GCC C++ Standard Library license, as described [here](https://gcc.gnu.org/onlinedocs/libstdc++/manual/license.html), add this:

```bash
ln -s scripts/license/GPL-3 COPYING
ln -s scripts/license/COPYING.RUNTIME
$ cat << EOF > LICENSE
License
=======

The source code in this repository is distributed under the GNU General Public
License version 3, with the addition under section 7 of an exception described
in the “GCC Runtime Library Exception, version 3.1”.

Both texts are included as the files COPYING and COPYING.RUNTIME respectively.

EOF
```

If you prefer, the more liberal Apache 2.0 license is included too:

```bash
ln -s scripts/license/Apache-2.0 LICENSE
```

To update the scripts:

```bash
git submodule update --remote
```

## Authors and license

This library is written by [Stefan Hamminga](stefan@prjct.net) and may be freely shared, distributed and modified according to the terms of the Apache 2.0 license (included as the file `license/Apache-2.0`).

## Repository

[https://github.com/StefanHamminga/cpp_lib_scripts](https://github.com/StefanHamminga/cpp_lib_scripts)
