# Development of Getgud C++ SDK

Getgud C++ SDK allows you to integrate your game with the GetGud platform. Once integrated, you will be able to stream your matches to Getgud's cloud, as well as to send reports and update player's data. This is a development repository which contains all our private code for SDK development. This README will include all possible build instructions for different systems. To check all SDK functions and how it works you can visit one of the client repos with the docs.

Since our SDK is not open-source we have to provide build files for different systems as well as header files so clients can use our SDK. We have to provide an extensive list of all possible  builds so that there will be no problem using our SDK in any system.

When a new build is created upload it to a corresponding folder on our **public** S3 bucket with a corresponding version number.
<br>[S3 bucket](https://s3.console.aws.amazon.com/s3/buckets/getgud-sdk-files)

## Table of Contents

- [Build name conventions](https://github.com/getgud-io/cpp-getgud-sdk-dev#build-name-conventions)
- [Build for Linux](https://github.com/getgud-io/cpp-getgud-sdk-dev#build-for-linux)
- [Build for Windows](https://github.com/getgud-io/cpp-getgud-sdk-dev#build-for-windows)

## Build name conventions

Build versions can be named using a combination of semantic versioning, build date, and unique identifiers for efficient organization and communication. Here's a suggestion on how to name build versions:

`[Major].[Minor].[Patch]-[BuildType]-[BuildDate]-[ShortCommitHash]`

For example: `1.2.3-Alpha-230531-abc1234`

1. Major, Minor, and Patch - Use semantic versioning (e.g., 1.2.3) to indicate the level of changes made to the codebase. Major represents incompatible, API-changing modifications, Minor represents added functionality that is backwards-compatible, and Patch represents backwards-compatible bug fixes.

2. BuildType - Include an identifier to indicate the type of build, such as Alpha, Beta, RC (Release Candidate), or Final.

3. BuildDate - Include the date when the build was created (e.g., 211001 for October 1, 2021) to maintain a chronological record of builds.  Date format `YYMMDD`

4. ShortCommitHash - Add a short commit hash from your version control system (e.g., Git) to easily trace the build back to a specific commit.

By following this version naming convention, you will provide clear information about the content and purpose of each build, making it easier to manage and distribute software updates.

## Build for Linux

First we need to build libcurl and zlib which are used inside SDK

### libcurl
```bash
sudo apt-get install binutils make csh g++ sed gawk autoconf automake autotools-dev shtool libtool curl cmake
cd libs/libcurl/
./buildconf
./configure --disable-shared --with-openssl --prefix=FULL_PATH_TO_SDK/libs/libcurl/builds/libcurl-x64-debug-static --enable-debug
./configure --disable-shared --with-openssl --prefix=FULL_PATH_TO_SDK/libs/libcurl/builds/libcurl-x64-release-static
```
If you do not need release build you can remove the last ./configure command
Replace `FULL_PATH_TO_SDK` to your full system path to `cpp-getgud-sdk-dev` folder!

Next do 
```bash
make
make install
```
The build files will appear in the build folder, you will need mostly `libcurl.a` and `libcurl.so` for Linux!


### zlib
```bash
cd libs/zlib/
./configure -prefix=./
make
make install 
```

The build files will appear in the root folder of zlib folder, you will need mostly `zlib.a` and `zlib.so` for Linux! 


### Build SDK

Now that we have build libraries that we need, let's build SDK itself.
This build sdk .a file
```bash
cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -SFULL_PATH_TO_SDK -BFULL_PATH_TO_SDK/build -G "Unix Makefiles"
cd build 
cmake --build FULL_PATH_TO_SDK/build --config Release --target all -j 4 --
```

And this builds sdk .so file
Rm all from build folder except _build

```bash
cmake --no-warn-unused-cli -DSO_BUILD:BOOL=TRUE -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -SFULL_PATH_TO_SDK -BFULL_PATH_TO_SDK/build -G "Unix Makefiles"
cmake --build FULL_PATH_TO_SDK/build --config Release --target all -j 4 --
```

Replace `FULL_PATH_TO_SDK` with your full system path to SDK. Example: `/home/admin/cpp-getgud-sdk-dev`

Congrats, SDK is built! You will mostly need `getgudsdk.a` and `getguddsk.so` files for Linux. Sometime you will also need to use build files for zlib and libcurl that we created
 
## Build for Windows
