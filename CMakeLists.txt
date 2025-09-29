cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) #disable extened syntex by compiler

set(MSYS2_MINGW_ROOT_DIR "C:/msys64/mingw64/bin")

project(HelloEmbed)

#https://cmake.org/cmake/help/latest/module/FindPython.html
find_package(Python REQUIRED Development Interpreter)


set(Python_ROOT ${Python_EXECUTABLE})
string(REGEX REPLACE "/[^/]*$" "" Python_ROOT "${Python_ROOT}")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(src)
add_subdirectory(python-scripts)

add_custom_target(
    copyGccRuntimeDependencyDll
    COMMENT "Copy Gcc Runtime Dependency files."
    DEPENDS Python::Interpreter
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build-scripts
    COMMAND ${Python_EXECUTABLE} copy_gcc_runtime_dependency_dll.py ${MSYS2_MINGW_ROOT_DIR} ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}
)
add_dependencies(${CMAKE_PROJECT_NAME} copyGccRuntimeDependencyDll)

#install python Lib
add_custom_target(
    installStdLib
    COMMENT "Install python DLLs and Lib (Source: ${Python_ROOT})."
    DEPENDS Python::Interpreter
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build-scripts
    COMMAND ${Python_EXECUTABLE} install-python-lib.py ${Python_ROOT} ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}
)
add_dependencies(${CMAKE_PROJECT_NAME} installStdLib)

add_custom_target(
    installLib
    COMMENT "Install python site-packages."
    DEPENDS Python::Interpreter 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${Python_EXECUTABLE} -m pip install -r ${CMAKE_SOURCE_DIR}/build-scripts/requirements.txt --target ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}/python/Lib/site-packages --no-warn-script-location --upgrade
)
add_dependencies(installLib installStdLib)
add_dependencies(${CMAKE_PROJECT_NAME} installLib)