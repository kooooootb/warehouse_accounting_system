FROM debian:12 AS base_image

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
WORKDIR /rms_sources

### Prepare dependencies and build them (prevent often rebuild while changing main targets)
COPY --chown=builder .bazelrc BUILD.bazel MODULE.bazel WORKSPACE /rms_sources
RUN bazel --output_base=/rms_output build //:libpqxx --config dbg
RUN bazel --output_base=/rms_output build //:jwt --config dbg

### Prepare project sources
COPY --chown=builder back /rms_sources/back

### Build project
RUN bazel --output_base=/rms_output build //back:rms314 --config dbg

### Run generated bin by hand
CMD /rms_sources/bazel-bin/back/rms314.runfiles/_main/back/rms314

### Run with bazel run
# CMD bazel --output_base=/rms_output run //back:rms314 --config dbg
