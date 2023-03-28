# WebServer1.0

## Introduction

## Environment

- OS: Ubuntu 20.04
- Compiler: g++ 9.4.0
- CMake:  3.16.3

## Build
````
mkdir build
cd build
cmake ..
make
````
## Usage
````
./WEBSERVER -p  port_number  -s  subreactor_number  -l  log_file_path(start with ./)
````-p 9899 -s 10 -l ./ss