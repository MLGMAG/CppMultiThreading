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