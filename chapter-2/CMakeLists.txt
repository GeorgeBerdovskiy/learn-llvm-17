# First, we set the minimum required CMake version to the
# number required by LLVM, and name the project "calc"
cmake_minimum_required (VERSION 3.20.0)
project ("calc")

# Next, the LLVM package needs to be loaded, and we add the
# directory of the CMake modules provided by LLVM to the search path
find_package(LLVM REQUIRED CONFIG)
message("Found LLVM ${LLVM_PACKAGE_VERSION}, build type ${LLVM_BUILD_TYPE}")
list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})

# We also need to add the definitions and the include path
# from LLVM. The used LLVM components are mapped to the
# library names with a function call
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(llvm_libs Core)

# Lastly, we indicate that we need to include the `src` subdirectory
# in our build, as this is where all of the C++ implementation that
# was done resides
add_subdirectory("src")

# NOTE - My LLVM CMake config is at "/usr/local/opt/llvm/lib/cmake/llvm"
# ---------------------------
# Run with `cmake -GNinja -DCMAKE_C_COMPILER=/usr/local/opt/llvm/bin/clang -DCMAKE_CXX_COMPILER=/usr/local/opt/llvm/bin/clang++ -DLLVM_DIR=/usr/local/opt/llvm/lib/cmake/llvm ../`
# and `ninja` (in the "build" folder)