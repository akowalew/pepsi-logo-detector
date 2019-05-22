# pepsi-logo-detector

Studies project - detecting logo of Pepsi in pictures. Written in modern C++ with small help of OpenCV

# Building

- Clone the repository
- Create and go to build directory: `mkdir build/ && cd build`
- Install conan dependencies: `conan install ..`
- Configure CMake: `cmake .. -DCMAKE_BUILD_TYPE=<build_type>`
- Build: `make -j4`
- Run the program and pass image to it: `./bin/find_logos assets/camera/<n>.jpg`, where `<n>` is number of image
- Optionally - run unit tests: `./bin/detector_test`
