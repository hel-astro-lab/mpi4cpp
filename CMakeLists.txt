cmake_minimum_required (VERSION 3.23)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 17)
    set (CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set (CMAKE_CXX_EXTENSIONS OFF)

project (
    mpi4cpp
    VERSION 0.1.0
    DESCRIPTION "C++ MPI wrapper library."
    HOMEPAGE_URL "https://github.com/hel-astro-lab"
    LANGUAGES CXX
)

add_library (mpi4cpp)

set_target_properties(mpi4cpp PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(mpi4cpp PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories (mpi4cpp PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_sources (
    mpi4cpp
    PUBLIC FILE_SET
           all_headers
           TYPE
           HEADERS
           BASE_DIRS
           ${PROJECT_SOURCE_DIR}/include/
           FILES
           ./include/mpi4cpp/communicator.h
           ./include/mpi4cpp/communicator_impl.h
           ./include/mpi4cpp/datatype_fwd.h
           ./include/mpi4cpp/datatype.h
           ./include/mpi4cpp/environment.h
           ./include/mpi4cpp/environment_impl.h
           ./include/mpi4cpp/exception.h
           ./include/mpi4cpp/mpi.h
           ./include/mpi4cpp/nonblocking.h
           ./include/mpi4cpp/nonblocking_impl.h
           ./include/mpi4cpp/point2point_impl.h
           ./include/mpi4cpp/request.h
           ./include/mpi4cpp/request_impl.h
           ./include/mpi4cpp/status.h
           ./include/mpi4cpp/status_impl.h
           ./include/mpi4cpp/detail/mpi_datatype_cache.h
           ./include/mpi4cpp/detail/mpi_datatype_cache_impl.h
           ./include/mpi4cpp/detail/mpl.h
)

# Use phony target for handling targets.
add_library (mpi4cpp_warnings INTERFACE)
target_compile_options (mpi4cpp_warnings
    INTERFACE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic -Wno-missing-braces>
)

find_package (MPI)

target_link_libraries (mpi4cpp PUBLIC MPI::MPI_CXX PRIVATE mpi4cpp_warnings)

if (PROJECT_IS_TOP_LEVEL)
    # Declares BUILD_TESTING option (default: ON).
    include(CTest)
    if (BUILD_TESTING)
        message (STATUS "Enabling tests...")
        add_subdirectory (test)
    endif ()
endif ()
