sleeptime=1000

make
./MThread_unittest
make clean
echo "\nsleeping $sleeptime s ..."
sleep $sleeptime
