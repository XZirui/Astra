#pragma once
#include <cstdlib>

namespace astra::ast {
    class ArenaAllocator {
        struct Block {
            char *Data{};
            size_t Size{};
            size_t Offset{};
            Block *Next{};
        };

        Block *Head;
        size_t BlockSize{};

        static size_t alignUp(size_t Size, size_t Align) {
            return (Size + Align - 1) & ~(Align - 1);
        }

        static bool hasSpace(const Block *TargetBlock, size_t Size, size_t Align) {
            size_t AlignedOffset = alignUp(TargetBlock->Offset, Align);
            return AlignedOffset + Size <= TargetBlock->Size;
        }

        Block *allocateBlock(size_t MinSize) const {
            auto* NewBlock = new Block;
            NewBlock->Size = MinSize;
            NewBlock->Data = static_cast<char *>(std::malloc(NewBlock->Size));
            NewBlock->Offset = 0;
            NewBlock->Next = Head;
            return NewBlock;
        }

    public:
        explicit ArenaAllocator(size_t Blocks = 4096) : Head(nullptr), BlockSize(Blocks) {};

        void *allocate(size_t Size, size_t Align) {
            if (!Head || !hasSpace(Head, Size, Align)) {
                Head = allocateBlock(Size > BlockSize ? Size : BlockSize);
            }

            size_t AlignedOffset = alignUp(Head->Offset, Align);
            void *Ptr = Head->Data + AlignedOffset;
            Head->Offset = AlignedOffset + Size;
            return Ptr;
        }

        ~ArenaAllocator() {
            Block *Current = Head;
            while (Current) {
                Block *Next = Current->Next;
                std::free(Current->Data);
                delete Current;
                Current = Next;
            }
        }
    };
} // namespace astra::ast