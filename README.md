# Cache Coherence Protocol Bench

Benchmarking code for evaluating the cost of cache coherence protocols implemented on different platforms.

## Build

### Prerequisites

> So far, the program can only be built on Linux.

* `cmake`, version `>=3.12`;
* Build system, e.g. `make` or `ninja`;
* C++ compiler supporting C++17, e.g. `GCC` or `clang`.

### Build Instructions

Clone the project to local:

```bash
git clone https://github.com/Lancern/cache-coherence-protocol-bench.git ccpb
cd ccpb
```

Create a build directory:

```bash
mkdir build
cd build
```

Then build:

```bash
cmake ..
cmake --build .
```

## Run

After a successful build, you should be able to find a file named `bench` under the build directory. Just run it :)

```bash
./bench
```

## License

This program is open-sourced under [MIT License](LICENSE).
