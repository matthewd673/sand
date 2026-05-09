# sand

Yet another falling sand sim (I made one a few years ago called
[powder](https://github.com/matthewd673/powder)).

## Build and run

The only dependency is [raylib](https://www.raylib.com/index.html).
I've only built this on a macOS, but should be pretty portable.
No make file yet.

```
cc *.c `pkg-config --libs --cflags raylib` -o sand && ./sand
```

## Goals

- Simulate sand falling
