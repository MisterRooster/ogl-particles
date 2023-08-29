/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "Shader.h"

#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>
#include "utility/Debug.h"
#include "utility/FileSystem.h"
#include "utility/Utils.h"


namespace nhahn
{
	void ShaderObject::addFile(const char* path)
	{
		loadFile(&_sources[_sourceCount++], path);
	}

	int ShaderObject::refresh()
	{
		int compileFlag = 0;

		for (int i = 0; i < _sourceCount; i++)
		{
			if (FileSystem::fileLastChanged(_sources[i].file) > _sources[i].timestamp)
			{
				loadFile(&_sources[i], _sources[i].file);
				compileFlag = 1;
			}
		}

		if (compileFlag)
			compile(_type);

		return compileFlag;
	}

	void ShaderObject::compile(ShaderType type_)
	{
		const GLchar* source[MAX_SOURCES];
		for (int i = 0; i < _sourceCount; i++)
			source[i] = _sources[i].src;

		GLuint shader = glCreateShader(type_);
		glShaderSource(shader, _sourceCount, source, NULL);
		glCompileShader(shader);

		_name = shader;
		_type = type_;

		check();
	}

	void ShaderObject::loadFile(ShaderSource* s, const char* path) 
	{
		s->file = path;
		s->src = FileSystem::readBinaryFile(path);
		s->timestamp = FileSystem::fileLastChanged(path);
	}

	void ShaderObject::check()
	{
		GLuint obj = _name;
		GLchar tmp[2048];

		int status, length;
		glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
		glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);

		GLchar* log = NULL;
		if (length > 1) {
			log = (GLchar*)malloc(length * sizeof(GLchar));
			glGetShaderInfoLog(obj, length, NULL, log);
		}

