#
# Marcel Bollmann, 2014
#
# Find gfsm & gfsmxl libraries by Bryan Jurish
# <http://kaskade.dwds.de/~moocow/mirror/projects/gfsm/>
#
# This CMake file was created mostly by copying from:
#   <http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries>
#
# It is likely to contain errors/bugs.
#

## Dependencies
find_package(GLIB2 REQUIRED)

## Find via pkg-config
find_package(PkgConfig)
pkg_check_modules(PC_LIBGFSM REQUIRED gfsm>=0.0.11)
pkg_check_modules(PC_LIBGFSMXL REQUIRED gfsmxl>=0.0.11)
set(LIBGFSM_DEFINITIONS ${PC_LIBGFSM_CFLAGS_OTHER} )

find_path(LIBGFSM_INCLUDE_DIR gfsm.h
          HINTS ${PC_LIBGFSM_INCLUDEDIR} ${PC_LIBGFSM_INCLUDE_DIRS}
          PATH_SUFFIXES gfsm )
find_path(LIBGFSMXL_INCLUDE_DIR gfsmxl.h
          HINTS ${PC_LIBGFSMXL_INCLUDEDIR} ${PC_LIBGFSMXL_INCLUDE_DIRS}
          PATH_SUFFIXES gfsmxl )

find_library(LIBGFSM_LIBRARY NAMES gfsm
             HINTS ${PC_LIBGFSM_LIBDIR} ${PC_LIBGFSM_LIBRARY_DIRS} )
find_library(LIBGFSMXL_LIBRARY NAMES gfsmxl
             HINTS ${PC_LIBGFSMXL_LIBDIR} ${PC_LIBGFSMXL_LIBRARY_DIRS} )

set(LIBGFSM_LIBRARIES ${LIBGFSM_LIBRARY} ${LIBGFSMXL_LIBRARY} ${GLIB2_LIBRARIES} )
set(LIBGFSM_INCLUDE_DIRS ${LIBGFSM_INCLUDE_DIR} ${LIBGFSMXL_INCLUDE_DIR} ${GLIB2_INCLUDE_DIRS} )

find_package_handle_standard_args(libgfsm DEFAULT_MSG LIBGFSM_LIBRARY LIBGFSM_INCLUDE_DIR)
find_package_handle_standard_args(libgfsmxl DEFAULT_MSG LIBGFSMXL_LIBRARY LIBGFSMXL_INCLUDE_DIR)
mark_as_advanced(LIBGFSM_INCLUDE_DIR LIBGFSM_LIBRARY)
mark_as_advanced(LIBGFSMXL_INCLUDE_DIR LIBGFSMXL_LIBRARY)
