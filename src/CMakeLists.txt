add_executable(${CMAKE_PROJECT_NAME} main.cpp py_interpreter.cpp py_console.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC imgui_lib ${Python_LIBRARIES} pybind11::embed)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Python_INCLUDE_DIRS})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${INSTALL_DIR_OF_THIS})

add_dependencies(${CMAKE_PROJECT_NAME} Python::Interpreter)