# ghostwriter Unit Tests

*ghostwriter* unit tests are written using the Qt Test library.  Please see instructions below for building and running.

## Prerequisites

Before the unit tests can be built and run, you must install the following on your operating system:

* cmake (version 3.16 or later)
* Qt 5.15 or later
* Qt modules (see main README.md in root directory for Qt module dependencies)

## Linux and MacOS

To build and run all tests on Linux, enter the following commands:

    $ cd <ghostwriter path>
    $ cd test
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ctest

To run with verbose output of each assertion check, run `ctest`  with the `-V` parameter.

    $ ctest -V

## Windows

    > cd <ghostwriter path>
    > cd test
    > mkdir build
    > cd build
    > cmake -G"NMake Makefiles" ..
    > nmake
    > ctest

To run with verbose output of each assertion check, run `ctest`  with the `-V` parameter.

    $ ctest -V

