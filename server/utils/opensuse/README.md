# OpenSuse specific files

In this directory are placed the files used specificly for OpenSuse.

All the procedures described here where tested on an OpenSuse 13.2.

## 3rd party libs

Synthese needs some 3rd party libs, which version and local modifications are specific enough that they are incompatible with the system ones.

To create a RPM containing only the necessary 3rd party libraries for Synthese (as root):
 
	wget "http://ci.rcsmobility.com/~build/3rd/3rd-dev-Linux-x86_64.tar.gz" -O /usr/src/packages/SOURCES/3rd-dev-Linux-x86_64.tar.gz
	rpmbuild -bb 3rd-synthese.spec
	
This will download, build and create a package at ```usr/src/packages/RPMS/x86_64/3rd-synthese-1.0-0.x86_64.rpm```

A development version, containing the include files can be create using :

	rpmbuild -bb 3rd-synthese-devel.spec

## Boost

Synthese needs a specific version of the boost library (version 1.57.0 at this time).

To create a RPM containing only the necessary boost libraries for Synthese (as root) :
``` 
wget "http://downloads.sourceforge.net/project/boost/boost/1.57.0/boost_1_57_0.tar.gz" -O /usr/src/packages/SOURCES/boost_1_57_0.tar.gz
rpmbuild -bb boost-synthese.spec
``` 

This will download, build and create a package at ```usr/src/packages/RPMS/x86_64/boost-synthese-1.57.0-0.x86_64.rpm```

## Synthese

A CPack RPM generator is available, but some specific configurations are available for OpenSuse.
To use it, place yourself at the base of the server source directory and do :
```
mkdir build
cd build
cmake ../ [YOUR OPTIONS HERE] -DOPENSUSE_RPM_132=1
make
cpack -G RPM
```

This will create a file named ```synthese-[GIT REV].rpm```. 
This package depends on the boost 1.57.0 described above.

Other options for the RPM generation :
* ```CPACK_RPM_PACKAGE_NAME``` : If set, will replace the default package name (```synthese```)
* ```CPACK_RPM_FILE_NAME``` : File name for the RPM package (wthout the .rpm extension)
* ```CPACK_RPM_PACKAGE_RELEASE``` : Change the RPM package realease number
* ```CPACK_RPM_USE_GIT_VERSION``` : If at 1, will use the output of git describe as version
