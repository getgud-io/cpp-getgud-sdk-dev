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
	class DamageActionData : public BaseActionData {

	public:
		std::string m_weaponGuid;
		std::string m_victimPlayerGuid;
		float m_damageDone = 0;

	public:
		GETGUDSDK_API DamageActionData(std::string matchGuid,
			long long actionTimeEpoch,
			std::string playerGuid,
			std::string victimPlayerGuid,
			float damageDone,
			std::string weaponGuid);
		GETGUDSDK_API DamageActionData(const DamageActionData& data);
		GETGUDSDK_API DamageActionData() = delete;
		GETGUDSDK_API ~DamageActionData() override;
		GETGUDSDK_API bool IsValid() override;
		GETGUDSDK_API void ToString(std::ostringstream& oss) override;
		GETGUDSDK_API DamageActionData* Clone() override;
	};
}  // namespace GetgudSDK
