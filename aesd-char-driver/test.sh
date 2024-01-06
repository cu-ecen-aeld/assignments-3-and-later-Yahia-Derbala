#!/bin/sh

sudo ./aesdchar_unload
make clean
make
sudo ./aesdchar_load
sudo start-stop-daemon -S -n aesdsocket -a ../server/aesdsocket -- -d
sudo ../assignment-autotest/test/assignment9-buildroot/drivertest.sh
sudo ../assignment-autotest/test/assignment9-buildroot/sockettest.sh



