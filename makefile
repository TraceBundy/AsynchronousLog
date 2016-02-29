OBJ=Logging.o \
	Logger.o \
	AsynchronousLogger.o \
	LogFile.o \
	Logger.o \
	HashBucket.o

speed:speed.o log.a
	g++ -Wall -std=c++11 -o $@ $^ -pthread
log.a:$(OBJ)
	ar -rc $@ $^
.cc.o:
	g++ -Wall -std=c++11 -fPIC -c $< -o $@

clean:
	rm *.o *.a speed
