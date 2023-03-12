# MSerialLib


MSerialLib is a library written in C++ for communicating over serial interface

## Building the application with MSerialLib

Project is using CMake to build a projects

### *Requirements*

* Cmake version >= <span style="color:red; font-weight: bold">3.21.2</a><span>
* Compiler Which is using ThreadModel: <span style="color:red; font-weight: bold">posix</span>
* GCC version >= <span style="color:red; font-weight: bold">8.1.0</a><span>
### *Notes*
  This build assumes that you have necessary compiler in your PATH, otherwise provide it with ```CMAKE_C_COMPILER``` and ```CMAKE_CXX_COMPILER```\
  The ```GENPATH``` is path to your generator it can be MinGW Makefile and other type of Makefiles or Ninja toolset\
The ```Generator``` is the name of the generator for which was provided appropriate executable in ```GENPATH``` for example ```MinGW``` or ```Ninja```
#### Generate CMake build files 
* Debug
```bash
cd "path/to/project/dir" && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM="GENPATH" -G Generator -S ./ -B ./debug-build
```
* Release
```bash
cd "path/to/project/dir" && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="GENPATH" -G Generator -S ./ -B ./release-build
```
#### Build the program
* Debug
```bash
cd "./debug-build" && cmake --build ./ --target wtf
```
* Release
```bash
cd "./release-build" && cmake --build ./ --target wtf
```

## Usage

```c++
bool res = a.recieve();
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

## License

[MIT](https://choosealicense.com/licenses/mit/)