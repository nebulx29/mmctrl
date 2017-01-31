
LIBS=-lm -lwiringPi
CFLAGS=
BINARY=/home/pi/bin/mmctrl

all: clean compile install

compile:
	gcc $(LIBS) $(CFLAGS) -o mmctrl mmctrl.c

install:
	cp /mnt/pishare/workspace/mmctrl/mmctrl $(BINARY)
	chmod +x $(BINARY)

run:
	sudo $(BINARY)

clean:
	rm -f *.o
	rm ./mmctrl
	rm $(BINARY)
