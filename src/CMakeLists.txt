list(APPEND SRC_FILES
        main.cpp
        Python_Wraps.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})
add_dependencies(${CMAKE_PROJECT_NAME} Python::Interpreter)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${Python_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${Python_LIBRARIES})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${INSTALL_DIR_OF_THIS})

if(MSVC)
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4 /utf-8)
else()
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall)
endif()



add_executable(imgui_example main_example.cpp)
target_link_libraries(imgui_example PUBLIC imgui_lib)