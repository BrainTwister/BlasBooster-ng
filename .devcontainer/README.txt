# Build with conan and cmake

```
conan profile detect
conan install . -s build_type=Debug --output-folder=build/gcc-debug --build=missing
conan install . -s build_type=Release --output-folder=build/gcc-release --build=missing
```
