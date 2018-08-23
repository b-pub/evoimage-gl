evoimage-gl
===========

What is evoimage-gl? 

First of all, this software is a port of Roger Alsing's "evolisa"
software to C++ and OpenGL.

Evoimage-gl is a tool that, given a target image, creates
candidates to match the target, using a genetic algorithm engine
to produce each new generation of candidates. Each candidate
image is compared to the target, and as the difference between
the two decreases, the candidate becomes the new breeder.

Images are created using semi-opaque polygons, with a variable
number of vertices. At each mutation step, a small change is made
to the current collection of polygons. For example, a polygon may
gain, it may lose a vertex, or it may change its color. After the
change is made, the newly rendered image is compared to see if it
is a better match to the target.

This software is released under the LGPL. See the COPYING file
for the license.


General Code Information

The code is split into a few subdirectories:
* inc    - Common shared headers, mostly for the mutation engine classes
* engine - The mutation engine classes
* src    - Various programs, including "evoimage-gl" itself


This project uses CMake 2.8 to generate the build files. Core
development has been on Mac OS X, but the programs use OpenGL,
GLUT, PNG (libpng 1.5) and so should be portable. Some internal
threading of dubious value utilizes the POSIX threads (pthreads).

Requirements:
* libpng 1.5 - http://libpng.sourceforge.net/index.html
* OpenGL
* GLUT
* CMake 2.8 - http://www.cmake.org/


An older implementation of evoimage (src/evoimage.cpp) uses the GD
raster library. This program is no longer supported, but if one is
interested in using it, have at it. You'll need the GD library.


Building

Once the sources are fetched or extracted, cd into the toplevel
directory. When using cmake, I like to locate the build directory
in the toplevel source directory. Thus:
```
    cd evoimage-gl
    mkdir build ; cd build
    cmake ..
```

CMake will then create build project files appropriate to the
platform.  By default on OS X, this creates Makefiles. On
Windows, it will create MSVC solution files. See CMake's -G
option to specify the desired type you need.

Once the build files are created, build it. On OS X:
```
    make
```

Running

evoimage-gl expects the "environment" file (the target image)
to be 200x200 currently. This will become arbitrary eventually.

Running evoimage-gl with no options displays its usage
information:
```
No environment image file given.
usage: evoimage [options] environment.png
Options:
    -r n    Render every n generations (default 300)
    -g n    Limit generations to n (default 10000)
    -c n    Generate n (n=1..10) children per generation (default 1)
    -s seed Initialize random number generator with seed
    -p n    Set maximum number of polygons used (default 50)
    -v n    Set maximum number of vertices/polygon used (default 20)

The environment.png file must have a resolution of 200x200.
```

Acknowledgements

Thanks to Roger Alsing for sharing is initial code in Dec 2008.
Also thanks to Michael Dougherty for his comments.

Code originally written January 2009.  
Made public December 2012.

Brent Burton
