Rabit
=====
DireenTech Inc.:  www.DireenTech.com
Harry Direen, PhD, PE:  hdireen@direentech.com
Randal Direen, PhD: rdireen@direentech.com

In April of 2016 rabit was ported from C# to cpp.
Rabit (origanally HOPS) was first concieved and designed in C# under a research contract 
at the US Air Force Academy (USAFA) in the 2011 timeframe.

Is a library for building multithreaded systems. Utitilties for sending messages back and forth between Managers (each existing on its own thread) are included.

Read the document:  RabitMultitrhreadedManagementSystem.docx in the rabit/Documentation folder to gain an
understanding of rabit's design and how to use rabit.



Metrics
-------

**TravisCI:**

[![Build Status](https://travis-ci.org/rdireen/rabitcpp.svg?branch=master)](https://travis-ci.org/rdireen/rabitcpp)


Documentation
-------------

See [Documentation](Documentation/RabitMultitrhreadedManagementSystem.docx)
See [examples](examples/README.md) and the **unittests**

Build and Install
=================

Rabit relies on the CMAKE build system.

Under Windows (and possibly Linux) rabit uses the vcpkg manager (https://learn.microsoft.com/en-us/vcpkg/get_started/overview) to manage 3rd party libraries and packages.
When vcpkg is used the enviroment variable: VCPKG_ROOT must be set to the user's path to 
the vcpkg installation location.  Also the user may need to add the vcpkg installtion 
location to their path so that the vcpkg executable can be called.

Rabit depends on **Boost** 1.55 or later. 
Rabit exposes Boost in header files so all programs that use Rabit must also import Boost
and have a CMAKE find_package(Boost REQUIRED)

```
On Linux:
$ sudo apt-get install libboost-all-dev
```

Meassage and Managers Autogeneration Software and Tools
=======================================================
Rabit has code auto-genration software for generating Rabit Messages and 
Rabit Manager frameworks.
The auto-generation software relies on Pyhon 3.6 or higre and open-source sofware 
tools provided by eproxima:
[Fast DDS/RTPS] (https://www.eprosima.com/index.php/products-all/eprosima-fast-dds) 
On Linux there is a script for installing fast-dds in the Tools 
A script is contained in the Tools directory for installing fast-dds on a linux machine.
On Windows go to the eproxima website and download and install eprosima-fast-dds for Windows.

  The code generator requires Python 3.6 or higher with the following packages:
  pip3 install --user empy
  pip3 install --user pyros-genmsg
  pip3 install --user packaging
  pip3 install PyYAML

```
Other possible libraries that may be needed.
pip3 install --user toml numpy jinja2 lxml
you may need to install:  sudo apt-get install python3-libxml2
    and sudo apt install libxslt-dev
```

Building Rabit is done with [CMake](https://cmake.org/)

Do this to build:

```
$ git clone https://github.com/rdireen/rabitcpp
$ cd rabitcpp
$ mkdir build
$ cd build
$ cmake -DWITHTESTS:BOOL=ON  -DWITHEXAMPLES:BOOL=ON ..
$ make
```

You should be able to run the examples:

```
$ ./example3
```

and you should be able to run the tests

```
$ make test
```

You can install with 

```
$ make install
```

Contributing
============
Reporting bugs, suggesting features, helping with documentation, and adding to the code is very welcome. 

License
=======

Copyright (C) 2016  Randy Direen.
Rabit is licensed under GNU General Public License, version 3, a copy of this license has been provided within the COPYING file in this directory, and can also be found at <http://www.gnu.org/licenses/>.
 
