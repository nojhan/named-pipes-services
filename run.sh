./service2 context data out &
PID_SERVICE=$!

echo "Context" > context &

echo "data" > data &

cat out

kill $PID_SERVICE
