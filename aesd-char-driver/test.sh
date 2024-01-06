#!/bin/sh

sudo ./aesdchar_unload
make clean
make
sudo ./aesdchar_load
sudo ../assignment-autotest/test/assignment9-buildroot/drivertest.sh


