# Create thread. Single thread
add_executable(singleThread createThread/singleThread.cpp)
target_link_libraries(singleThread pthread)

# Create thread. Multiple threads
add_executable(multipleThread createThread/multipleThreads.cpp)
target_link_libraries(multipleThread pthread)
