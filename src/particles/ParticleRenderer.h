/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <memory>
#include <map>
#include <string>


namespace nhahn
{
	class ParticleSystem;

	class IParticleRenderer
	{
	public:
		IParticleRenderer() { }
		virtual ~IParticleRenderer() { }

		IParticleRenderer(const IParticleRenderer&) = delete;
		IParticleRenderer& operator = (const IParticleRenderer&) = delete;

		virtual void generate(ParticleSystem* sys, bool useQuads) = 0;
		virtual void destroy() = 0;
		virtual void update() = 0;
		virtual void render() = 0;
	};

	class ParticleRendererFactory
	{
		using MapOfRendererGenerators = std::map<std::string, std::shared_ptr<IParticleRenderer>(*)()>;

	public:
		static bool isAvailable(const char* name);
		static std::shared_ptr<IParticleRenderer> create(const char* name);

	protected:
		static void initGeneratorMap();
		static std::unique_ptr<MapOfRendererGenerators> s_generatorMap;

	private:
		ParticleRendererFactory();
	};
}