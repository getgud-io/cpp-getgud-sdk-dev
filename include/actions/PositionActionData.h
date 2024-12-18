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
	unsigned int GetPositionActionSize();
	class PositionActionData : public BaseActionData {
	private:
		unsigned int m_size = 0;

	public: 
		bool m_isPlayerFirstPosition = false;
		Orientation m_orientation;

	public:
		GETGUDSDK_API PositionActionData(std::string matchGuid,
			long long actionTimeEpoch,
			std::string playerGuid,
			PositionF position,
			RotationF rotation);
		GETGUDSDK_API PositionActionData(const PositionActionData& data);
		GETGUDSDK_API PositionActionData() = delete;
		GETGUDSDK_API ~PositionActionData() override;
		GETGUDSDK_API bool IsValid() override;
		GETGUDSDK_API void ToString(std::ostringstream& oss) override;
		GETGUDSDK_API PositionActionData* Clone() override;
		GETGUDSDK_API Orientation& getOrientation();
	};
}  // namespace GetgudSDK
