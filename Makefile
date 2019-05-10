CC=g++ -g -Wall

# List of source files for your thread library
THREAD_SOURCES=mutex.cpp thread.cpp cpu.cpp cv.cpp thread_impl.cpp cpu_impl.cpp mutex_impl.cpp
#THREAD_SOURCES=$(wildcard *.cpp)

# Generate the names of the thread library's object files
THREAD_OBJS=${THREAD_SOURCES:.cpp=.o}


all: libthread.o test0 test1 test2 test3 test5 test6 test7 test8 test9 test10 test11 test12 test13 test14 test15 test16 test17 test18 test19 test20 test22

# Compile the thread library and tag this compilation
libthread.o: ${THREAD_OBJS}
	./autotag.sh
	ld -r -o $@ ${THREAD_OBJS}

# Compile an application program
test%: test%.cpp libthread.o libcpu.o
	${CC} -o $@ $^ -ldl -pthread

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${THREAD_OBJS} libthread.o app
