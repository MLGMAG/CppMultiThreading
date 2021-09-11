# Fork
add_executable(fork fork/fork.cpp)

# Thread join
add_executable(threadJoin threadJoin/threadJoin.cpp)
target_link_libraries(threadJoin pthread)

# Multiple joins
add_executable(multipleJoins multipleJoins/multipleJoins.cpp)
target_link_libraries(multipleJoins pthread)

# Hungry Philosophers. Mutex
add_executable(hungryPhilosophers hungryPhilosophers/hungryPhilosophersMtx.cpp)
target_link_libraries(hungryPhilosophers pthread)
