add_subdirectory(lib)
add_subdirectory(tests)

add_executable(slinger
    main.cpp
)

include_directories(slinger lib)
target_link_libraries(slinger PRIVATE slingerlib)