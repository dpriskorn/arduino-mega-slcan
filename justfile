default: compile

compile:
	arduino-cli compile --fqbn arduino:avr:mega .

upload:
	arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:mega .

monitor:
	arduino-cli monitor -p /dev/ttyACM0 --fqbn arduino:avr:mega

sniff:
	uv pip install -e . && uv run mega-slcan-sniff
