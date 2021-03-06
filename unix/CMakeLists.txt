# Create thread. Single thread
add_executable(singleThread createThread/singleThread.cpp)
target_link_libraries(singleThread pthread)

# Create thread. Multiple threads
add_executable(multipleThread createThread/multipleThreads.cpp)
target_link_libraries(multipleThread pthread)

# Synchronized Print
add_executable(synchronizedPrint synchronizedPrint/synchronizedPrint.cpp)
target_link_libraries(synchronizedPrint pthread)

# Hungry Philosophers. Mutex
add_executable(hungryPhilosophersMutexUnix hungryPhilosophers/hungryPhilosophersMutex.cpp)
target_link_libraries(hungryPhilosophersMutexUnix pthread)

# Semaphore
include_directories(${UNIX_SOURCE_PATH}/semaphore)
add_library(
        semaphoreUnixLib STATIC
        ${UNIX_SOURCE_PATH}/semaphore/semaphore.h
        ${UNIX_SOURCE_PATH}/semaphore/semaphore.cpp
)
add_executable(semaphoreUnixTest semaphore/semaphoreTest.cpp)
target_link_libraries(semaphoreUnixTest PRIVATE semaphoreUnixLib pthread)

# Hungry Philosophers. Semaphore
add_executable(hungryPhilosophersSemaphoreUnix hungryPhilosophers/hungryPhilosophersSemaphore.cpp)
target_link_libraries(hungryPhilosophersSemaphoreUnix PRIVATE semaphoreUnixLib pthread)

# Hungry Philosophers. Events
add_executable(hungryPhilosophersEventsUnix hungryPhilosophers/hungryPhilosophersEvents.cpp)
target_link_libraries(hungryPhilosophersEventsUnix PRIVATE pthread)

# Linked Queue
include_directories(${UNIX_SOURCE_PATH}/linkedQueue)
add_library(
        linkedQueueUnixLib STATIC
        ${UNIX_SOURCE_PATH}/linkedQueue/linkedQueue.h
        ${UNIX_SOURCE_PATH}/linkedQueue/linkedQueue.cpp
)
add_executable(linkedQueueUnixTest linkedQueue/linkedQueueTest.cpp)
target_link_libraries(linkedQueueUnixTest PRIVATE linkedQueueUnixLib)

# Consumer Producer
add_executable(consumerProducerSemaphoreUnix consumerProducer/consumerProducerSemaphore.cpp)
target_link_libraries(consumerProducerSemaphoreUnix PRIVATE semaphoreUnixLib linkedQueueUnixLib pthread)
