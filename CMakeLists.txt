cmake_minimum_required(VERSION 3.5)
project(Web_Server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fno-strict-aliasing -O2 -Wall -Wextra -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 -Wnonnull" -D)

find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})

aux_source_directory(src/base WS_BASE)
aux_source_directory(src/server WS_SERVER)
aux_source_directory(src/http/provider WS_PROVIDER)
aux_source_directory(src/FastCgi WS_FASTCGI)
aux_source_directory(src/net WS_NET)
aux_source_directory(src/http WS_HTTP)
aux_source_directory(src/tool WS_TOOL)
aux_source_directory(src/log WS_LOG)

add_executable(Web_Server
    src/Web_Server.cc
    ${WS_SERVER}
    ${WS_PROVIDER}
    ${WS_BASE}
    ${WS_NET}
    ${WS_HTTP}
    ${WS_TOOL}
    ${WS_FASTCGI}
    ${WS_LOG}
)

target_link_libraries(Web_Server PRIVATE ${Boost_LIBRARIES} pthread)