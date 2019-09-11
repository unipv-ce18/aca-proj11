FROM debian:buster-slim

RUN set -ex \
  && apt-get update \
  && apt-get install --no-install-recommends -y \
       libopencv-core3.2 \
       libopencv-imgcodecs3.2 \
       libopencv-highgui3.2 \
  && savedAptMark="$(apt-mark showmanual)" \
  && apt-get install --no-install-recommends -y \
       ca-certificates \
       cmake \
       g++ \
       git \
       libopencv-dev \
       make \
  \
  && apt-mark showmanual | xargs apt-mark auto > /dev/null \
  && { [ -z "$savedAptMark" ] || apt-mark manual $savedAptMark; } \
  \
  && tempDir="$(mktemp -d)" \
  && cd "$tempDir" \
  \
  && git clone https://github.com/unipv-ce18/aca-proj11.git \
  && mkdir aca-proj11/build \
  && cd aca-proj11/build \
  && cmake -DCMAKE_BUILD_TYPE=Release .. \
  && make \
  && mv morph / \
  && cd / \
  && rm -rf "$tempDir" \
  && apt-get purge -y --auto-remove
