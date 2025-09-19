cmake_minimum_required(VERSION 3.16)
project(SearchEngineCpp LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Dependencies (nlohmann_json) fetched once at top-level
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_subdirectory(src)   # static/shared library with core logic
add_subdirectory(app)   # CLI executable
add_subdirectory(test) # unit tests
