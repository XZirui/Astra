#pragma once
#include <cstdlib>

namespace astra::ast {
    class ArenaAllocator {
        struct Block {
            char *data{};
            size_t size{};
            size_t offset{};
            Block *next{};
        };

        Block *head;
        size_t blockSize{};

        static size_t alignUp(size_t size, size_t align) {
            return (size + align - 1) & ~(align - 1);
        }

        static bool hasSpace(Block *block, size_t size, size_t align) {
            size_t alignedOffset = alignUp(block->offset, align);
            return alignedOffset + size <= block->size;
        }

        Block *allocateBlock(size_t minSize) const {
            auto* block = new Block;
            block->size = minSize;
            block->data = static_cast<char *>(std::malloc(block->size));
            block->offset = 0;
            block->next = head;
            return block;
        }

    public:
        explicit ArenaAllocator(size_t blocks = 4096) : head(nullptr), blockSize(blocks) {};

        void *allocate(size_t size, size_t align) {
            if (!head || !hasSpace(head, size, align)) {
                head = allocateBlock(size > blockSize ? size : blockSize);
            }

            size_t alignedOffset = alignUp(head->offset, align);
            void *ptr = head->data + alignedOffset;
            head->offset = alignedOffset + size;
            return ptr;
        }

        ~ArenaAllocator() {
            Block *current = head;
            while (current) {
                Block *next = current->next;
                std::free(current->data);
                delete current;
                current = next;
            }
        }
    };
}