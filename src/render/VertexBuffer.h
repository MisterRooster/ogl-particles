/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include "BufferObject.h"


namespace nhahn
{
    #define MAX_VBO_ATTRIBUTES 16

    class Shader;

    enum VboAttributes
    {
        VBO_ATT_POSITION = (1 << 0),
        VBO_ATT_NORMAL = (1 << 1),
        VBO_ATT_COLOR = (1 << 2),
        VBO_ATT_TEXCOORD0 = (1 << 3),
        VBO_ATT_TEXCOORD1 = (1 << 4),
        VBO_ATT_COUNT = 5
    };

    class VertexBuffer : public BufferObject
    {
        struct VertexAttrib
        {
            const char* name;
            int size;
            bool norm;
            GLenum type;
            size_t offset;
            GLint index;

            VertexAttrib() {}
            VertexAttrib(const char* _name, int _size, bool _norm, GLenum _type, size_t _offset) :
                name(_name), size(_size), norm(_norm), type(_type), offset(_offset), index(-1) {}
        };

    public:
        VertexBuffer(int length);

        void initBuffer();

        void addAttribute(const char* name, GLint size, GLenum type, int norm);
        void setStandardAttributes(int attributes);

        void enableVertexAttributes();
        void disableVertexAttributes();

        void mapAttributes(Shader& shader);

        void draw(Shader& shader, GLenum mode, int count = 0);
        void drawIndexed(BufferObject& ibo, Shader& shader, GLenum mode, int count = 0);

        int length() const { return _length; }
        int elementSize() const { return _elementSize; }
        int attributeCount() const { return _attributeCount; }

    private:
        void enableVertexAttrib(int index);
        void disableVertexAttrib(int index);

    private:
        int _length;
        int _elementSize;

        int _attributeCount;
        VertexAttrib _attributes[MAX_VBO_ATTRIBUTES];
    };
}
