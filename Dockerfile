FROM debian:12 as base_image

WORKDIR /

### External deps
RUN apt-get update && \
    apt-get install -y libpq-dev libpqxx-dev git libncurses5

### Install bazel
RUN apt-get install -y apt-transport-https curl gnupg && \
    curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel-archive-keyring.gpg && \
    mv bazel-archive-keyring.gpg /usr/share/keyrings && \
    echo "deb [arch=amd64 signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list && \
    apt-get update && apt-get install -y bazel

    
### Handle build environment
# Create folder with outputs
RUN mkdir /rms_output
# Create folder with sources
RUN mkdir /rms_sources
# Create user for build
RUN groupadd -r builder && useradd -m -r -g builder builder && \
# Hand him output and sources folders ownership
    chown -R builder:builder /rms_output && \
    chown -R builder:builder /rms_sources 
# Switch to him
USER builder

### Prepare sources
COPY --chown=builder .bazelrc BUILD.bazel MODULE.bazel WORKSPACE /rms_sources
COPY --chown=builder back /rms_sources/back
WORKDIR /rms_sources

### Build project
RUN bazel --output_base=/rms_output build //back:rms314 --config dbg

### Run generated bin by hand
# WORKDIR /rms_output/bazel-bin/back/rms314.runfiles/ ### this is where final bin is stored
# CMD ["./_main/back/rms314"]

### Run with bazel run
CMD exec bazel --output_base=/rms_output run //back:rms314 --config dbg
