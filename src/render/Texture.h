/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include "GL/glew.h"


namespace nhahn
{
	class BufferObject;

	enum TexelType
	{
		TEXEL_FLOAT,
		TEXEL_INT,
		TEXEL_UNSIGNED,
		TEXEL_DEPTH,
		TEXEL_DEPTH_STENCIL
	};

	enum TextureType
	{
		TEXTURE_BUFFER,
		TEXTURE_1D,
		TEXTURE_CUBE,
		TEXTURE_2D,
		TEXTURE_3D
	};

	class Texture
	{
	public:
		Texture(TextureType type, int width, int height = 1, int depth = 1, int levels = 1);
		~Texture();

		void init(GLuint bufferObject = -1);

		void setFormat(TexelType texel, int channels, int chan_bytes);
		void setFilter(bool clamp, bool linear);

		void copy(void* data, int level = 0);
		void copyPbo(BufferObject& pbo, int level = 0);

		void bindImage(int unit, bool read = true, bool write = true, int level = 0);
		void bind(int unit);
		void bindAny();

		unsigned long long int size();

		TextureType type() const { return _type; }
		TexelType texelType() const { return _texelType; }

		int channels() const { return _channels; }
		int bpChannel() const { return _chanBytes; }

		GLuint glName() const { return _glName; }
		GLenum glType() const { return _glType; }

		int width() const { return _width; }
		int height() const { return _height; }

		int depth() const { return _depth; }
		int levels() const { return _levels; }

		int boundUnit() { return _boundUnit; }

		static unsigned long long int memoryUsage() { return _memoryUsage; }

	private:
		void selectUnit(int unit);
		int selectVictimUnit();

		void markAsUsed(int unit);

	private:
		static unsigned long long int _memoryUsage;

		static const int MaxTextureUnits = 80;
		static int _selectedUnit;
		static int _nextTicket;
		static int _unitTicket[MaxTextureUnits];
		static Texture* _units[MaxTextureUnits];

		TextureType _type;
		TexelType _texelType;
		int _channels;
		int _chanBytes;

		GLuint _glName;
		GLenum _glType;
		GLenum _glFormat;
		GLenum _glChanType;
		GLenum _elementType;
		int _elementSize;

		int _width;
		int _height;
		int _depth;
		int _levels;

		int _boundUnit;
	};
}