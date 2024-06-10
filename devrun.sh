# pass in flags like `sh devrun.sh -f` to force a rebuild

if [ "$1" == "-f" ]; then
    rm -rf build
fi

if [ "$1" == "-d" ]; then
    rm -rf build
    mkdir build
    cd build
    cmake .. && make CMAKE_BUILD_TYPE=Debug && ./voxel-thing
    exit
fi

mkdir -p build
cd build
cmake .. && make && ./voxel-thing
