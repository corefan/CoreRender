/*
Copyright (C) 2010, Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _CORERENDER_RENDER_MATERIAL_HPP_INCLUDED_
#define _CORERENDER_RENDER_MATERIAL_HPP_INCLUDED_

#include "ShaderText.hpp"
#include "Texture.hpp"
#include "UniformData.hpp"

namespace cr
{
namespace render
{
	class Material : public res::Resource
	{
		public:
			Material(render::VideoDriver *driver,
			         render::Renderer *renderer,
			         res::ResourceManager *rmgr,
			         const std::string &name);
			virtual ~Material();

			void setShader(ShaderText::Ptr shader);
			ShaderText::Ptr getShader();

			void setShaderFlags(const std::string &flags)
			{
				shaderflags = flags;
			}
			std::string getShaderFlags()
			{
				return shaderflags;
			}

			struct TextureInfo
			{
				Texture::Ptr texture;
				std::string name;
			};

			void addTexture(const std::string name, Texture::Ptr texture);
			Texture::Ptr getTexture(const std::string &name);
			const std::vector<TextureInfo> &getTextures();

			UniformData &getUniformData()
			{
				return uniforms;
			}

			virtual bool load();

			virtual const char *getType()
			{
				return "Material";
			}

			typedef core::SharedPointer<Material> Ptr;
		private:
			render::VideoDriver *driver;
			render::Renderer *renderer;

			ShaderText::Ptr shader;
			std::string shaderflags;

			std::vector<TextureInfo> textures;

			UniformData uniforms;
	};
}
}

#endif
