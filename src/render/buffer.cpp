#include "buffer.h"
#include <glad/glad.h>

Buffer::Buffer(size_t size) {
    create(size);
    bind();
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)size, nullptr, GL_DYNAMIC_DRAW);
}

Buffer::Buffer(const void *data, size_t size) {
    create(size);
    bind();
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)size, data, GL_STATIC_DRAW);
}

Buffer::~Buffer() {
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &bufferID);
}

void Buffer::create(size_t size) {
    this->m_size = size;
    glGenVertexArrays(1, &vertexArrayID);
    glGenBuffers(1, &bufferID);
}

void Buffer::bind() const {
    glBindVertexArray(vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
}

void Buffer::unbind() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

size_t Buffer::getSize() const {
    return m_size;
}

void Buffer::setData(const void *data, size_t offset, size_t size) {
    glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)offset, (GLsizei)size, data);
}

void Buffer::setAttribute(uint index, uint count, size_t size, size_t offset) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, (GLint)count, GL_FLOAT, GL_FALSE, (GLsizei)size, reinterpret_cast<void *>(offset));
}
