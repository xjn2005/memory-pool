#include "memory_pool.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>

int main() {
    MultiBucketMemoryPool pool;

    std::cout << "Testing scoped_allocate (RAII)...\n";

    {
        auto buf = pool.scoped_allocate<char>(100);
        assert(buf.get() != nullptr);

        char* p = buf.get();
        p[0] = 'H';
        p[1] = 'e';
        p[2] = 'l';
        p[3] = 'l';
        p[4] = 'o';

        std::cout << "Allocated and wrote data: " << p << "\n";


    }  

    std::cout << "Scoped buffer auto released!\n";

    // 测试多种大小
    std::vector<std::pair<size_t, void*>> allocated;
    for (size_t size : {8, 16, 32, 64, 128, 500, 4096, 5000}) {
        void* p = pool.allocate(size);
        assert(p != nullptr);
        allocated.emplace_back(size, p);
        std::cout << "Allocated " << size << " bytes at " << p << "\n";
    }

    // 释放
    for (auto& [size, p] : allocated) {
        pool.deallocate(p, size);
    }

    std::cout << "All tests passed!\n";

    return 0;
}