CFLAGS += -g -Wall
LDFLAGS +=
LDLIBS += 

EXECUTABLES = sample_app 

.PHONY: all
all: $(EXECUTABLES)

sample_app: sample_app.o list.o

.PHONY: clean
clean:
	$(RM) $(EXECUTABLES) *.[adios]