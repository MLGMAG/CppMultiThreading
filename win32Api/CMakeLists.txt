# Simple Thread
add_executable(simpleThread simpleThread/simpleThread.cpp)

# Thread addition
add_executable(threadAddition threadAddition/threadAddition.cpp)

# Mutex
add_executable(createMutex mutex/createMutex.cpp)

# Synchronized print
add_executable(synchronizedPrintWin32Api mutex/synchronizedPrint.cpp)

# Readers & Writers. Critical section
add_executable(readersWritersCriticalSectionWin32Api readersWriters/readersWritersCriticalSection.cpp)

#Semaphore
add_executable(semaphoreWin32Test semaphore/semaphoreTest.cpp)

# Sleeping Barber. Semaphore
include_directories(${UNIX_SOURCE_PATH}/linkedQueue)
add_executable(sleepingBarberSemaphoreWin32 sleepingBarber/sleepingBarberSemaphore.cpp)
target_link_libraries(sleepingBarberSemaphoreWin32 PRIVATE linkedQueueUnixLib)
