Plush
=====
This is [Plush][plush] by [Justin *"time compiler"* Frankel][jtcf] amalgamated
into a *single file library* for fun and ease of use on modern systems.

Getting Started
---------------
To compile and run the demo on any \*nix-like system with [SDL2][sdl2] installed type in
the following incantation:

```bash
$ cc -O2 `sdl2-config --cflags --libs` -lm demo.c -o build/demo
$ build/demo
```

You can also try adding `-ffast-math -funroll-loops -fomit-frame-pointer` in order to try and get some
more juice out of it. Your mileage may vary!

To compile it to [WASM][wasm] and run it on the web via [Emscripten][emscripten], issue the following
incantations instead:

```bash
$ emcc -O2 --preload-file data/ --shell-file shell.html -s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 demo.c -o build/demo.html
$ emrun build/demo.html
```

Contribute
----------
* Fork the project.
* Make your feature addition or bug fix.
* Do **not** bump the version number.
* Create a pull request. Bonus points for topic branches.

License
-------
**Plush** is provided **as-is** under the **zlib** license.
For more information see LICENSE.

[plush]: https://1014.org/code/nullsoft/plush/
[jtcf]: https://1014.org/
[mingw]: https://www.mingw-w64.org/
[sdl2]: https://www.libsdl.org/
[wasm]: https://webassembly.org/
[emscripten]: https://emscripten.org/
