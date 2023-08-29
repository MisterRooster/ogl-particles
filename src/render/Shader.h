/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <time.h>
#include "GL/glew.h"
#include "glm/glm.hpp"


namespace nhahn
{
	#define MAX_SOURCES 16

	enum ShaderType 
	{
		INVALID_SHADER = -1,
		VERTEX_SHADER = GL_VERTEX_SHADER,
		GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
		COMPUTE_SHADER = GL_COMPUTE_SHADER
	};

	struct ShaderSource
	{
		const char* file;
		char* src;
		time_t timestamp;
	};

	/** OpenGL shader. */
	class ShaderObject
	{
	public:
		ShaderObject() : _type(INVALID_SHADER), _name(-1), _sourceCount(0) {}

		void addFile(const char* path);
		int refresh();
		void compile(ShaderType type);

		ShaderType type() const { return _type; }
		GLuint name() const { return _name; }

		int sourceCount() const { return _sourceCount; }
		const ShaderSource& source(int i) const { return _sources[i]; }

	private:
		void loadFile(ShaderSource* s, const char* path);
		void check();

	private:
		ShaderType _type;
		GLuint _name;

		int _sourceCount;
		ShaderSource _sources[MAX_SOURCES] = {};
	};


	#define MAX_SHADERS 8
	#define MAX_OUTPUTS 8
	#define MAX_VARYINGS 8
	#define MAX_UNIFORMS 32

	enum FeedbackMode
	{
		FEEDBACK_INTERLEAVED,
		FEEDBACK_SEPARATE
	};

	union UniformValue {
		int i;
		float f;
		int i2[2];
		int i3[3];
		int i4[3];
		float f2[2];
		float f3[3];
		float f4[4];
	};

	/** Can hold multiple ShaderObjects. */
	class Shader
	{
	public:
		Shader() : _shaderID(-1), _shaderCount(0), _outputCount(0), _varyingCount(0),
			_feedbackMode(FEEDBACK_INTERLEAVED), _uniformCount(0) {}
		Shader(const char* path, const char* includeFile,const char* v,
			const char* g, const char* f, int outputs);
		Shader(const char* path, const char* includeFile, const char* c);
		~Shader();

		inline GLuint id() const { return _shaderID; }

		ShaderObject* addObject();
		void addOutput(const char* name);
		void addFeedbackVarying(const char* name);
		void setFeedbackMode(FeedbackMode f);

		int refresh();
		void link();
		void bind() const;
		void unbind() const;

		void dispatch(int sizeX, int sizeY = 1, int sizeZ = 1) const;

		GLint uniform(const char* name);
		void setUniformI(const char* name, int f1);
		void setUniformI(const char* name, int f1, int f2);
		void setUniformI(const char* name, int f1, int f2, int f3);
		void setUniformI(const char* name, int f1, int f2, int f3, int f4);
		void setUniformF(const char* name, float f1);
		void setUniformF(const char* name, float f1, float f2);
		void setUniformF(const char* name, float f1, float f2, float f3);
		void setUniformF(const char* name, float f1, float f2, float f3, float f4);
		void setUniformF(const char* name, const glm::vec2& v);
		void setUniformF(const char* name, const glm::vec3& v);
		void setUniformF(const char* name, const glm::vec4& v);
		void setUniformMat(const char* name, const glm::mat4& v, bool transpose);

	private:
		int uniformIndex(const char* name);
		void check();

	private:
		GLuint _shaderID;

		int _shaderCount;
		ShaderObject _shaders[MAX_SHADERS];
		int _outputCount;
		const char* _outputs[MAX_OUTPUTS];
		int _varyingCount;
		const char* _varyings[MAX_VARYINGS];
		FeedbackMode _feedbackMode;

		int _uniformCount;
		int _uniformHash[MAX_UNIFORMS];
		int _uniformLocation[MAX_UNIFORMS];
		UniformValue _uniformVals[MAX_UNIFORMS];
	};
}
