#
# Sort kernel CMake configuration
#

include_directories(${PROJECT_SOURCE_DIR}/io/buffer)

#
# Sort Kernel
#
set(srcs
    SortKernel.cpp
)

set(incs
    SortKernel.hpp
)

PDAL_ADD_DRIVER(kernel sort "${srcs}" "${incs}" objects)
set(PDAL_TARGET_OBJECTS ${PDAL_TARGET_OBJECTS} ${objects} PARENT_SCOPE)
