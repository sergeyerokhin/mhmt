# makfile configuration
NAME            = mhmt
OBJECTS         = mhmt-emit.o mhmt-globals.o mhmt-hash.o mhmt-lz.o mhmt-main.o mhmt-optimal.o mhmt-pack.o mhmt-parsearg.o mhmt-tb.o mhmt-depack.o
SRCS            = mhmt-emit.s mhmt-globals.s mhmt-hash.s mhmt-lz.s mhmt-main.s mhmt-optimal.s mhmt-pack.s mhmt-parsearg.s mhmt-tb.s mhmt-depack.s

CFLAGS          = -Os -Wextra -g

#switch the compiler (for the internal make rules)
CC              = gcc
DA		= objdump -dS

#all should be the first target. it's built when make is runwithout args
all: ${NAME} ${SRCS}

#additional rules for files
${NAME}: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ ${OBJECTS}

clean:
	rm -f ${NAME} ${OBJECTS} ${SRCS}

#project dependencies
mhmt-emit.o: ../src/mhmt-emit.c ../src/mhmt-types.h ../src/mhmt-globals.h ../src/mhmt-lz.h ../src/mhmt-optimal.h ../src/mhmt-emit.h
	${CC} ${CFLAGS} -c $<

mhmt-globals.o: ../src/mhmt-globals.c ../src/mhmt-types.h ../src/mhmt-globals.h
	${CC} ${CFLAGS} -c $<

mhmt-hash.o: ../src/mhmt-hash.c ../src/mhmt-types.h ../src/mhmt-hash.h
	${CC} ${CFLAGS} -c $<

mhmt-lz.o: ../src/mhmt-lz.c ../src/mhmt-types.h ../src/mhmt-tb.h ../src/mhmt-lz.h
	${CC} ${CFLAGS} -c $<

mhmt-main.o: ../src/mhmt-main.c ../src/mhmt-types.h ../src/mhmt-globals.h ../src/mhmt-parsearg.h
	${CC} ${CFLAGS} -c $<

mhmt-optimal.o: ../src/mhmt-optimal.c ../src/mhmt-types.h ../src/mhmt-optimal.h
	${CC} ${CFLAGS} -c $<

mhmt-pack.o: ../src/mhmt-pack.c ../src/mhmt-types.h ../src/mhmt-globals.h ../src/mhmt-pack.h ../src/mhmt-hash.h ../src/mhmt-tb.h ../src/mhmt-lz.h ../src/mhmt-optimal.h ../src/mhmt-emit.h
	${CC} ${CFLAGS} -c $<

mhmt-parsearg.o: ../src/mhmt-parsearg.c ../src/mhmt-types.h ../src/mhmt-globals.h ../src/mhmt-parsearg.h
	${CC} ${CFLAGS} -c $<

mhmt-tb.o: ../src/mhmt-tb.c ../src/mhmt-types.h ../src/mhmt-globals.h ../src/mhmt-tb.h
	${CC} ${CFLAGS} -c $<

mhmt-depack.o: ../src/mhmt-depack.c ../src/mhmt-types.h ../src/mhmt-globals.h ../src/mhmt-depack.h ../src/mhmt-depack-megalz.c ../src/mhmt-depack-hrum.c
	${CC} ${CFLAGS} -c $<

%.s: %.o
	${DA} $< >$@

