cmake_minimum_required(VERSION 3.30)
project(RompeBloques)

set(CMAKE_CXX_STANDARD 20)

set(RAYLIB_PATH ${CMAKE_SOURCE_DIR}/raylib-5.5_win64_mingw-w64)

find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(RompeBloques
        main.cpp
        auth/Login.cpp
        auth/Login.h
        src/Menu.cpp
        src/Menu.h
        Api´s/ApiClient.cpp
        Api´s/ApiClient.h
        Api´s/GameApiConfig.h
        src/RompeBloques.cpp
        src/RompeBloques.h

)

target_include_directories(RompeBloques PRIVATE
        ${RAYLIB_PATH}/include
)

target_link_directories(RompeBloques PRIVATE
        ${RAYLIB_PATH}/lib
)

target_link_libraries(RompeBloques
        raylib
        opengl32
        gdi32
        winmm
        cpr::cpr
        nlohmann_json::nlohmann_json
)