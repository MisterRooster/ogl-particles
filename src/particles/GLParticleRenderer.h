/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include "ParticleRenderer.h"


namespace nhahn
{
	class GLParticleRenderer : public IParticleRenderer
	{
	public:
		GLParticleRenderer() { }
		~GLParticleRenderer() { destroy(); }

		void generate(ParticleSystem* sys, bool useQuads) override;
		void destroy() override;
		void update() override;
		void render() override;

	protected:
		ParticleSystem* m_system{ nullptr };

		unsigned int m_bufPos{ 0 };
		unsigned int m_bufCol{ 0 };
		unsigned int m_vao{ 0 };
	};

	class GLParticleRendererUseMap : public GLParticleRenderer
	{
	public:
		GLParticleRendererUseMap() { }
		~GLParticleRendererUseMap() { destroy(); }

		void update() override;
	};

	class GLParticleRendererDoubleVao : public GLParticleRenderer
	{
	public:
		GLParticleRendererDoubleVao() { }
		~GLParticleRendererDoubleVao() { destroy(); }

		virtual void generate(ParticleSystem* sys, bool useQuads) override;
		virtual void destroy() override;
		virtual void update() override;
		virtual void render() override;

	protected:
		unsigned int m_doubleBufPos[2];
		unsigned int m_doubleBufCol[2];
		unsigned int m_doubleVao[2];
		unsigned int m_id;
	};

	struct Vertex
	{
		float x, y, z;
		unsigned char r, g, b, a;
	};

	class GLParticleRendererPersistent : public GLParticleRenderer
	{
	public:
		GLParticleRendererPersistent() { }
		~GLParticleRendererPersistent() { destroy(); }

		virtual void generate(ParticleSystem* sys, bool useQuads) override;
		virtual void update() override;
		virtual void render() override;

	protected:
		unsigned int m_id;
		Vertex* m_ptr = nullptr;
	};
}