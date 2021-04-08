#pragma once

#include <vector>
#include <memory>

#include "GLTFLoading/GLTFParser.h"

namespace Crown
{
	struct AnimationData;
	struct SamplerData;
	class Joint;
	
	//Animation class stores the key frames (with relevant joint transform data) of an animation for a specific AnimationDrawObject
	class Animation
	{
	public:
		Animation(AnimationData& a_AnimationData);
		~Animation();

		//Update Joint transformations based on Animation data
		void Update(Joint& a_RootJoint, float a_DeltaTime);

		std::string GetName() const { return m_Name; }
		
	private:
		//Helper function used to get the key frame indices that come before and after m_TimeElapsed
		//as well as a progression (percentage) used to interpolate between the two key frames
		void GetSurroundingKeyFrameIndices(const SamplerData& a_SamplerData, size_t& a_BeforeKeyIndex, size_t& a_AfterKeyIndex, float& a_Progression);
		
	private:
		float m_TimeElapsed = 0.0f;
		float m_AnimationDuration = 0.0f;
		
		//m_ChannelData and m_SamplerData contain the actual animation data loaded from GLTF
		std::vector<std::shared_ptr<ChannelData>> m_ChannelData;
		std::vector<std::shared_ptr<SamplerData>> m_SamplerData;

		std::string m_Name;
	};
}
