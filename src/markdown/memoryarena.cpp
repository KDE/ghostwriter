/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MEMORY_ARENA_CPP
#define MEMORY_ARENA_CPP

#include "memoryarena.h"

namespace ghostwriter
{
template<class T>
MemoryArena<T>::MemoryArena() :
    slotIndex(0), chunkSize(256)
{
    ;
}

template<class T>
MemoryArena<T>::MemoryArena(const size_t chunkSize) :
    slotIndex(0), chunkSize(chunkSize)
{
    ;
}

template<class T>
MemoryArena<T>::~MemoryArena()
{
    freeAll();
}

template<class T>
T *MemoryArena<T>::allocate()
{
    if (arena.empty() || (slotIndex >= (int)chunkSize)) {
        arena.push(new Chunk(chunkSize));
        slotIndex = 0;
    }

    int index = slotIndex;
    slotIndex++;
    return &(arena.top()->data()[index]);
}

template<class T>
void MemoryArena<T>::freeAll()
{
    while (!arena.isEmpty()) {
        Chunk *chunk = arena.pop();

        if (nullptr != chunk) {
            delete chunk;
        }
    }

    slotIndex = 0;
}
} // namespace ghostwriter

#endif  // MEMORY_ARENA_CPP
