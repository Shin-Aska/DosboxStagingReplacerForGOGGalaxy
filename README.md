# Dosbox Staging Replacer For GOG Galaxy
## Core Module

[![Build Status](https://travis-ci.org/Shin-Aska/DosboxStagingReplacerForGOGGalaxy.svg?branch=master)](https://travis-ci.org/Shin-Aska/DosboxStagingReplacerForGOGGalaxy)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

This repository is the core module of the Dosbox Staging Replacer For GOG Galaxy project.

This is the backend application that does the work of replacing Dosbox with Dosbox Staging for GOG Galaxy installed classic games.

## About

Replaces Dosbox with Dosbox staging on GOG Galaxy installed classic games
This repository contains the core application which does the backend work

## Requirements
1. Windows OS (As currently GOG Galaxy is only available on Windows)
2. Must have GOG Galaxy installed
3. Must have Dosbox Staging installed
4. Must have a classic game installed via GOG Galaxy

## How to use

1. Download the latest release from
   the [releases page](https://github.com/Shin-Aska/DosboxStagingReplacerForGOGGalaxy/releases)
2. Extract the zip file
3. Run the application (DosboxStagingReplacerForGOGGalaxy.exe -h)

## How to build

1. Clone the repository
2. Create a build folder (in this example, we will create a folder for debug)
```bash
mkdir -p cmake-build-debug
```
3. Change to the build folder
```bash
cd cmake-build-debug
```
4. Run CMake to generate the build files
```bash
  cmake ..
```
5. Build the project
```bash
  cmake --build . --config Debug
 ```

### IDE Integration

- **CLion**: CLion automatically detects and configures the project using the provided `CMakeLists.txt`. No additional setup is required.
- **Visual Studio Code (VSCode)**: Install the **C++ extension** (by Microsoft), which includes built-in support for CMake. Once installed, open the repository folder and VSCode will detect the `CMakeLists.txt` file at the root. You can configure build settings and targets via the Command Palette (`Ctrl+Shift+P`) using commands like `CMake: Configure` and `CMake: Build`.
- **Other IDEs**: If your IDE supports CMake natively, it should recognize the root `CMakeLists.txt` file and configure the project automatically. If not, check whether a CMake plugin is available for your IDE. Alternatively, you can use CMake's `-G` option to generate project files (e.g., for Code::Blocks or Eclipse), though note that this method is deprecated by CMake since version 3.27

**Note**: It is recommended to name build directories with a `cmake-build-` prefix (e.g., `cmake-build-debug`, `cmake-build-release`). The repository's `.gitignore` is configured to exclude such folders automatically.


## Documentation

Currently, there is no documentation available, but the code is well commented and should be easy to understand.
I am planning to create a detailed wiki page for this project in the future.

## Third Party Libraries
The following third party libraries are used in this project:
- [SQLite](https://www.sqlite.org/index.html) - Used for database operations
- [argparse](https://github.com/p-ranav/argparse) - Used for command line argument parsing

It is located in the libs directory and is included in the project as a submodule for compilation purposes.

This repository only allows public domain and MIT licensed third party libraries.

In addition, it is preferred that the libraries introduced are the amalgamated versions as to increase the portability of the project.

That being said since these are amalgamated versions, this repository is responsible for updating the libraries to the latest version.

### License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
