#!/bin/bash

case "$1" in
    start)
        echo "Starting aesdsocket daemon..."
        start-stop-daemon --start --background --pidfile /var/run/aesdsocket.pid --make-pidfile --exec /usr/bin/aesdsocket -- -d
        ;;
    stop)
        echo "Stopping aesdsocket daemon..."
        start-stop-daemon --stop --pidfile /var/run/aesdsocket.pid
        ;;
    *)
        echo "Usage: $0 {start|stop}"
        exit 1
        ;;
esac

exit 0
