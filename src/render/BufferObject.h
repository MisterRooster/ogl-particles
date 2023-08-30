/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include "GL\glew.h"


namespace nhahn
{
    enum BufferType
    {
        ARRAY_BUFFER,
        ELEMENT_ARRAY_BUFFER,
        PIXEL_PACK_BUFFER,
        PIXEL_UNPACK_BUFFER,
        SHADER_STORAGE_BUFFER,
        UNIFORM_BUFFER
    };

    enum MapFlags
    {
        MAP_READ = (1 << 0),
        MAP_WRITE = (1 << 1),
        MAP_INVALIDATE_RANGE = (1 << 2),
        MAP_INVALIDATE = (1 << 3),
        MAP_FLUSH_EXPLICIT = (1 << 4),
        MAP_UNSYCHRONIZED = (1 << 5)
    };

    class BufferObject
    {
    public:
        BufferObject(BufferType type);
        BufferObject(BufferType type, GLsizei size);
        virtual ~BufferObject();

        void init(GLsizei size);

        void map(int flags = MAP_READ | MAP_WRITE | MAP_INVALIDATE);
        void mapRange(GLintptr offset, GLsizeiptr length, int flags = MAP_READ | MAP_WRITE | MAP_INVALIDATE_RANGE);
        void unmap();

        void invalidate();
        void invalidateRange(GLintptr offset, GLsizeiptr length);

        void bind();
        void unbind();

        void bindIndexed(int index);
        void bindIndexedRange(int index, GLintptr offset, GLsizeiptr size);
        void unbindIndexed(int index);

        void copyData(void* data, GLsizei size, GLenum usage);

        GLuint glName() const { return _glName; }
        GLenum type() const { return _type; }
        GLsizei size() const { return _size; }

        void* data() const { return _data; }

    private:
        BufferType _type;
        GLenum _glType;
        GLuint _glName;
        GLsizei _size;

        void* _data;
    };
}