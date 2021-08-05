
./service1 data out &
PID_SERVICE=$!

socat -v -u TCP-LISTEN:8423,reuseaddr,fork PIPE:./data

kill $PID_SERVICE
