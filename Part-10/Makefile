PROGS = sctpserv01 sctpclient01

LIBS+= -lsctp -lunp

CFLAGS= -g

OBJECT= sctp_wrapper.o sctp_getnostrm.o sctp_addr_to_associd.o

CC= gcc

all: ${PROGS}

sctpserv01: sctpserv01.o ${OBJECT}
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

sctpclient01: sctpclient01.o ${OBJECT}
	${CC} ${CFLAGS} -o $@ $^ ${LIBS}

sctp_wrapper: sctp_wrapper.o
	${CC} ${CFLAGS} -o ${LIBS}

sctp_getnostrm: sctp_getnostrm.o
	${CC} ${CFLAGS} -o ${LIBS}

sctp_addr_to_associd: sctp_addr_to_associd.o
	${CC} ${CFLAGS} -o ${LIBS}
