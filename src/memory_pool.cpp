#include "memory_pool.h"

#include <new>          // std::bad_alloc, std::nothrow
#include <vector>       // std::vector
#include <memory>       // std::unique_ptr


size_t RoundUp(size_t bytes) {
    return ((bytes + ALIGNMENT - 1) & ~(ALIGNMENT - 1));
}

// GetBucketIndex 实现
size_t GetBucketIndex(size_t bytes) {
    size_t aligned = RoundUp(bytes);
    if (aligned > MAX_BUCKET_SIZE) return NUM_BUCKETS;

    size_t index = 0;
    size_t power = ALIGNMENT;
    while (aligned > power) {
        power <<= 1;
        ++index;
    }
    return index;
}

// FixedMemoryPool 实现（多 chunk 版）
FixedMemoryPool::FixedMemoryPool(size_t block_size)
    : block_size_(RoundUp(block_size)) {}

FixedMemoryPool::~FixedMemoryPool() {
    for (char* chunk : chunks_) {
        operator delete(chunk);
    }
}

void FixedMemoryPool::allocate_new_chunk() {
    // 为整个 chunk 分配原始内存
    char* new_chunk = static_cast<char*>(operator new(block_size_ * BLOCKS_PER_CHUNK));
    chunks_.push_back(new_chunk);

    // 将所有块链接到空闲链表中
    for (size_t i = 0; i < BLOCKS_PER_CHUNK; ++i) {
        char* addr = new_chunk + i * block_size_;
        FreeBlock* block = reinterpret_cast<FreeBlock*>(addr);
        if (i + 1 < BLOCKS_PER_CHUNK) {
            char* next_addr = new_chunk + (i + 1) * block_size_;
            block->next = reinterpret_cast<FreeBlock*>(next_addr);
        } else {
            block->next = nullptr;
        }
    }

    free_list_head_ = reinterpret_cast<FreeBlock*>(new_chunk);
}

void* FixedMemoryPool::allocate() {
    if (!free_list_head_) allocate_new_chunk();

    FreeBlock* block = free_list_head_;
    free_list_head_ = block->next;
    return block;
}

void FixedMemoryPool::deallocate(void* ptr) {
    if (!ptr) return;
    FreeBlock* block = static_cast<FreeBlock*>(ptr);
    block->next = free_list_head_;
    free_list_head_ = block;
}

bool FixedMemoryPool::owns(void* ptr) const {
    if (!ptr) return false;
    for (char* chunk : chunks_) {
        if (ptr >= chunk && ptr < chunk + block_size_ * BLOCKS_PER_CHUNK) {
            return true;
        }
    }
    return false;
}

// MultiBucketMemoryPool 实现
MultiBucketMemoryPool::MultiBucketMemoryPool() {
    buckets_.reserve(NUM_BUCKETS);
    for (size_t i = 0; i < NUM_BUCKETS; ++i) {
        size_t size = ALIGNMENT * (1ULL << i);
        buckets_.emplace_back(std::make_unique<FixedMemoryPool>(size));
    }
}

void* MultiBucketMemoryPool::allocate(size_t bytes) {
    if (bytes == 0) return nullptr;

    size_t index = GetBucketIndex(bytes);
    if (index >= NUM_BUCKETS) {
        return new char[bytes];
    }

    return buckets_[index]->allocate();
}

void MultiBucketMemoryPool::deallocate(void* ptr, size_t bytes) {
    if (!ptr || bytes == 0) return;

    size_t index = GetBucketIndex(bytes);
    if (index >= NUM_BUCKETS) {
        delete[] static_cast<char*>(ptr);
        return;
    }

    buckets_[index]->deallocate(ptr);
}


