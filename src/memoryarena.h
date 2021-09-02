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

#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

#include <QStack>
#include <QVector>

namespace ghostwriter
{
/**
 * This class provides a simple memory arena for allocating chunks of
 * memory for a single class/struct type.  It does not as yet support
 * parameters in class constructors.  Use this class to avoid
 * new/delete calls for each object allocated when there are many
 * objects of the same type being created and destroyed.
 */
template <class T>
class MemoryArena
{
public:
    /**
     * Constructor.
     */
    MemoryArena();

    /**
     * Constructor.  Takes the number of objects to allocate
     * for each memory chunk added to the arena.
     */
    MemoryArena(const size_t chunkSize);

    /**
     * Destructor.
     */
    ~MemoryArena();

    /**
     * Allocates a new object.
     */
    T *allocate();

    /**
     * Frees all the memory in the arena.
     */
    void freeAll();

private:
    typedef QVector<T> Chunk;
    QStack<Chunk *> arena;
    int slotIndex;
    size_t chunkSize;
};
} // namespace ghostwriter

#ifndef MEMORY_ARENA_CPP
#include "memoryarena.cpp"
#endif

#endif
