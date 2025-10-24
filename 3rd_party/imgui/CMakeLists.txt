list(APPEND IMGUI_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_tables.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)
list(APPEND IMGUI_INCLUDE_PATHES
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/backends
)

include(FetchContent)
FetchContent_Declare(
  glfw_lib
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw_lib)

find_package(Vulkan REQUIRED)

add_library(imgui_lib STATIC ${IMGUI_SRCS})
target_include_directories(imgui_lib PUBLIC ${IMGUI_INCLUDE_PATHES})
target_link_libraries(imgui_lib PUBLIC glfw Vulkan::Vulkan)