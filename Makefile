LIBS=-lpam -lcrypt
CFLAGS=-I./include -fPIC -g
LDFLAGS=-shared

all: pam_pmsca.so pmsca-enroll

clean:
	rm -f *.o pam_pmsca.so pmsca-enroll

pam_pmsca.so: pam_pmsca.o
	gcc ${LDFLAGS} $^ -o $@ ${LIBS}

pmsca-enroll: pmsca-enroll.o
	gcc $^ -o $@ ${LIBS}

%.o: src/%.c
	gcc -c ${CFLAGS} $< -o $@ ${LIBS}
