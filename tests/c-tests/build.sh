gcc -std=c99 c-tests.c -o c-tests -L. -lGetgudSDK -lpsl
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
