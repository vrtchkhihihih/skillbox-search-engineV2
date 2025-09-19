add_executable(search_engine main.cpp)
target_link_libraries(search_engine PRIVATE searchlib)
set_target_properties(search_engine PROPERTIES FOLDER "apps")
