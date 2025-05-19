cmake_minimum_required(VERSION 3.16)
project(fix8_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "/usr/local/share/fix8/contrib") # FindFix8.cmake

# imports databento::databento
find_package(databento REQUIRED)                  

# Find Fix8 (installed under /usr/local)
find_package(Fix8 REQUIRED)

# Find Poco (Fix8 dependency)
find_package(Poco REQUIRED COMPONENTS Foundation Net)

# f8c generated files
file(GLOB GENERATED_SOURCES "${CMAKE_SOURCE_DIR}/generated/*.cpp" "${CMAKE_SOURCE_DIR}/generated/*.hpp")

# Executable and sources
add_executable(fix_replayer
  src/fix_replayer.cpp
  src/BasicServer.cpp
  src/DBNReader.cpp
  src/TickToFIXAdapter.cpp
  ${GENERATED_SOURCES}
)

# Add fix_client executable
add_executable(fix_client
  src/fix_client.cpp
  ${GENERATED_SOURCES}
)

# Include headers
target_include_directories(fix_replayer PRIVATE include "${CMAKE_SOURCE_DIR}/generated")
target_include_directories(fix_client PRIVATE include "${CMAKE_SOURCE_DIR}/generated")

# Link against Fix8, Poco, pthread
target_link_libraries(fix_replayer
  PRIVATE
    fix8
    databento::databento
    Poco::Foundation
    Poco::Net
    pthread
)

target_link_libraries(fix_client
  PRIVATE
    fix8
    Poco::Foundation
    Poco::Net
    pthread
)
