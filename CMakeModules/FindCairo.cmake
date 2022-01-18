# FindCairo
# Locate the cairo graphics library
#
# This sets the following variables:
#   CAIRO_FOUND
#   CAIRO_INCLUDE_DIRS
#   CAIRO_LIBRARIES
#   CAIRO_DEFINITIONS

if (CAIRO_FOUND)
    message(STATUS "Using cairo library ${CAIRO_LIBRARIES} (cached)")
else (CAIRO_FOUND)

    # Find header first, then libraries
    find_path(CAIRO_INCLUDE_DIR
        NAMES
          cairo.h
        PATHS
          /usr/include
          /usr/local/include
          /opt/local/include
          /sw/include
          $ENV{DEVLIBS_PATH}/include
        PATH_SUFFIXES
          cairo
    )

    find_library(CAIRO_LIBRARY
        NAMES
          cairo
        PATHS
          /usr/lib
          /usr/local/lib
          /opt/local/lib
          /sw/lib
          $ENV{DEVLIBS_PATH}/lib
    )

    if (CAIRO_LIBRARY)
        set(CAIRO_FOUND TRUE                         CACHE BOOL "Cairo library exists")
        set(CAIRO_INCLUDE_DIRS ${CAIRO_INCLUDE_DIR}  CACHE PATH "Cairo include directory")
        set(CAIRO_LIBRARIES ${CAIRO_LIBRARY}         CACHE FILEPATH "Cairo library")
        set(CAIRO_DEFINITIONS ""                     CACHE STRING "Cairo build definitions")

        message(STATUS "Using cairo library ${CAIRO_LIBRARIES}")
    else (CAIRO_LIBRARY)
        message(STATUS "Did not find cairo library")
    endif (CAIRO_LIBRARY)

endif (CAIRO_FOUND)

