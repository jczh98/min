# min-ray
## Build
- Install `vcpkg`
- `vcpkg install tbb nlohmann-json fmt glfw3`
- `git clone --recursive $url`
- Typing command line
```shell script
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=${vcpkg_root}/scripts/buildsystems/vcpkg.cmake ..
make -j8
```