# Robot Tests

This folder contains tests written with the widely used [Robot Framework](http://robotframework.org/).  
Most test libraries involved in our tests are documented in its excellent [User Guide](http://robotframework.org/robotframework/#user-guide).

## Prerequisites

The following prerequisites must be fulfilled in order to run robot tests.
* use Python 2 !
* install [pip](https://pip.pypa.io/en/latest/installing.html)
* install Robot Framework 2.8.7 (We don't yet support more recent version)
```
pip2 install 'robotframework==2.8.7'
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
* run test !
```
pybot ../cases/
``` 
Default s3-server binary used is ``/opt/rcs/synthese3/bin/s3-server``. This can be customized doing something like :
```
pybot --variable S3_SERVER_BINARY:/path/to/my/own/s3-server ../cases/
```
* check results in ``log.html``, ``output.xml`` and  ``report.html``.
* intermediate log and data files created for the purpose of each test case run will lie in the matching subfolder. Note that it is a good idea to clear completely this subfolder if you want to run the same test again !

