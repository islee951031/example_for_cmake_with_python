cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) #disable extened syntex by compiler

set(MSYS2_MINGW_ROOT_DIR "C:/msys64/mingw64/bin")

project(
    ExampleForCMakeWithPython
    VERSION 0.0.0.3
)

#https://cmake.org/cmake/help/latest/module/FindPython.html
find_package(Python REQUIRED Development Interpreter)
get_filename_component(PYTHON_ROOT_DIR "${Python_EXECUTABLE}" DIRECTORY)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(src)
add_subdirectory(python-scripts)

#install python Lib
add_custom_target(
    installStdLib
    COMMENT "Install python DLLs and Lib (Source: ${PYTHON_ROOT_DIR})."
    DEPENDS Python::Interpreter
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build-scripts
    COMMAND ${Python_EXECUTABLE} install-python-lib.py ${PYTHON_ROOT_DIR} ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}
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

# Copy the dynamic link libraries which depend installed MinGW.
if(MINGW)
    message(STATUS "MINGW is configured. (PATH : ${CMAKE_CXX_COMPILER})")
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    file(COPY ${MINGW_BIN_DIR}/libwinpthread-1.dll ${MINGW_BIN_DIR}/libstdc++-6.dll ${MINGW_BIN_DIR}/libgcc_s_seh-1.dll
        DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME})
endif()