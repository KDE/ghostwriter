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
