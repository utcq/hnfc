
PORT := /dev/ttyACM0
BOARD := arduino:avr:uno
SKETCH := hardware/

flash:
	arduino-cli compile --fqbn $(BOARD) $(SKETCH)
	arduino-cli upload -p $(PORT) --fqbn $(BOARD) $(SKETCH)


read:
	minicom -D /dev/ttyACM0 -b 9600