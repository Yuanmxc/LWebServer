cmake_minimum_required(VERSION 3.5)
project(Web_Server)

set(CMAKE_CXX_STANDARD 14)
link_libraries(-pthread)

file(GLOB WS_SOURCES
"src/base/.cpp"
"src/core/.cpp"
"src/core/provider/.cpp"
"src/net/.cpp"
"src/http/.cpp"
"src/tool/.cpp"
)

add_executable(Web_Server
src/Web_Server.cc
${WS_SOURCES}
)