add_custom_target(
    installScript
    COMMENT "Copy Python Script"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build-scripts
    COMMAND ${Python_EXECUTABLE} sync-scripts.py ${CMAKE_CURRENT_SOURCE_DIR} ${INSTALL_DIR_OF_THIS}/python-scripts
)
add_dependencies(${CMAKE_PROJECT_NAME} installScript)