ARG ALPINE_VERSION=3.22
ARG DENO_IMAGE=denoland/deno:alpine

FROM alpine:${ALPINE_VERSION} AS build

RUN apk add --no-cache build-base cmake ninja

WORKDIR /src
COPY . .

RUN cmake -S . -B build -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_LIBDIR=lib \
      -DBUILD_TESTING=ON \
      -DMORPHEUS_STEMLIB_DIR=/src/vendor/alpheios-morpheus/dist/stemlib \
 && cmake --build build \
 && ctest --test-dir build --output-on-failure \
 && cmake --install build --prefix /opt/morpheus \
 && MORPHEUS_RUNTIME_DATA_BUILD_DIR=/src/build \
      sh tools/prepare-runtime-data.sh /opt/morpheus-runtime-data

FROM alpine:${ALPINE_VERSION} AS runtime-base

RUN apk add --no-cache libgcc

COPY --from=build /opt/morpheus /opt/morpheus
COPY --from=build \
  /opt/morpheus-runtime-data \
  /opt/morpheus/share/morpheus/stemlib

ENV LD_LIBRARY_PATH=/opt/morpheus/lib
ENV MORPHLIB=/opt/morpheus/share/morpheus/stemlib

FROM ${DENO_IMAGE} AS deno-runtime

USER root
COPY --from=build /opt/morpheus /opt/morpheus
COPY --from=build \
  /opt/morpheus-runtime-data \
  /opt/morpheus/share/morpheus/stemlib
COPY --from=build \
  /src/bindings/deno \
  /opt/morpheus/share/morpheus/deno

ENV LD_LIBRARY_PATH=/opt/morpheus/lib
ENV MORPHEUS_LIBRARY=/opt/morpheus/lib/libmorpheus.so
ENV MORPHEUS_STEMLIB=/opt/morpheus/share/morpheus/stemlib

USER deno
WORKDIR /app

FROM runtime-base AS runtime

ENTRYPOINT ["/opt/morpheus/bin/cruncher"]
