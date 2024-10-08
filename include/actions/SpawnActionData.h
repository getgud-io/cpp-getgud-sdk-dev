#pragma once
#include "BaseActionData.h"

#include <vector>

#if defined(__cplusplus) && defined(_WIN32)
#ifndef GETGUDSDK_EXPORTS
#define GETGUDSDK_API __declspec(dllexport)
#else
#define GETGUDSDK_API __declspec(dllimport)
#endif
#else
#define GETGUDSDK_API __attribute__((visibility("default")))
#endif


namespace GetgudSDK {
	class SpawnActionData : public BaseActionData {
		PositionF m_position;
		RotationF m_rotation;
		float m_initialHealth = 0;
		std::string m_teamGuid;
		std::string m_characterGuid;

	public:
		GETGUDSDK_API SpawnActionData(std::string matchGuid,
			long long actionTimeEpoch,
			std::string playerGuid,
			std::string characterGuid,
			std::string teamGuid,
			float initialHealth,
			PositionF position,
			RotationF rotation);
		GETGUDSDK_API SpawnActionData(const SpawnActionData& data);
		GETGUDSDK_API SpawnActionData() = delete;
		GETGUDSDK_API ~SpawnActionData() override;
		GETGUDSDK_API bool IsValid() override;
		GETGUDSDK_API void ToString(std::ostringstream& oss) override;
		GETGUDSDK_API SpawnActionData* Clone() override;
	};
}  // namespace GetgudSDK
