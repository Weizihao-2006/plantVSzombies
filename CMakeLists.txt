cmake_minimum_required(VERSION 3.6)

set(APP_NAME PVZ) # 项目名称 [cite: 4]
project(${APP_NAME})

# 2. 强制开启 C++20 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/std:c++20 /W3)
endif()
# --- 引擎路径设置 ---
if(NOT DEFINED BUILD_ENGINE_DONE)
    set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
    set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

    include(CocosBuildSet)
    # 编译核心引擎 [cite: 4]
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# --- 资源文件处理 ---
set(GAME_RES_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# --- 源代码自动收集 (核心修改点) ---
# 自动递归搜索 Classes 下的所有 .cpp 和 .h 文件，无需手动添加
file(GLOB_RECURSE GAME_SOURCE 
    "Classes/*.cpp"
    "Classes/*.h"
)

# 包含 Windows 平台特定的入口文件 [cite: 6]
if(WINDOWS)
    list(APPEND GAME_SOURCE
         proj.win32/main.h
         proj.win32/resource.h
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
endif()

# --- 生成可执行文件 ---
if(NOT ANDROID)
    add_executable(${APP_NAME} ${GAME_SOURCE})
else()
    # Android 保持原样配置 [cite: 9, 10]
    add_library(${APP_NAME} SHARED ${GAME_SOURCE})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

# --- 链接库与头文件 ---
target_link_libraries(${APP_NAME} cocos2d)
target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

# --- 配置拷贝资源与DLL (Windows 必需) ---
setup_cocos_app_config(${APP_NAME})

if(WINDOWS)
    cocos_copy_target_dll(${APP_NAME}) # 自动拷贝相关的 .dll 文件 [cite: 11]
    cocos_get_resource_path(APP_RES_DIR ${APP_NAME})
    cocos_copy_target_res(${APP_NAME} LINK_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()