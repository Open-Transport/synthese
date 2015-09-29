# Robot Tests

This folder contains tests written with the widely used [Robot Framework](http://robotframework.org/).  
Most test libraries involved in our tests are documented in its excellent [User Guide](http://robotframework.org/robotframework/#user-guide).

## Prerequisites

The following prerequisites must be fulfilled in order to run robot tests.
* use Python 2 !
* install [pip](https://pip.pypa.io/en/latest/installing.html)
* install Robot Framework
```
pip2 install robotframework
```
* install dependencies and test libraries
```
pip2 install requests
pip2 install robotframework-requests
pip2 install robotframework-databaselibrary
```

## Running tests
* create an out-of-tree result dir and cd into it
```
$ mkdir result ; cd result
``` 
* execute cmake 
```
$ cmake ..
``` 
* eventually, some variables like WITH_INEO_TERMINUS might be added to command line to execute optional tests (server must have been built accordingly, ie with same cmake WITH_* variables)
```
$ cmake -DWITH_INEO_TERMINUS=ON ..
``` 
* run test !
```
$ make check
``` 
* check results in ``log.html``, ``output.xml`` and  ``report.html`` in appropriate subdirs.
* intermediate log and data files created for the purpose of each test case run will lie in the matching subfolder. Note that it is a good idea to clear completely this subfolder if you want to run the same test again !

