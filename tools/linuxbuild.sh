PROJECT_DIR="$(pwd)../"
LIBS_DIR="$[PROJECT_DIR]../libs/"

cd ${PROJECT_DIR}

git submodule update --init

cd ${LIBS_DIR}libcurl/

./buildconf

./configure --disable-shared --with-openssl --prefix=${LIBS_DIR}libcurl/builds/libcurl-x64-debug-static --enable-debug

make
make install

./configure --disable-shared --with-openssl --prefix=${LIBS_DIR}/libcurl/builds/libcurl-x64-release-static

make
make install

cd ${LIBS_DIR}zlib/

./configure -prefix=${LIBS_DIR}zlib/

make
make install

cd ${PROJECT_DIR}
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -SFULL_PATH_TO_SDK -BFULL_PATH_TO_SDK/build -G "Unix Makefiles"

cd ${PROJECT_DIR}build/

cmake --build FULL_PATH_TO_SDK/build --config Release --target all -j 4 --

cd ${PROJECT_DIR}

cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -SFULL_PATH_TO_SDK -BFULL_PATH_TO_SDK/build -G "Unix Makefiles"

cd ${PROJECT_DIR}build/

cmake --build FULL_PATH_TO_SDK/build --config Debug --target all -j 4 --


