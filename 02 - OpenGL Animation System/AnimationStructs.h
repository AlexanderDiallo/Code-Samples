#pragma once

#include <vector>
#include <glm/hlm.hpp>

namespace Crown
{
	class Joint;
	class Animation;

	struct AnimationVertexAttributes
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
		glm::ivec4 bones;
		glm::vec4 weights;
	};

	struct Texture
	{
		unsigned int m_texture;
		int width = -1;
		int height = -1;
	};

	struct AnimationDrawObject
	{
		std::vector<AnimationVertexAttributes> vertexBuffer;
		std::vector<unsigned int> indexBuffer;

		unsigned int animationVAO = -1;

		unsigned int posVBO = -1;
		unsigned int normalVBO = -1;
		unsigned int texCoordVBO = -1;

		unsigned int indexEBO = -1;

		Texture texture;

		std::unique_ptr<Joint> rootJoint = nullptr;

		std::vector<glm::mat4> boneMatrices;
		std::vector<Animation> animations;
		size_t activeAnimationIndex = 0;
	};

	void LoadTexture(Texture& a_Texture, char const* a_FilePath)
	{
		glGenTextures(1, &a_Texture.m_texture);
		glBindTexture(GL_TEXTURE_2D, a_Texture.m_texture);
		//Wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		//Filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_set_flip_vertically_on_load(false);

		int ncrChannels = 0;

		//Texture 0 
		unsigned char* data = stbi_load(a_FilePath, &a_Texture.width, &a_Texture.height, &ncrChannels, 0);
		if (data)
		{
			switch (ncrChannels)
			{
			case 3:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_Texture.width, a_Texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				break;
			case 4:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_Texture.width, a_Texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
				break;
			default:
				assert(false);
				break;
			}
		}
		else
		{
			LOG("FAILED TO LOAD TEXTURE");
			assert(false);
		}
		stbi_image_free(data);
	}
}