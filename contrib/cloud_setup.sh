#/bin/sh

# Write this in your shell to use me!
# sh -c "$(curl -fsSL https://raw.githubusercontent.com/unipv-ce18/aca-proj11/master/contrib/cloud_setup.sh)"

set -ex
REPO_URL=https://github.com/unipv-ce18/aca-proj11.git

apt-get update
apt-get install --no-install-recommends -y \
       ca-certificates \
       cmake \
       g++ \
       git \
       libopencv-dev \
       make

git clone $REPO_URL
mkdir aca-proj11/build
cd aca-proj11/build

cmake -DCMAKE_BUILD_TYPE=Release ..
make
