# common-library-cpp

## Features
 - design-pattern
 - file
 - json
 - log
 - queue
 - socket
 - thread

## Supported Platforms
 - requires a compiler that conforms to the C++17 standard or higher.

### Operating Systems
 - CentOS

### Compiler
 - gcc 8.3.1

### Build System
 - cmake 3.12

## Build
 - basic
   - mkdir build && cd build
   - cmake ../ && make -j4 install
   - output
     - integrate
       - ./install/integrate
     - individual
       - ./install/individual
 - test
   - mkdir build && cd build
   - cmake ../ -DENABLE_TEST=on && make -j4 install && make test
 - code coverage
   - dnf install perl-Digest-MD5
   - mkdir build && cd build
   - cmake ../ -DENABLE_TEST=on && make -j4 install && make test && make lcov_run
   - run ./coverage/html/index.html in your browser
