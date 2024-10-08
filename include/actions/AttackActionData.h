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
	class AttackActionData : public BaseActionData {

	public:
		std::string m_weaponGuid;

	public:
		GETGUDSDK_API AttackActionData(std::string matchGuid,
			long long actionTimeEpoch,
			std::string playerGuid,
			std::string weaponGuid);
		GETGUDSDK_API AttackActionData(const AttackActionData& data);
		GETGUDSDK_API AttackActionData() = delete;
		GETGUDSDK_API ~AttackActionData() override;
		GETGUDSDK_API bool IsValid() override;
		GETGUDSDK_API void ToString(std::ostringstream& oss) override;
		GETGUDSDK_API AttackActionData* Clone() override;
	};
}  // namespace GetgudSDK
