# Fork
add_executable(fork fork/fork.cpp)

# Thread join
add_executable(threadJoin threadJoin/threadJoin.cpp)
target_link_libraries(threadJoin pthread)

# Multiple joins
add_executable(multipleJoins multipleJoins/multipleJoins.cpp)
target_link_libraries(multipleJoins pthread)

# Hungry Philosophers. Mutex
add_executable(hungryPhilosophersMtx hungryPhilosophers/hungryPhilosophersMtx.cpp)
target_link_libraries(hungryPhilosophersMtx pthread)

# Semaphore
include_directories(${SOURCE_PATH}/semaphore)
add_library(
        semaphoreLib STATIC
        ${SOURCE_PATH}/semaphore/semaphore.h
        ${SOURCE_PATH}/semaphore/semaphore.cpp
)
add_executable(semaphore semaphore/semaphoreTest.cpp)
target_link_libraries(semaphore PRIVATE semaphoreLib pthread)

# Hungry Philosophers. Semaphore
add_executable(hungryPhilosophersSemaphore hungryPhilosophers/hungryPhilosophersSemaphore.cpp)
target_link_libraries(hungryPhilosophersSemaphore PRIVATE semaphoreLib pthread)

# Hungry Philosophers. Events
add_executable(hungryPhilosophersEvents hungryPhilosophers/hungryPhilosophersEvents.cpp)
target_link_libraries(hungryPhilosophersEvents pthread)

# Consumer & Producer. Semaphore
add_executable(consumerProducer consumerProducer/consumerProducerSemaphore.cpp)
target_link_libraries(consumerProducer PRIVATE semaphoreLib pthread)
