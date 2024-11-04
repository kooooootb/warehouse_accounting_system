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
RUN mkdir /was_output
# Create folder with sources
RUN mkdir /was_sources
# Create user for build
RUN groupadd -r builder && useradd -m -r -g builder builder && \
# Hand him output and sources folders ownership
    chown -R builder:builder /was_output && \
    chown -R builder:builder /was_sources 
# Switch to him
USER builder
WORKDIR /was_sources

### Prepare dependencies and build them (prevent often rebuild while changing main targets)
COPY --chown=builder .bazelrc BUILD.bazel MODULE.bazel WORKSPACE /was_sources
RUN bazel --output_base=/was_output build //:libpqxx --config dbg
RUN bazel --output_base=/was_output build //:jwt --config dbg

### Prepare project sources
COPY --chown=builder back /was_sources/back

### Build project
RUN bazel --output_base=/was_output build //back:was --config dbg

### Run generated bin by hand
CMD /was_sources/bazel-bin/back/was.runfiles/_main/back/was

### Run with bazel run
# CMD bazel --output_base=/was_output run //back:was --config dbg
