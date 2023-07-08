# common-library-cpp

## Features
 - config
 - design-pattern
 - file
 - json
 - log
 - process
 - queue
 - socket
 - thread
 - util

## Supported Platforms
 - requires a compiler that conforms to the C++20 standard or higher.

### Operating Systems
 - CentOS Stream release 9

### Compiler
 - gcc version 12.2.1 20221121

### Build System
 - cmake version 3.20.2

## Build
 - gcc
   - `dnf install gcc-toolset-12`
   - `source /opt/rh/gcc-toolset-9/enable`
 - basic
   - `mkdir build && cd build`
   - `cmake ../ && make -j4 install`
   - output
     - integrate
       - `./install/integrate`
     - individual
       - `./install/individual`
 - test
   - `mkdir build && cd build`
   - `cmake ../ -DENABLE_TEST=on && make -j4 install && make test`
 - code coverage
   - `dnf install perl-FindBin`
   - `dnf install perl-Time-HiRes`
   - `dnf install perl-CPAN`
   - `perl -MCPAN -e 'install Capture::Tiny'`
   - `perl -MCPAN -e 'install DateTime'`
   - `perl -MCPAN -e 'install Date::Parse'`
   - `mkdir build && cd build`
   - `cmake ../ -DENABLE_TEST=on && make -j4 install && make test && make lcov_run`
   - run `./coverage/html/index.html` in your browser
