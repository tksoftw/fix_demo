# Required packages
1. cmake libpoco-dev libssl-dev libzstd-dev autoconf automake libtool

# Installing databento
git clone https://github.com/databento/databento-cpp
cd databento-cpp
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_INSTALL_PREFIX='/usr'
cmake --build build --target databento --parallel
cmake --install build

go back to cpp directory

# Setting up fix8
% tar xvzf 1.4.3.tar.gz
% cd fix8-1.4.3
% ./bootstrap
% ./configure
% make
% make install

go back to cpp directory

# Generating fix8 precompiled FIX44 specification headers
cd fix8-1.4.3/compiler
f8c -p FIX44 -n FIX44 ../../FIX44.xml
mkdir -p ../../generated
mv FIX44*.* ../../generated/

go back to cpp directory

# building replayer (server) and client
mkdir build
cd build
cmake ..
cmake --build .

stay in build dir

# run replayer
./fix_replayer ../server.xml ../../ticks.dbn
# run client:
./fix_client ../client.xml


