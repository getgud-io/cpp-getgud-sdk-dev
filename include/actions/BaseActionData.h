#pragma once

#if defined(__cplusplus) && defined(_WIN32)
#ifndef GETGUDSDK_EXPORTS
#define GETGUDSDK_API __declspec(dllexport)
#else
#define GETGUDSDK_API __declspec(dllimport)
#endif
#else
#define GETGUDSDK_API __attribute__((visibility("default")))
#endif

#include <map>
#include <string>

#include "Helpers.h"
#include <sstream>

namespace GetgudSDK {
	unsigned int GetEmptyActionSize();

	class BaseActionData {
	public:
		Actions m_actionType = Actions::None;
		long long m_actionTimeEpoch = 0;
		std::string m_playerGuid;
		std::string m_matchGuid;
		bool m_isEmpty = false;

	public:
		GETGUDSDK_API BaseActionData(BaseData data, bool _isEmpty = false);
		GETGUDSDK_API BaseActionData(const BaseActionData& data);
		GETGUDSDK_API BaseActionData() = delete;
		GETGUDSDK_API virtual ~BaseActionData();
		GETGUDSDK_API virtual bool IsValid();
		GETGUDSDK_API virtual void ToString(std::ostringstream& oss);
		GETGUDSDK_API virtual BaseActionData* Clone();
	};
}  // namespace GetgudSDK
