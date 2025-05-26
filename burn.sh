set -m
stty -F /dev/ttyUSB0 1200 -cstopb -parenb cs8 -icrnl -echo
cat /dev/ttyUSB0 &
cat blink.hex >/dev/ttyUSB0
fg