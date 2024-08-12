PROJECT_DIR="$(pwd)/../"
LIBS_DIR="${PROJECT_DIR}libs/"

cd ${PROJECT_DIR}

if ! test ./libs/libcurl; then
	git submodule update --init
fi

if ! test ./libs/zlib; then
	git submodule update --init
fi

cd ${LIBS_DIR}libcurl/

if [ ! -e "${LIBS_DIR}libcurl/builds/libcurl-x64-debug-static/lib/libcurl.a" ]; then
	./buildconf

	./configure --enable-shared --with-openssl --prefix=${LIBS_DIR}libcurl/builds/libcurl-x64-debug-static --enable-debug

	make
	make install
fi

if [ ! -e "${LIBS_DIR}libcurl/builds/libcurl-x64-release-static/lib/libcurl.a" ]; then
	./configure --enable-shared --with-openssl --prefix=${LIBS_DIR}/libcurl/builds/libcurl-x64-release-static

	make
	make install
fi

cd ${LIBS_DIR}zlib/

if [ ! -e "${LIBS_DIR}zlib/include/zlib.h" ]; then
./configure -prefix=${LIBS_DIR}zlib/ --enable-shared

make CFLAGS="-fPIC"
make install
fi

cd ${PROJECT_DIR}
cmake --no-warn-unused-cli -DSO_BUILD:STRING=True -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -B./build -G "Unix Makefiles"

if ! test ${PROJECT_DIR}build; then
	mkdir ${PROJECT_DIR}build
fi

cd ${PROJECT_DIR}build/

cmake --build . --config Release --target all -j 4 --
