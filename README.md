# Common Library for C++

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
 - Ubuntu 22.04.3 LTS

### Compiler
 - gcc version 13.1.0

### Build System
 - cmake version 3.20.2

## Build
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
   - prepare
     - CentOS Stream release 9
       - `dnf install perl-FindBin`
       - `dnf install perl-Time-HiRes`
       - `dnf install perl-CPAN`
       - `perl -MCPAN -e 'install Capture::Tiny'`
       - `perl -MCPAN -e 'install DateTime'`
       - `perl -MCPAN -e 'install Date::Parse'`
     - Ubuntu 22.04.3 LTS
       - `sudo perl -MCPAN -e 'install Capture::Tiny'`
       - `sudo perl -MCPAN -e 'install DateTime'`
   - `mkdir build && cd build`
   - `cmake ../ -DENABLE_TEST=on && make -j4 install && make test && make lcov_run`
   - run `./coverage/html/index.html` in your browser
