
LIBS=-lm -lwiringPi
CFLAGS=
BINARY=mmctrl
INSTALLPATH=/home/pi/bin

all: clean compile install

compile:
	gcc $(LIBS) $(CFLAGS) -o $(BINARY) mmctrl.c

install:
	cp $(BINARY) $(INSTALLPATH)/
	chmod +x $(BINARY)

run:
	sudo $(BINARY)

clean:
	rm -f *.o
	rm -f ./$(BINARY)

