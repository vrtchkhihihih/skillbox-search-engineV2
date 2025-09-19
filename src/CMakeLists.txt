add_library(searchlib
    lib/ConverterJSON.cpp
    lib/InvertedIndex.cpp
    lib/SearchServer.cpp
)

target_include_directories(searchlib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(searchlib PUBLIC nlohmann_json::nlohmann_json)

target_compile_features(searchlib PUBLIC cxx_std_17)

# For MSVC warnings
if(MSVC)
  target_compile_options(searchlib PRIVATE /W4 /permissive-)
else()
  target_compile_options(searchlib PRIVATE -Wall -Wextra -Wpedantic)
endif()
