#pragma once
#include "BaseActionData.h"

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
	class HealActionData : public BaseActionData {
		float m_healthGained = 0;

	public:
		GETGUDSDK_API HealActionData(std::string matchGuid,
			long long actionTimeEpoch,
			std::string playerGuid,
			float healthGained);
		GETGUDSDK_API HealActionData(const HealActionData& data);
		GETGUDSDK_API HealActionData() = delete;
		GETGUDSDK_API ~HealActionData() override;
		GETGUDSDK_API bool IsValid() override;
		GETGUDSDK_API void ToString(std::ostringstream& oss) override;
		GETGUDSDK_API HealActionData* Clone() override;
	};
}  // namespace GetgudSDK
