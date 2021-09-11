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

# Semaphore
include_directories(${UNIX_SOURCE_PATH}/semaphore)
add_library(
        semaphoreLib STATIC
        ${UNIX_SOURCE_PATH}/semaphore/semaphore.h
        ${UNIX_SOURCE_PATH}/semaphore/semaphore.cpp
        )
add_executable(semaphore semaphore/semaphoreTest.cpp)
target_link_libraries(semaphore PRIVATE semaphoreLib pthread)
