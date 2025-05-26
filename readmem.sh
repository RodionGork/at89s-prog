set -m
stty -F /dev/ttyUSB0 1200 -cstopb -parenb cs8 -icrnl -echo
cat /dev/ttyUSB0 &
echo "?10000000" >/dev/ttyUSB0 && sleep 2 && kill $$
fg