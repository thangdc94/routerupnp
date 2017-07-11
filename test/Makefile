CFLAGS += -g -Wall
LDFLAGS +=
LDLIBS += -lrt

EXECUTABLES = testposix testsysv

.PHONY: all
all: $(EXECUTABLES)

testposix: testposix.o

testsysv: testsysv.o

.PHONY: clean
clean:
	$(RM) $(EXECUTABLES) *.[adios]
