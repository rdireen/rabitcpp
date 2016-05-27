Rabit
=====

Is a library for building multithreaded systems. Utitilties for sending messages back and forth between Managers (each existing on its own thread) are included. 

Metrics
-------

**TravisCI:**

[![Build Status](https://travis-ci.org/rdireen/rabitcpp.svg?branch=master)](https://travis-ci.org/rdireen/rabitcpp)


Documentation
-------------

See [examples](examples/README.md) and the **unittests**

Build and Install
=================


Rabit depends on **Boost** 1.55 or later. 

```
$ sudo apt-get install libboost-all-dev
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
 
