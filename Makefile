
PORT := /dev/ttyACM0
BOARD := arduino:avr:uno
SKETCH := hardware/

flash:
	arduino-cli compile --fqbn $(BOARD) $(SKETCH)
	arduino-cli upload -p $(PORT) --fqbn $(BOARD) $(SKETCH)


read:
	screen /dev/ttyACM0 9600