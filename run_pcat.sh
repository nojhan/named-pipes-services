
# Start the service and let it run in the background.
./pcat data &
PID_SERVICE=$!

echo "Hello world!" > data &

KILL $PID_SERVICE
