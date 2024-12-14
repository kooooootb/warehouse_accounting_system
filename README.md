<h1>BUILD</h1>
<p><code>bazel build --enable_workspace //back:was --config dbg</code></p>

<h1>CREATE COMPILATION DB</h1>
<p>With https://github.com/kooooootb/bazel-compilation-database</p>
<p><code>bazel-compilation-database/generate.py -s -- --enable_workspace</code></p>