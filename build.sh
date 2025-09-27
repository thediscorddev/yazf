make
cp lib/libyazf.so ./example/
cd example
make
LD_LIBRARY_PATH=. ./bin/example