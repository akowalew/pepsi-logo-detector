# pepsi-logo-detector

![Pepsi detection example](https://github.com/akowalew/pepsi-logo-detector/raw/master/docs/img/pepsi0.png)

## Description

The aim of this project was to write an application to perform Pepsi drinks logos detection. It was written fully in C++17 with a small help of OpenCV (only `cv::imread`, `cv::imwrite` and `cv::Mat*` are used). All of the image processing algorithm was written manually. Application works rather cross-platform, in command line, and provides a small subset of unit tests. Images used in logo detection were taken both from internet and manually, using my smartphone's camera. Algorithms parameters may be tuned using dedicated JSON files. 

## Used stuff

Except of course an OpenCV library, in order to finish the project, I've decided to use following cool stuffs:
 - `CMake` - to manage build process,
 - `Conan` - to handle 3rd party dependencies,
 - `spdlog` - excellent library for logging,
 - `Catch2` - unit testing in BDD,
 - `jsonformoderncpp` - reading/writing JSON files,
 - `CLI11` - setting up a CLI interface.

## Building and running

- Clone the repository,
- Create and go to build directory: `mkdir build/ && cd build`,
- Install conan dependencies: `conan install ..`,
- Configure CMake: `cmake .. -DCMAKE_BUILD_TYPE=<build_type>`,
- Build: `make -j4`,
- Run the program and pass image to it: `./bin/find_logos assets/camera/<n>.jpg`, where `<n>` is number of image,
- If you would like to see all steps of image processing, provide also `-v` (`--verbose`) option,
- Optionally - run unit tests: `./bin/detector_test`.
