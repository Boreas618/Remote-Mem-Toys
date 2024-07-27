**Install jemalloc**: [](https://github.com/jemalloc/jemalloc/blob/dev/INSTALL.md)

Note that on OS X, the configuration step should use a slightly different setting:

```
./configure --enable-prof --with-jemalloc-prefix= #There is a space here!
```

> By default, the prefix is "", except on OS X, where it is "je_". On OS X, jemalloc overlays the default malloc zone, but makes no attempt to actually replace the "malloc", "calloc", etc. symbols.
