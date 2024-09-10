gcc -std=c99 tests-c.c -o tests-c -L. -lGetgudSDK -lpthread
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
