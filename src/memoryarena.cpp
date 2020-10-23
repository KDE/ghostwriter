/***********************************************************************
 *
 * Copyright (C) 2020 wereturtle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <QLinkedListIterator>

#include "memoryarena.h"

namespace ghostwriter
{
template<class T>
MemoryArena<T>::MemoryArena() :
    currentChunk(arena), slotIndex(0), chunkSize(256)
{
    ;
}

template<class T>
MemoryArena<T>::MemoryArena(const size_t chunkSize) :
    currentChunk(arena), slotIndex(0), chunkSize(chunkSize)
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
    if
    (
        arena.isEmpty() || (slotIndex >= (int)chunkSize)
    ) {
        if (!currentChunk.hasNext()) {
            currentChunk.insert(new Chunk(chunkSize));
        } else {
            currentChunk.next();
        }

        slotIndex = 0;
    } else if (0 == slotIndex && currentChunk.hasNext()) {
        currentChunk.next();
    }

    T *data = currentChunk.peekPrevious()->data();

    if (nullptr != data) {
        int index = slotIndex;
        slotIndex++;
        return &(data[index]);
    }

    return nullptr;
}

template<class T>
void MemoryArena<T>::freeAll()
{
    if (arena.isEmpty()) {
        return;
    }

    currentChunk.toFront();

    while (currentChunk.hasNext()) {
        Chunk *chunk = currentChunk.next();

        if (nullptr != chunk) {
            delete chunk;
        }

        currentChunk.remove();
    }

    slotIndex = 0;
}
} // namespace ghostwriter
