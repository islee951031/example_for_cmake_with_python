file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
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