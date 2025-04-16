all: build upload

build:
	arduino-cli compile --fqbn esp32:esp32:esp32 esp32st7789lcd.ino

upload:
	arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 esp32st7789lcd.ino

monitor:
	arduino-cli monitor -p /dev/ttyUSB0 --config 115200