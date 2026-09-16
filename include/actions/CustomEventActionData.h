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
	class CustomEventActionData : public BaseActionData {
		std::string m_customEventGuid;
		int m_version;
		// Stored already base64 encoded so the payload can never carry the
		// action-stream delimiter, whatever the customer puts in it.
		std::string m_payloadBase64;

	public:
		// Empty playerGuid marks a match-level event and is sent as PvE.
		GETGUDSDK_API CustomEventActionData(std::string matchGuid,
			long long actionTimeEpoch,
			std::string playerGuid,
			std::string customEventGuid,
			int version,
			const std::string& payload);
		GETGUDSDK_API CustomEventActionData(const CustomEventActionData& data);
		GETGUDSDK_API CustomEventActionData() = delete;
		GETGUDSDK_API ~CustomEventActionData() override;
		GETGUDSDK_API bool IsValid() override;
		GETGUDSDK_API void ToString(std::ostringstream& oss) override;
		GETGUDSDK_API CustomEventActionData* Clone() override;
	};
}  // namespace GetgudSDK
