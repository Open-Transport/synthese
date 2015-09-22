# Building under GNU Linux

## Prerequisites

The following prerequisites must be fulfilled in order to build synthese server. Sample command lines are provided for Debian packaging based distributions. Only x86_64 bits architectures are supported.

* git, cmake, g++ toolchain
```
# apt-get install git g++ make cmake
```
* bzip2, zlib and mySQL client development libraries
```
# apt-get install zlib1g-dev libbz2-dev libmysqlclient-dev
``` 
* boost development library (>= 1.57). For seamless integration, a prebuilt tarball of boost 1.57 including all necessary boost components to build synthese is provided alongside synthese package.
```
# wget http://ci.rcsmobility.com/~build/boost/boost157-dev-Linux-x86_64.tar.gz
# tar xzf boost157-dev-Linux-x86_64.tar.gz -C /
``` 
* Third party development libraries. A prebuilt tarball of all third-party dependencies is provided alongside synthese package.
```
# wget http://ci.rcsmobility.com/~build/3rd/3rd-dev-Linux-x86_64.tar.gz
# tar xzf 3rd-dev-Linux-x86_64.tar.gz -C /opt/rcs/
``` 

## Synthese Server Build 

From server subfolder, in a proper local git workspace :
* create an out-of-tree build dir and cd into it
```
$ mkdir build ; cd build
``` 
* provide location of on Boost development library
```
$ export BOOST_ROOT=/opt/rcs/boost
``` 
* launch a release build
```
$ cmake .. && make
``` 
* ...or a debug build
```
$ cmake -DCMAKE_BUILD_TYPE=debug .. && make -j NUMBER_OF_CORES
``` 

That should do the job.

In case of problem, it may be useful to launch make in verbose mode to check g++ compile and link invocations. To do so : 
```
$ make VERBOSE=1
```

# Building under Microsoft Windows

## Prerequisites

Our standard Windows build is achieved from scratch under Windows 8.1 with Visual Studio 2013. Only 64 bits build instructions are provided here. Command line samples must be run under Microsoft Visual Studio 2013 command line 64 bits.

* git client must be installed, for instance from http://git-scm.com/download/win
* cmake must be installed. Download and install :  http://www.cmake.org/files/v3.1/cmake-3.1.0-rc2-win32-x86.exe
* bzip2, zlib and mySQL client development libraries must be extracted
  * http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz (extract path = BZIP2_SRC_DIR)
  * http://zlib.net/zlib-1.2.8.tar.gz (extract path = ZLIB_SRC_DIR)
  * https://extranet.rcsmobility.com/attachments/download/23511/mysql-5.5.29-winx64.zip (extract path = MYSQL_SRC_DIR)

* build and install Boost 1.57 (there is no prebuilt package for Windows right now)
  * http://sourceforge.net/projects/boost/files/boost/1.57.0/boost_1_57_0.tar.bz2 (extract path = BOOST_SRC_DIR)

* configure Boost components needed by Synthese and build them ; note that Boost.Chrono must be selected for Windows build since this is needed for Boost.Thread on this platform. 
```
> set BZIP2_SOURCE=BZIP2_SRC_DIR
> set ZLIB_SOURCE=ZLIB_SRC_DIR
> bootstrap --with-libraries=chrono,date_time,random,filesystem,iostreams,program_options,regex,system,test,thread,zlib
> bjam runtime-link=static architecture=x86 address-model=64 install
```

* Build third party development libraries needed by Synthese. Sample process is provided for zlib.
```
> git clone http://git.rcsmobility.com/rcsmobility/zlib
```
* create an out-of-tree build dir and cd into it
```
$ mkdir build ; cd build
``` 
* generate Visual Studio 2013 projects for Debug
```
> cmake -DCMAKE_BUILD_TYPE=debug -G "Visual Studio 12 2013 Win64" ..
```
* ...or for Release
```
> cmake -G "Visual Studio 12 2013 Win64" ..
```
* then launch Visual Studio 2013 ; note this is important to force x64 toolchain (32 bits toolchain version might freeze at link time). Environment variable can be set once for all as well.
```
> set PreferredToolArchitecture=x64
> devenv
```
* in Visual Studio, open build/zlib.sln and execute INSTALL target

* Repeat this process for png, expat, geos, proj, haru, iconv, spatialite (in this order!)

## Synthese Server Build 
From server subfolder, in a proper local git workspace :

* create an out-of-tree build dir and cd into it
```
$ mkdir build ; cd build
``` 
* provide location of on Boost development library
```
set BOOST_ROOT=BOOST_SRC_DIR
``` 
* set mySQL source dir :
```
set MYSQL_DIR=MYSQL_SRC_DIR
```
* generate Visual Studio 2013 projects for Debug
```
> cmake -DCMAKE_BUILD_TYPE=debug -G "Visual Studio 12 2013 Win64" ..
```
* ...or for Release
```
> cmake -G "Visual Studio 12 2013 Win64" ..

* then launch Visual Studio 2013 ; note this is important to force x64 toolchain (32 bits toolchain version might freeze at link time). Environment variable can be set once for all as well.
```
> set PreferredToolArchitecture=x64
> devenv
```

* open build/synthese3.sln solution in Visual Studio
* launch Debug or Release build, according to previously provided CMAKE_BUILD_TYPE


