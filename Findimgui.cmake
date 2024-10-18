if(NOT DEFINED IMGUI_INCLUDE_DIRS)
    set(IMGUI_INCLUDE_DIRS ${IMGUI_DIR} ${IMGUI_DIR}/backends CACHE STRING "path to imgui include dirs")
endif()
if(NOT DEFINED IMGUI_LIBRARIES)
    file(GLOB IMGUI_SOURCES "${IMGUI_DIR}/imgui*.cpp")
    add_library(imgui ${IMGUI_SOURCES} ${IMGUI_BACKEND_SOURCES})
    target_include_directories(imgui PRIVATE ${IMGUI_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS}) 
    set(IMGUI_LIBRARIES imgui CACHE STRING "path to imgui libraries")
endif()