
./service1 data > out &
PID_SERVICE=$!

echo "Hellow World!" > data &

cat out &

echo "exit" > data

# kill $PID_SERVICE
