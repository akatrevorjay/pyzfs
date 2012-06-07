# This is a CMake example for Python
cmake_minimum_required(VERSION 2.6)

project(pyzfs)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
INCLUDE(CheckTypeSize)
INCLUDE(CheckStructHasMember)
INCLUDE(CheckIncludeFiles)

FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})


#include_directories([AFTER|BEFORE] [SYSTEM] dir1 dir2 ...)
INCLUDE_DIRECTORIES(/usr/include)
INCLUDE_DIRECTORIES(/usr/include/x86_64-linux-gnu)
INCLUDE_DIRECTORIES(/usr/include/libzfs)
INCLUDE_DIRECTORIES(/usr/include/libspl)

## first we add the executable that generates the table
#add_executable(MakeTable MakeTable.cxx)
#
# add the command to generate the source code
#add_custom_command (
#  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Table.h
#  COMMAND MakeTable ${CMAKE_CURRENT_BINARY_DIR}/Table.h
#  DEPENDS MakeTable
#  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
#include_directories ("${CMAKE_CURRENT_BINARY_DIR}")
#include_directories ("${PROJECT_BINARY_DIR}")
#include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# add config.h header
#add_library(pyzfs ${CMAKE_CURRENT_BINARY_DIR}/config.h )
#add_library(MathFunctions mysqrt.cxx ${CMAKE_CURRENT_BINARY_DIR}/Table.h  )


## Check for vars in zfs_config.h; this one is especially important on ZoL
CHECK_INCLUDE_FILES(${CMAKE_CURRENT_SOURCE_DIR}/zfs_config.h HAVE_IOCTL_IN_SYS_IOCTL_H)



SET(CMAKE_SWIG_FLAGS "-c++;-classic")
SET(CMAKE_SHARED_LINKER_FLAGS "-fPIC")

SET_SOURCE_FILES_PROPERTIES(pyzfs.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(pyzfs.i PROPERTIES SWIG_FLAGS "")
SWIG_ADD_MODULE(pyzfs python pyzfs.i z.cxx zfs.cxx zpool.cxx ${CMAKE_CURRENT_BINARY_DIR}/config.h)
SWIG_LINK_LIBRARIES(pyzfs ${PYTHON_LIBRARIES})
SWIG_LINK_LIBRARIES(pyzfs zfs)
SWIG_LINK_LIBRARIES(pyzfs stdc++)
set(SWIG_MODULE_pyzfs_EXTRA_DEPS common.i)

set(CMAKE_EXTRA_INCLUDE_FILES libzfs.h)
CHECK_TYPE_SIZE(zfs_source_t ZFS_SOURCE_T)
CHECK_TYPE_SIZE(sendflags_t SENDFLAGS_T)
CHECK_STRUCT_HAS_MEMBER (recvflags_t istail libzfs.h HAVE_RECVFLAGS_T_ISTAIL)
set(CMAKE_EXTRA_INCLUDE_FILES)

if(HAVE_SENDFLAGS_T)
  set(CMAKE_REQUIRED_LIBRARIES zfs)
#   CHECK_C_SOURCE_COMPILES("#include \"config.h\"
# #include <libzfs.h>
  CHECK_C_SOURCE_COMPILES("#include <libzfs.h>

boolean_t func(zfs_handle_t *one, void *two) {}

int main(int argc, char *argv[]) {
// zfs_send(openfs, fromsnap, tosnap, flags, outfd, my_eval, foo);
   sendflags_t flags;
   zfs_send(NULL,   NULL,     NULL,   flags, 0,     func, NULL);
}
" BOOLEAN_T_CALLBACK)
  set(CMAKE_REQUIRED_LIBRARIES)
endif(HAVE_SENDFLAGS_T)




CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

ENABLE_TESTING()
ADD_TEST(main python2.7 test.py)

