load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

### Boost
# Famous C++ library that has given rise to many new additions to the C++ Standard Library
# Makes @boost available for use: For example, add `@boost//:algorithm` to your deps.
# For more, see https://github.com/nelhage/rules_boost and https://www.boost.org
http_archive(
    name = "com_github_nelhage_rules_boost",
    strip_prefix = "rules_boost-d8f4f9f88b12461b19354dea4df5f9ee78262067",

    # Replace the commit hash in both places (below) with the latest, rather than using the stale one here.
    # Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" in the README).
    url = "https://github.com/nelhage/rules_boost/archive/d8f4f9f88b12461b19354dea4df5f9ee78262067.tar.gz",
    # When you first run this tool, it'll recommend a sha256 hash to put here with a message like: "DEBUG: Rule 'com_github_nelhage_rules_boost' indicated that a canonical reproducible form can be obtained by modifying arguments sha256 = ..."
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()

_ALL_CONTENT = """\
filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)
"""

### Thalhammer's jwt lib
http_archive(
    name = "Thalhammer_jwt",
    build_file_content = _ALL_CONTENT,
    sha256 = "b9eb270e3ba8221e4b2bc38723c9a1cb4fa6c241a42908b9a334daff31137406",
    strip_prefix = "jwt-cpp-0.7.0",
    url = "https://github.com/Thalhammer/jwt-cpp/archive/refs/tags/v0.7.0.tar.gz",
)

### libpqxx
http_archive(
    name = "jtv_libpqxx",
    build_file_content = _ALL_CONTENT,
    sha256 = "a1fafd5f6455f6c66241fca1f35f5cb603251580b99f9a0cf1b5d0a586006f16",
    strip_prefix = "libpqxx-7.9.0",
    url = "https://github.com/jtv/libpqxx/archive/refs/tags/7.9.0.tar.gz",
)
