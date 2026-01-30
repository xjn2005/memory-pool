#pragma once

#include <cstddef>      // size_t
#include <cstdint>      // uintptr_t
#include <vector>
#include <memory>       // std::unique_ptr
#include <functional>   // std::function 
#include <stdexcept>    // std::bad_alloc

// 常量定义（可调）
constexpr size_t ALIGNMENT = 8;
constexpr size_t MAX_BUCKET_SIZE = 4096;     // 4KB
constexpr size_t NUM_BUCKETS = 10;            // 8,16,32,...,4096
constexpr size_t BLOCKS_PER_CHUNK = 1024;

// RoundUp 和 GetBucketIndex 函数声明
size_t RoundUp(size_t bytes);
size_t GetBucketIndex(size_t bytes);

// FreeBlock 定义（侵入式链表）
union FreeBlock {
    FreeBlock* next;
    alignas(ALIGNMENT) char data[1];
};

// 单桶内存池（声明）
class FixedMemoryPool {
private:
    const size_t block_size_;
    FreeBlock* free_list_head_ = nullptr;
    std::vector<char*> chunks_;              // 多 chunk 支持
    size_t remaining_in_current_ = 0;

    void allocate_new_chunk();

public:
    explicit FixedMemoryPool(size_t block_size);
    ~FixedMemoryPool();

    void* allocate();
    void deallocate(void* ptr);
    bool owns(void* ptr) const;
};

// 多桶内存池（声明）
class MultiBucketMemoryPool {
private:
    std::vector<std::unique_ptr<FixedMemoryPool>> buckets_;

public:
    MultiBucketMemoryPool();
    ~MultiBucketMemoryPool() = default;

    void* allocate(size_t bytes);
    void deallocate(void* ptr, size_t bytes);

// RAII scoped_allocate 实现（模板函数必须在头文件里定义，或这里实现）
    template<typename T>
    std::unique_ptr<T, std::function<void(void*)>> scoped_allocate(size_t count = 1) {
        void* raw = allocate(sizeof(T) * count);
        if (!raw) {
            throw std::bad_alloc();
        }
        return std::unique_ptr<T, std::function<void(void*)>>(
            static_cast<T*>(raw),
            [this, count](void* p) {
                deallocate(p, sizeof(T) * count);
            }
        );
    }
};