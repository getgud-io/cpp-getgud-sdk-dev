# Development of Getgud C++ SDK

Getgud C++ SDK allows you to integrate your game with the GetGud platform. Once integrated, you will be able to stream your matches to Getgud's cloud, as well as to send reports and update player's data. This is a development repository which contains all our private code for SDK development. This README will include all possible build instructions for different systems. To check all SDK functions and how it works you can visit one of the client repos with the docs.

Since our SDK is not open-source we have to provide build files for different systems as well as header files so clients can use our SDK. We have to provide an extensive list of all possible  builds so that there will be no problem using our SDK in any system.

When a new build is created upload it to a corresponding folder on our **public** S3 bucket with a corresponding version number.

## Table of Contents

- [Build name conventions](https://github.com/getgud-io/cpp-getgud-sdk-dev#build-name-conventions)
- [Build for Linux](https://github.com/getgud-io/cpp-getgud-sdk-dev#build-for-linux)
- [Build for Windows](https://github.com/getgud-io/cpp-getgud-sdk-dev#build-for-windows)

## Build name conventions

Build versions can be named using a combination of semantic versioning, build date, and unique identifiers for efficient organization and communication. Here's a suggestion on how to name build versions:

[Major].[Minor].[Patch]-[BuildType]-[BuildDate]-[ShortCommitHash]

For example: 1.2.3-Alpha-230531-abc1234

1. Major, Minor, and Patch - Use semantic versioning (e.g., 1.2.3) to indicate the level of changes made to the codebase. Major represents incompatible, API-changing modifications, Minor represents added functionality that is backwards-compatible, and Patch represents backwards-compatible bug fixes.

2. BuildType - Include an identifier to indicate the type of build, such as Alpha, Beta, RC (Release Candidate), or Final.

3. BuildDate - Include the date when the build was created (e.g., 20211001 for October 1, 2021) to maintain a chronological record of builds.

4. ShortCommitHash - Add a short commit hash from your version control system (e.g., Git) to easily trace the build back to a specific commit.

By following this version naming convention, you will provide clear information about the content and purpose of each build, making it easier to manage and distribute software updates.

## Build for Linux

## Build for Windows
