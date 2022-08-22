project(sl_sample)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin/Streamline/)

# imgui
set(IMGUI_PATH    "${CMAKE_SOURCE_DIR}/third_party/imgui/")
set(IMGUI_INCLUDE "${IMGUI_PATH}" "${IMGUI_PATH}/backends/")
set(IMGUI_SRC     "${IMGUI_PATH}/backends/imgui_impl_dx12.cpp"
                  "${IMGUI_PATH}/backends/imgui_impl_win32.cpp"
                  "${IMGUI_PATH}/imgui.cpp"
                  "${IMGUI_PATH}/imgui_demo.cpp"
                  "${IMGUI_PATH}/imgui_draw.cpp"
                  "${IMGUI_PATH}/imgui_tables.cpp"
                  "${IMGUI_PATH}/imgui_widgets.cpp"
)

# tinyexr
set(TINYEXR_PATH    "${CMAKE_SOURCE_DIR}/third_party/tinyexr/")
set(TINYEXR_INCLUDE "${TINYEXR_PATH}" "${TINYEXR_PATH}/deps/miniz")
set(TINYEXR_SRC     "${TINYEXR_PATH}/deps/miniz/miniz.c")

#stb image
set(STB_PATH     "${CMAKE_SOURCE_DIR}/third_party/stb/")
set(STB_INCLUDE  "${STB_PATH}")

# streamline sdk
set(SL_BUILD_PLATFORM  "x64")
set(SL_BUILD_RELEASE   "Release_${SL_BUILD_PLATFORM}")
set(SL_PATH            "${CMAKE_SOURCE_DIR}/third_party/Streamline")
set(SL_ARTIFACTS       "${SL_PATH}/_artifacts")
set(SL_INCLUDE         "${SL_PATH}/include")
set(SL_PRODUCTION      "${SL_PATH}/bin/x64/")
set(SL_COMMON          "${SL_ARTIFACTS}/sl.common/${SL_BUILD_RELEASE}")
set(SL_COMPUTE         "${SL_ARTIFACTS}/sl.compute/${SL_BUILD_RELEASE}")
set(SL_INTERPOSER      "${SL_ARTIFACTS}/sl.interposer/${SL_BUILD_RELEASE}")
set(SL_NIS             "${SL_ARTIFACTS}/sl.nis/${SL_BUILD_RELEASE}")

# dxc
set(DXC_PATH "${CMAKE_SOURCE_DIR}/third_party/DXC/")
set(DXC_INCLUDE "${CMAKE_SOURCE_DIR}/third_party/DXC/inc/")
set(DXC_LIB "${CMAKE_SOURCE_DIR}/third_party/DXC/lib/x64/dxcompiler.lib")
set(DXC_BIN "${CMAKE_SOURCE_DIR}/third_party/DXC/bin/x64/")

set(SAMPLES_PATH "${CMAKE_SOURCE_DIR}")

set(DX_SAMPLE_SRC "src/Sample.cpp"
                  "src/AppRenderer.cpp"
                  "src/UIRenderer.cpp"
                  "src/DeviceResources.cpp"
                  "src/SLWrapper.cpp"
                  "src/BilinearUpscale.cpp"
                  "${SAMPLES_PATH}/common/Image.cpp"
                  "src/dpi.manifest"
)

set(DX_SAMPLE_HEADERS "include/AppRenderer.h"
                      "include/DeviceResources.h"
                      "include/UIRenderer.h"
                      "include/BilinearUpscale.h"
                      "include/SLWrapper.h"
                      "include/DXUtilities.h"
                      "include/d3dx12.h"
                      "${SAMPLES_PATH}/common/Image.h"
                      "${SAMPLES_PATH}/common/Utilities.h"
)

set(DX_SAMPLE_SHADERS  "src/bilinearUpscale.hlsl")

set(DX_SAMPLE_MEDIA "${SAMPLES_PATH}/media/images/1080.png"
                    "${SAMPLES_PATH}/media/images/1440.png"
                    "${SAMPLES_PATH}/media/images/1660.png"
)

set_source_files_properties(${DX_SAMPLE_SHADERS} PROPERTIES VS_COPY_TO_OUT_DIR TRUE)

source_group("external" FILES ${IMGUI_SRC} ${TINYEXR_SRC})
source_group("shaders" FILES ${DX_SAMPLE_SHADERS})
add_executable(${PROJECT_NAME} ${DX_SAMPLE_SRC} ${IMGUI_SRC} ${TINYEXR_SRC} ${DX_SAMPLE_HEADERS} ${DX_SAMPLE_SHADERS})
target_link_libraries(${PROJECT_NAME} PUBLIC 
      ${DXC_LIB} 
      ${SL_NIS}/sl.nis.lib 
      ${SL_COMMON}/sl.common.lib 
      ${SL_COMPUTE}/sl.compute.lib 
      ${SL_INTERPOSER}/sl.interposer.lib
)
target_include_directories(${PROJECT_NAME} PUBLIC include 
      ${IMGUI_INCLUDE}
      ${TINYEXR_INCLUDE} 
      ${STB_INCLUDE} 
      ${SAMPLES_PATH}/common
      ${SL_INCLUDE}
)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/NIS
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SAMPLES_PATH}"/DX12/src/bilinearUpscale.hlsl" $<TARGET_FILE_DIR:${PROJECT_NAME}>/NIS
)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DXC_BIN}/dxcompiler.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DXC_BIN}/dxil.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>/
)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/media/images
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${SAMPLES_PATH}/media/images $<TARGET_FILE_DIR:${PROJECT_NAME}>/media/images
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "$<IF:$<CONFIG:Debug>,${CMAKE_COMMAND};-E;copy_if_different;${SL_COMMON}/sl.common.dll;${SL_COMMON}/sl.common.pdb;$<TARGET_FILE_DIR:${PROJECT_NAME}>,${CMAKE_COMMAND};-E;copy_if_different;${SL_PRODUCTION}/sl.common.dll;$<TARGET_FILE_DIR:${PROJECT_NAME}>>"
    COMMAND "$<IF:$<CONFIG:Debug>,${CMAKE_COMMAND};-E;copy_if_different;${SL_INTERPOSER}/sl.interposer.dll;${SL_INTERPOSER}/sl.interposer.pdb;$<TARGET_FILE_DIR:${PROJECT_NAME}>,${CMAKE_COMMAND};-E;copy_if_different;${SL_PRODUCTION}/sl.interposer.dll;$<TARGET_FILE_DIR:${PROJECT_NAME}>>"
    COMMAND "$<IF:$<CONFIG:Debug>,${CMAKE_COMMAND};-E;copy_if_different;${SL_NIS}/sl.nis.dll;${SL_NIS}/sl.nis.pdb;$<TARGET_FILE_DIR:${PROJECT_NAME}>,${CMAKE_COMMAND};-E;copy_if_different;${SL_PRODUCTION}/sl.nis.dll;$<TARGET_FILE_DIR:${PROJECT_NAME}>>"
    COMMAND_EXPAND_LISTS
)