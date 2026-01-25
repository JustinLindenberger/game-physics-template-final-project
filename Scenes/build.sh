
if [[ "$1" == "--all" && -d ../build ]]; then
    rm -rf ../build
    mkdir ../build
fi

if [[ ! -d ../build ]]; then
    mkdir ../build
fi

cmake -S .. -B ../build                     \
    -DCMAKE_BUILD_TYPE=Release              \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DCMAKE_CXX_FLAGS_RELEASE="             \
        -Ofast                              \
        -march=native                       \
        -flto                               \
        -ffast-math                         \
        -funroll-loops                      \
        -ftree-vectorize"

cmake --build ../build -j$(nproc)


