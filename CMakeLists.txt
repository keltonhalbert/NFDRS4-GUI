cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ version selection")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(NFDRSGUI CXX)
include(GNUInstallDirs)
# include(CTest)

set(IMGUI_DIR ./external/imgui)
set(IMPLOT_DIR ./external/implot)
set(NFDRS4_DIR ./external/NFDRS4)
set(IMGUI_USER_CONF ${CMAKE_SOURCE_DIR}/include/NFDRSGUI/imgui_conf.h)

# Libraries
if(EMSCRIPTEN)
  if(EMSCRIPTEN_VERSION VERSION_GREATER_EQUAL "3.1.57")
    set(IMGUI_EMSCRIPTEN_GLFW3 "--use-port=contrib.glfw3" CACHE STRING "Choose between --use-port=contrib.glfw3 and -sUSE_GLFW=3 for GLFW implementation (default to --use-port=contrib.glfw3)")
  else()
    # cannot use contrib.glfw3 prior to 3.1.57
    set(IMGUI_EMSCRIPTEN_GLFW3 "-sUSE_GLFW=3" CACHE STRING "Use -sUSE_GLFW=3 for GLFW implementation" FORCE)
  endif()
  set(LIBRARIES glfw NFDRS4)
  add_compile_options(-sDISABLE_EXCEPTION_CATCHING=1 -DIMGUI_DISABLE_FILE_FUNCTIONS=1 -sUSE_PTHREADS=1)
else()
    set(OpenGL_GL_PREFERENCE "GLVND")
    find_package(OpenGL REQUIRED)
    find_package(glfw3 3.3 REQUIRED)
    set(LIBRARIES glfw OpenGL::GL NFDRS4)
endif()

## Add the git submodule for NFDRS4 
add_subdirectory(external/NFDRS4)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O3 -ftree-vectorize -funroll-loops")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ftree-vectorize -funroll-loops")

## used for passing custom build options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(LOCAL_PREFIX "${CMAKE_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${LOCAL_PREFIX}")

## add all CPP files as sources
add_executable( NFDRSGUI
    ## main program file
    src/NFDRSGUI/main.cpp 
    src/NFDRSGUI/NFDRSGUI.cpp
    src/NFDRSGUI/meteogram.cpp
    src/NFDRSGUI/FW21Decoder.cpp
    src/NFDRSGUI/nfdrs_settings.cpp
    #src/NFDRSGUI/deadfuel_settings.cpp
    src/NFDRSGUI/livefuel_settings.cpp
    ## Dear Imgui files
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ## implot files
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_demo.cpp
    ${IMPLOT_DIR}/implot_items.cpp
    ## Backend files
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )

target_include_directories(NFDRSGUI PUBLIC
    ${IMGUI_DIR}
    ${IMPLOT_DIR}
    ${IMGUI_DIR}/backends
    include
    ./external/emscripten-browser-file/
    )
target_link_libraries(NFDRSGUI PUBLIC ${LIBRARIES})

set_target_properties(NFDRSGUI PROPERTIES LINKER_LANGUAGE CXX)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
target_compile_definitions(NFDRSGUI PRIVATE IMGUI_USER_CONFIG="${IMGUI_USER_CONF}")

# Emscripten settings
if(EMSCRIPTEN)
  if("${IMGUI_EMSCRIPTEN_GLFW3}" STREQUAL "--use-port=contrib.glfw3")
      target_compile_options(NFDRSGUI PUBLIC
      "${IMGUI_EMSCRIPTEN_GLFW3}"
  )
  endif()
  message(STATUS "Using ${IMGUI_EMSCRIPTEN_GLFW3} GLFW implementation")
  target_link_options(NFDRSGUI PRIVATE
    "${IMGUI_EMSCRIPTEN_GLFW3}"
    "-sWASM=1"
    "-sSHARED_MEMORY=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sSTACK_SIZE=2100000"
    "-sUSE_PTHREADS=1"
    "-sPTHREAD_POOL_SIZE=4"
    "-sNO_EXIT_RUNTIME=0"
    "-sASSERTIONS=1"
    "-sDISABLE_EXCEPTION_CATCHING=1"
    "-sNO_FILESYSTEM=1"
    "-sEXPORTED_RUNTIME_METHODS=[ccall]"
    "-sEXPORTED_FUNCTIONS=[_main,_malloc,_free]"
  )
  set_target_properties(NFDRSGUI PROPERTIES OUTPUT_NAME "index")
  # copy our custom index.html to build directory
  add_custom_command(TARGET NFDRSGUI POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_LIST_DIR}/src/web/index.html" $<TARGET_FILE_DIR:NFDRSGUI>
  )
endif()
