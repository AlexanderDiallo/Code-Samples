#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <unique_ptr>

#include "AnimationStructs.h"

namespace Crown
{
	const int maxBones = 128;
	const int maxBonesPerVertex = 4;

	class Shader;

	class AnimatedModel
	{
	public:
		AnimatedModel();
		~AnimatedModel();

		void Update(float a_DeltaTime = 1.f / 120.f);
		void Draw(glm::mat4 a_MVP, glm::mat4 a_Model);

		void Load(std::string a_ModelRootDirectory, std::string a_ModelFileName);

		std::vector<std::string> GetAllAnimationNames(size_t a_PrimitiveID);
		bool SetAnimationByName(size_t a_PrimitiveID, const std::string a_AnimationName);

		Joint* GetJointByName(size_t a_PrimitiveID, const std::string a_JointName);

	private:
		void LoadGLTFData(std::string a_ModelRootDirectory, std::string a_ModelFileName);
		void LoadGPUVertexAttributeData();

	private:

		std::vector<AnimationDrawObject> m_DrawObjects;

		int m_MVPLoc = -1;
		int m_BonesLoc = -1;

		std::unique_ptr<Shader> m_Shaders = nullptr;
	};
}
