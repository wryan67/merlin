CFLAGS=-O2 -Wall -z now
LIBS=-lm -lpthread -lasound -lwiringPi -lwiringPiMCP23x17rpi -lNeoPixelRPi
#;wiringPiDev;wiringPiPca9685;;wiringPiADS1115rpi;wiringPiPCA9635rpi;

all: chmod merlin 

merlin: merlin.cpp -lasound -lwiringPiMCP23x17rpi -lNeoPixelRPi tones.o
	@echo compiling merlin
	@rm -f bin/ARM/Debug/merlin.out
	@g++ ${CFLAGS} -o merlin merlin.cpp tones.o ${LIBS}
	@mkdir -p bin/ARM/Debug
	@cp -p merlin bin/ARM/Debug/merlin.out 2> /dev/null
	@sudo ln -fs ${HOME}/projects/merlin/merlin.sh /usr/local/bin/merlin

#g++ -o merlin.out merlin.cpp -l"pthread" -l"wiringPi" -l"wiringPiDev" -l"wiringPiPca9685" -l"wiringPiMCP23x17rpi" -l"wiringPiADS1115rpi" -l"wiringPiPCA9635rpi" -l"NeoPixelRPi" -l"asound" -Wall -z now 

tones.o: tones.cpp -lasound 
	@g++ -c -o tones.o  ${CFLAGS} tones.cpp

chmod:
	@chmod 755 listSoundCards.sh
clean:
	@rm -rf merlin bin obj
