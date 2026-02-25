
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends --fix-missing \
    build-essential ca-certificates \
    cmake pkg-config clang-format \
    libboost-all-dev \
    curl wget git \
    && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends --fix-missing \
    libcurl4-openssl-dev \
    jq netcat-openbsd \
    && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

ARG NODE_VERSION="24.11.0"
ARG NODE_DISTRO="linux-x64"
RUN curl -fsSL "https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-${NODE_DISTRO}.tar.xz" -o /tmp/node.tar.xz \
    && curl -fsSL "https://nodejs.org/dist/v${NODE_VERSION}/SHASUMS256.txt" -o /tmp/SHASUMS256.txt \
    && grep "node-v${NODE_VERSION}-${NODE_DISTRO}.tar.xz" /tmp/SHASUMS256.txt | sha256sum -c - \
    && tar -xJf /tmp/node.tar.xz -C /usr/local --strip-components=1 \
    && rm /tmp/node.tar.xz /tmp/SHASUMS256.txt
ENV NODE_PATH="/usr/local/lib/node_modules"
ENV PATH="/usr/local/bin:$PATH"

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
ARG DEPS_TRANSPORT_PROTOCOL=rpc_v2
RUN cd /deps \
    && dir="firebolt-cpp-transport-${DEPS_TRANSPORT_V}" \
    && ENABLE_LEGACY_RPC_V1="OFF" \
    && i=0 && while ! curl -sL https://github.com/rdkcentral/firebolt-cpp-transport/releases/download/v${DEPS_TRANSPORT_V}/firebolt-cpp-transport-${DEPS_TRANSPORT_V}.tar.gz | tar xzf - ; do test "$i" -eq 5 && exit 1; i=$((i+1)); sleep 1; done \
    && if [ "${DEPS_TRANSPORT_PROTOCOL}" = "legacy" ]; then \
         ENABLE_LEGACY_RPC_V1="ON"; \
       fi \
    && cmake -B "build/$dir" \
          -DCMAKE_BUILD_TYPE=Release \
          -DBUILD_SHARED_LIBS=ON \
          -DENABLE_LEGACY_RPC_V1=$ENABLE_LEGACY_RPC_V1 \
          "$dir" \
    && cmake --build "build/$dir" --target install \
    && rm -rf "build/$dir" "$dir"

WORKDIR /workspace

CMD ["/bin/bash"]
