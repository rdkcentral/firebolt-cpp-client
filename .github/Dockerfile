FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends --fix-missing \
    build-essential ca-certificates \
    cmake pkg-config clang-format \
    libboost-all-dev \
    curl wget git \
    g++-multilib \
    libcurl4-openssl-dev \
    jq netcat-openbsd \
    && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

ARG NODE_VERSION="24.11.0"
ARG NODE_CHECKSUM="46da9a098973ab7ba4fca76945581ecb2eaf468de347173897044382f10e0a0a"
ENV NODE_DIR="/usr/local/node"
RUN curl -fsSL "https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz" -o /tmp/node.tar.xz \
    && echo "${NODE_CHECKSUM}  /tmp/node.tar.xz" | sha256sum -c - \
    && mkdir -p ${NODE_DIR} \
    && tar -xJf /tmp/node.tar.xz -C ${NODE_DIR} --strip-components=1 \
    && rm /tmp/node.tar.xz
ENV NODE_PATH="${NODE_DIR}/lib/node_modules"
ENV PATH="${NODE_DIR}/bin:$PATH"

RUN mkdir -p /deps

ARG DEPS_GOOGLETEST_V="1.15.2"
RUN cd /deps \
    && dir="googletest-${DEPS_GOOGLETEST_V}" \
    && i=0 && while ! curl -sL https://github.com/google/googletest/releases/download/v${DEPS_GOOGLETEST_V}/$dir.tar.gz | tar xzf -; do test "$i" -eq 5 && exit 1; i=$((i+1)); sleep 1; done \
    && cmake -B "build/$dir" \
          -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_SHARED_LIBS=ON \
          "$dir" \
    && cmake --build "build/$dir" --target install \
    && rm -rf "build/$dir" "$dir"

ARG DEPS_NLOHMANN_JSON_V="3.11.3"
RUN cd /deps \
    && dir="nlohmann-json-${DEPS_NLOHMANN_JSON_V}" \
    && git clone --depth 1 --branch "v${DEPS_NLOHMANN_JSON_V}" "https://github.com/nlohmann/json" "$dir" \
    && cmake -B "build/$dir" \
          -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_SHARED_LIBS=ON \
          -DJSON_BuildTests=OFF \
          "$dir" \
    && cmake --build "build/$dir" --target install \
    && rm -rf "build/$dir" "$dir"

ARG DEPS_JSON_SCHEMA_VALIDATOR_V="2.3.0"
RUN cd /deps \
    && dir="json-schema-validator-${DEPS_JSON_SCHEMA_VALIDATOR_V}" \
    && i=0 && while ! curl -sL https://github.com/pboettch/json-schema-validator/archive/refs/tags/${DEPS_JSON_SCHEMA_VALIDATOR_V}.tar.gz | tar xzf - ; do test "$i" -eq 5 && exit 1; i=$((i+1)); sleep 1; done \
    && cmake -B "build/$dir" \
          -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_SHARED_LIBS=ON \
          -DJSON_VALIDATOR_BUILD_TESTS=OFF \
          -DJSON_VALIDATOR_BUILD_EXAMPLES=OFF \
          "$dir" \
    && cmake --build "build/$dir" --target install \
    && rm -rf "build/$dir" "$dir"

ARG DEPS_WEBSOCKETPP_V="0.8.2"
RUN cd /deps \
    && dir="websocketpp-${DEPS_WEBSOCKETPP_V}" \
    && i=0 && while ! curl -sL https://github.com/zaphoyd/websocketpp/archive/refs/tags/${DEPS_WEBSOCKETPP_V}.tar.gz | tar xzf - ; do test "$i" -eq 5 && exit 1; i=$((i+1)); sleep 1; done \
    && cmake -B "build/$dir" \
          -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_SHARED_LIBS=ON \
          -DBUILD_TESTS=OFF \
          -DBUILD_EXAMPLES=OFF \
          "$dir" \
    && cmake --build "build/$dir" --target install \
    && rm -rf "build/$dir" "$dir"

RUN apt-get update && apt-get install -y --no-install-recommends --fix-missing \
    python3-pip \
    && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

RUN pip install --break-system-packages gcovr

# --- Transport should be always at the very end as it changes most often
ARG DEPS_TRANSPORT_V
RUN cd /deps \
    && dir="firebolt-cpp-transport-${DEPS_TRANSPORT_V}" \
    && i=0 && while ! curl -sL https://github.com/rdkcentral/firebolt-cpp-transport/releases/download/v${DEPS_TRANSPORT_V}/firebolt-cpp-transport-${DEPS_TRANSPORT_V}.tar.gz | tar xzf - ; do test "$i" -eq 5 && exit 1; i=$((i+1)); sleep 1; done \
    && cmake -B "build/$dir" \
          -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_SHARED_LIBS=ON \
          "$dir" \
    && cmake --build "build/$dir" --target install \
    && rm -rf "build/$dir" "$dir"

WORKDIR /workspace

CMD ["/bin/bash"]