		if (!status || length > 1) {
			int src_length;
			glGetShaderiv(obj, GL_SHADER_SOURCE_LENGTH, &src_length);

			GLchar* src = (GLchar*)malloc(src_length * sizeof(GLchar));
			glGetShaderSource(obj, src_length, NULL, src);

			DBG("shader", DebugLevel::WARNING, "---------------------------\n");
			int line = 1;
			GLchar* src2 = src;
			for (int i = 0; i < src_length; i++, src2++) {
				if (*src2 == '\n') {
					memcpy(tmp, src, src2 - src);
					tmp[src2 - src] = '\0';

					DBG("shader", DebugLevel::WARNING, "%4d | %s\n", line, tmp);
					src = src2 + 1;
					line++;
				}
			}
			DBG("shader", DebugLevel::WARNING, "%4d | %s\n", line, src);
			DBG("shader", DebugLevel::WARNING, "---------------------------\n");
			DBG("shader", DebugLevel::WARNING, "%s\n", log);
			FAIL("Unable to compile shader\n");
		}
		else if (length > 1) {
			DBG("shader", DebugLevel::WARNING, "%s\n", log);
		}
	}

	//----------------------------------------------------------------------------------------------

	Shader::~Shader()
	{
		glDeleteProgram(_shaderID);
	}

	Shader::Shader(const char* path, const char* includeFile, const char* v, const char* g,
		const char* f, int outputs) : _shaderID(-1), _shaderCount(0), _outputCount(0),
		_varyingCount(0), _feedbackMode(FEEDBACK_INTERLEAVED), _uniformCount(0)
	{
		char fullPreamble[1024], fullV[1024], fullG[1024], fullF[1024];

		sprintf(fullPreamble, "%s%s", path, includeFile);
		sprintf(fullV, "%s%s", path, v);
		sprintf(fullG, "%s%s", path, g);
		sprintf(fullF, "%s%s", path, f);

		ShaderObject* vert, * frag, * geom;
		if (f)
		{
			frag = addObject();
			if (includeFile)
				frag->addFile(fullPreamble);
			frag->addFile(fullF);
			frag->compile(FRAGMENT_SHADER);
		}

		vert = addObject();
		if (includeFile)
			vert->addFile(fullPreamble);
		vert->addFile(fullV);
		vert->compile(VERTEX_SHADER);

		if (g)
		{
			geom = addObject();
			if (includeFile)
				geom->addFile(fullPreamble);
			geom->addFile(fullG);
			geom->compile(GEOMETRY_SHADER);
		}

		char out[] = "FragColor0";
		for (int i = 0; i < outputs; i++, out[9]++) /* not more than 10 outputs */
			addOutput(out);

		link();
	}

	Shader::Shader(const char* path, const char* includeFile, const char* c) :
		_shaderID(-1), _shaderCount(0), _outputCount(0),
		_varyingCount(0), _feedbackMode(FEEDBACK_INTERLEAVED), _uniformCount(0)
	{
		char fullPreamble[1024], fullC[1024];

		sprintf(fullPreamble, "%s%s", path, includeFile);
		sprintf(fullC, "%s%s", path, c);

		ShaderObject* compute = addObject();
		if (includeFile)
			compute->addFile(fullPreamble);
		compute->addFile(fullC);
		compute->compile(COMPUTE_SHADER);

		link();
	}

	ShaderObject* Shader::addObject()
	{
		return &_shaders[_shaderCount++];
	}

	void Shader::addOutput(const char* name)
	{
		_outputs[_outputCount++] = name;
	}

	void Shader::addFeedbackVarying(const char* name)
	{
		_varyings[_varyingCount++] = name;
	}

	void Shader::setFeedbackMode(FeedbackMode f)
	{
		_feedbackMode = f;
	}

	int Shader::refresh()
	{
		int linkFlag = 0;

		for (int i = 0; i < _shaderCount; i++)
			if (_shaders[i].refresh())
				linkFlag = 1;

		if (linkFlag)
			link();

		return linkFlag;
	}

	void Shader::link()
	{
		_shaderID = glCreateProgram();

		for (int i = 0; i < _shaderCount; i++)
			glAttachShader(_shaderID, _shaders[i].name());

		for (int i = 0; i < _outputCount; i++)
			glBindFragDataLocation(_shaderID, i, _outputs[i]);

		if (_varyingCount)
		{
			GLenum mode = (_feedbackMode == FEEDBACK_INTERLEAVED ? GL_INTERLEAVED_ATTRIBS : GL_SEPARATE_ATTRIBS);
			glTransformFeedbackVaryings(_shaderID, _varyingCount, _varyings, mode);
		}

		glLinkProgram(_shaderID);
		check();
	}

	void Shader::bind() const
	{
		glUseProgram(_shaderID);
	}

	void Shader::unbind() const
	{
		glUseProgram(0);
	}

	void Shader::check() {
		GLuint obj = _shaderID;

		int status, length;
		glGetProgramiv(obj, GL_LINK_STATUS, &status);
		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);

		glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
		GLchar* log;
		if (length > 1)
		{
			log = (GLchar*)malloc(length * sizeof(GLchar));
			glGetProgramInfoLog(obj, length, NULL, log);

			DBG("shader", DebugLevel::WARNING, "%s\n", log);
		}
	}

	void Shader::dispatch(int sizeX, int sizeY, int sizeZ) const
	{
		glDispatchCompute(sizeX, sizeY, sizeZ);
	}

	int Shader::uniformIndex(const char* name)
	{
		int hash = Utils::stringHash(name);

		for (int i = 0; i < _uniformCount; i++)
		{
			if (_uniformHash[i] == hash)
				return i;
		}

		_uniformHash[_uniformCount] = hash;
		_uniformLocation[_uniformCount] = glGetUniformLocation(_shaderID, name);
		_uniformCount++;

		return _uniformCount - 1;
	}

	GLint Shader::uniform(const char* name)
	{
		return _uniformLocation[uniformIndex(name)];
	}

	/* TODO: Kill it with fire and rebuild with a generic version */

	void Shader::setUniformI(const char* name, int i) 
	{
		int t = uniformIndex(name);
		if (_uniformVals[t].i != i)
		{
			glUniform1i(_uniformLocation[t], i);
			_uniformVals[t].i = i;
		}
	}

	void Shader::setUniformI(const char* name, int i1, int i2)
	{
		int t = uniformIndex(name);
		if (_uniformVals[t].i2[0] != i1 || _uniformVals[t].i2[1] != i2)
		{
			glUniform2i(_uniformLocation[t], i1, i2);
			_uniformVals[t].i2[0] = i1;
			_uniformVals[t].i2[1] = i2;
		}
	}

	void Shader::setUniformI(const char* name, int i1, int i2, int i3)
	{
		int t = uniformIndex(name);
		int* is = _uniformVals[t].i3;
		if (is[0] != i1 || is[1] != i2 || is[2] != i3)
		{
			glUniform3i(_uniformLocation[t], i1, i2, i3);
			is[0] = i1;
			is[1] = i2;
			is[2] = i3;
		}
	}

	void Shader::setUniformI(const char* name, int i1, int i2, int i3, int i4)
	{
		int t = uniformIndex(name);
		int* is = _uniformVals[t].i4;
		if (is[0] != i1 || is[1] != i2 || is[2] != i3 || is[3] != i4) 
		{
			glUniform4i(_uniformLocation[t], i1, i2, i3, i4);
			is[0] = i1;
			is[1] = i2;
			is[2] = i3;
			is[3] = i4;
		}
	}

	void Shader::setUniformF(const char* name, float f)
	{
		int t = uniformIndex(name);
		if (_uniformVals[t].f != f)
		{
			glUniform1f(_uniformLocation[t], f);
			_uniformVals[t].f = f;
		}
	}

	void Shader::setUniformF(const char* name, float f1, float f2)
	{
		int t = uniformIndex(name);
		if (_uniformVals[t].f2[0] != f1 || _uniformVals[t].f2[1] != f2)
		{
			glUniform2f(_uniformLocation[t], f1, f2);
			_uniformVals[t].f2[0] = f1;
			_uniformVals[t].f2[1] = f2;
		}
	}

	void Shader::setUniformF(const char* name, float f1, float f2, float f3)
	{
		int t = uniformIndex(name);
		float* fs = _uniformVals[t].f3;
		if (fs[0] != f1 || fs[1] != f2 || fs[2] != f3)
		{
			glUniform3f(_uniformLocation[t], f1, f2, f3);
			fs[0] = f1;
			fs[1] = f2;
			fs[2] = f3;
		}
	}

	void Shader::setUniformF(const char* name, float f1, float f2, float f3, float f4)
	{
		int t = uniformIndex(name);
		float* fs = _uniformVals[t].f4;
		if (fs[0] != f1 || fs[1] != f2 || fs[2] != f3 || fs[3] != f4)
		{
			glUniform4f(_uniformLocation[t], f1, f2, f3, f4);
			fs[0] = f1;
			fs[1] = f2;
			fs[2] = f3;
			fs[3] = f4;
		}
	}

	void Shader::setUniformF(const char* name, const glm::vec2& v)
	{
		int t = uniformIndex(name);
		if (_uniformVals[t].f2[0] != v.x || _uniformVals[t].f2[1] != v.y)
		{
			glUniform2f(_uniformLocation[t], v.x, v.y);
			_uniformVals[t].f2[0] = v.x;
			_uniformVals[t].f2[1] = v.y;
		}
	}

	void Shader::setUniformF(const char* name, const glm::vec3& v)
	{
		int t = uniformIndex(name);
		float* fs = _uniformVals[t].f3;
		if (fs[0] != v.x || fs[1] != v.y || fs[2] != v.z)
		{
			glUniform3f(_uniformLocation[t], v.x, v.y, v.z);
			fs[0] = v.x;
			fs[1] = v.y;
			fs[2] = v.z;
		}
	}

	void Shader::setUniformF(const char* name, const glm::vec4& v)
	{
		int t = uniformIndex(name);
		float* fs = _uniformVals[t].f4;
		if (fs[0] != v.x || fs[1] != v.y || fs[2] != v.z || fs[3] != v.w)
		{
			glUniform4f(_uniformLocation[t], v.x, v.y, v.z, v.w);
			fs[0] = v.x;
			fs[1] = v.y;
			fs[2] = v.z;
			fs[3] = v.w;
		}
	}

	void Shader::setUniformMat(const char* name, const glm::mat4& m, bool transpose)
	{
		glUniformMatrix4fv(_uniformLocation[uniformIndex(name)], 1, transpose, glm::value_ptr(m));
	}
}

