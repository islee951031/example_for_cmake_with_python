if(WIN32)
    message(STATUS "WIN32 is configured.")
else()
    message(STATUS "This is not supported build environment.")
endif()

cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) #disable extened syntex by compiler

project(
    ExampleForCMakeWithPython
    VERSION 0.0.0.6
)
set(MSYS2_MINGW_ROOT_DIR "C:/msys64/mingw64/bin")
set(INSTALL_DIR_OF_THIS ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME})

#https://cmake.org/cmake/help/latest/module/FindPython.html
find_package(Python 3.12.10 EXACT REQUIRED Development Interpreter)
get_filename_component(PYTHON_ROOT_DIR "${Python_EXECUTABLE}" DIRECTORY)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_subdirectory(3rd_party/pybind11)
add_subdirectory(3rd_party/glfw)
add_subdirectory(3rd_party/imgui)
add_subdirectory(src)
add_subdirectory(python-scripts)

#install python Lib
add_custom_target(
    installStdLib
    COMMENT "Install python DLLs and Lib (Source: ${PYTHON_ROOT_DIR})."
    DEPENDS Python::Interpreter
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build-scripts
    COMMAND ${Python_EXECUTABLE} install-python-lib.py ${PYTHON_ROOT_DIR} ${INSTALL_DIR_OF_THIS}
)
add_dependencies(${CMAKE_PROJECT_NAME} installStdLib)

# Copy the dynamic link libraries which depend installed MinGW.
if(MINGW)
    message(STATUS "MINGW is configured. (PATH : ${CMAKE_CXX_COMPILER})")
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    foreach(DLLs_DEPENDING_INSTALLED_MINGW libwinpthread-1.dll libstdc++-6.dll libgcc_s_seh-1.dll)
        file(COPY ${MINGW_BIN_DIR}/${DLLs_DEPENDING_INSTALLED_MINGW} DESTINATION ${INSTALL_DIR_OF_THIS})
    endforeach()
endif()