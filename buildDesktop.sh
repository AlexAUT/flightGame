set -e

cd $(dirname $0)

mkdir -p buildDesktop
cd buildDesktop
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make flightGame -j 8
cd ..
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/
if [ $1 -gt 1 ]
then
  apitrace trace buildDesktop/flightGame/flightGame
elif [ $1 -gt 0 ]
then
  gdb buildDesktop/flightGame/flightGame
else
  buildDesktop/flightGame/flightGame
fi
