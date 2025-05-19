## Build Instructions
Install the required packages:

```bash
sudo apt-get update
sudo apt-get install -y \
  cmake \
  libpoco-dev \
  libssl-dev \
  libzstd-dev \
  autoconf \
  automake \
  libtool
```

Install Databento C++ SDK
```bash
git clone https://github.com/databento/databento-cpp.git
cd databento-cpp

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX='/usr'

cmake --build build --target databento --parallel
sudo cmake --install build
cd ..
```

Build and Install fix8
```bash
tar xvzf fix8-1.4.3.tar.gz
cd fix8-1.4.3

./bootstrap
./configure
make
sudo make install
cd ..
```

Generate FIX44 Specification Headers
```bash
cd fix8-1.4.3/compiler

f8c -p FIX44 -n FIX44 ../../FIX44.xml

mkdir -p ../../generated
mv FIX44*.* ../../generated/
cd ../..
```

Build server (replayer) and client
```bash
mkdir -p build
cd build

cmake ..
cmake --build .
```

## Running the programs(s)
While within the build directory:
### Running the server (replayer)
```bash
./fix_replayer ../resources/server.xml ../resources/ticks.dbn
```
### Running the client
```bash
./fix_client ../resources/client.xml
```

