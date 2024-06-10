#pragma once


#include <cstddef>
#include <sys/types.h>

class Buffer {
    private:
        size_t m_size = 0;

    protected:
        uint bufferID = 0, vertexArrayID = 0;

        void create(size_t size);

    public:
        explicit Buffer(size_t maxSize);
        Buffer(const void *data, size_t size);
        virtual ~Buffer();

        void bind() const;
        static void unbind();

        size_t getSize() const;

        static void setData(const void *data, size_t offset, size_t size);
        static void setAttribute(uint index, uint count, size_t size, size_t offset);
};

