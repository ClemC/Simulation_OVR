cd ../Include/gtest-1.7.0/build/

rm -f ./CMakeCache.txt
cmake ..
make
echo "\nSuccess !"
echo "Enable test in File.cpp by changing ENABLE_GTEST in 1."
echo "Sleeping 5s ..."
sleep 5