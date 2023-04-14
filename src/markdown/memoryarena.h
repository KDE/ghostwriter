/*
 * SPDX-FileCopyrightText: 2020-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
