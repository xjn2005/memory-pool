# Memory Pool



[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE) [![GitHub issues](https://img.shields.io/github/issues/xjn2005/memory-pool?color=orange)](https://github.com/xjn2005/memory-pool/issues) [![GitHub stars](https://img.shields.io/github/stars/xjn2005/memory-pool?style=social)](https://github.com/xjn2005/memory-pool/stargazers) 

<p align="left">
  <a href="README.md">简体中文</a> | 
  <a href="README_en.md">English</a>
</p>


## English

### Overview

A fixed-size memory pool implemented in C++17 that efficiently handles frequent allocations and deallocations of many small objects. It uses bucketed fixed-size pools, chunk-based bulk allocation, and an intrusive free list for fast O(1)-like operations.

### Build & Run

```bash
mkdir build
cd build
cmake ..
cmake --build .
# Run:
# Linux/macOS: ./test_exe
# Windows: test_exe.exe
```

Tip: to switch build type, pass `-DCMAKE_BUILD_TYPE=Debug` to `cmake`.

### Notes

- `ALIGNMENT` and `alignas` ensure proper memory alignment.
- `RoundUp`/`GetBucketIndex` determine the bucket for a requested size.
- `scoped_allocate<T>` returns a `std::unique_ptr` with a custom deleter to automatically free memory.
- `allocate_new_chunk()` allocates raw memory with `operator new` and partitions it into fixed-size blocks.
- `reinterpret_cast` is used when converting `char*` to `FreeBlock*`; ensure alignment correctness.

---

## Testing

`src/test.cpp` contains a simple test that:
- exercises `scoped_allocate` (RAII) and allocations of different sizes;
- prints allocation addresses and asserts non-null pointers.

---

## Feedback & Contributing

Issues and pull requests are very welcome. If you encounter problems or have questions, please open an issue — we aim to respond promptly.

---

## License

MIT License
