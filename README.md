# IPCLib

An open source cross platform C++ socket library. 
IPCLib can be used to create connections between two processes either on the same system or on two different systems, but was mainly created to provide a connection between two processes on the same system for inter process communication. IPCLib provides a server and a client class that can be used to create this behaviour and to easily send data between the two processes. IPCLib works on Windows and Linux (tested on Ubuntu 20.04).

## Download

You can download the IPCLib library from the [Releases](https://github.com/red-panda-productions/ipc-lib/releases) page. We recommend using the latest version as it is (currently) the most stable version available. If you do not like the available packages you can always build the project from the source code using CMake with your prefered compiler.

## Using IPCLib in your project

For Windows you need to ensure that the library is included and can be found in your library files. Furthermore you need to ensure that the IPCLib dll is imported next to your program when the program is builded, which can be done with CMake. 

For Linux you might need to add the compiler option -pthread to the gcc compiler, as the IPCLib library needs to link with this library in order to work.

## Documentation

- [Documentation](https://github.com/red-panda-productions/ipc-lib/DOCUMENTATION.md)

## Project that use this library

- [SDALib](https://github.com/red-panda-productions/SDALib)
- [DAISI](https://github.com/red-panda-productions/speed-dreams)

## Notes

This project was created by students of Utrecht University.

