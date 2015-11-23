LDLIBS=-lpapi -lm
CFLAGS=-Wall -std=c99

.PHONY: all clean

TARGET = chol_dense
HEADER = prof.h

all: ${TARGET}

${TARGET}: ${HEADER}

clean:
	- rm -rf ${TARGET}

