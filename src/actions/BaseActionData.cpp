#include "BaseActionData.h"
#include <mutex>
#include "../config/Config.h"
#include "../utils/Validator.h"

#ifdef _DEBUG
std::mutex counterMutex;
#endif

namespace GetgudSDK {

	unsigned int GetEmptyActionSize() {
		int size = 0;
		size += 40 * sizeof(char);  // matchGuid size
		size += sizeof(long);       // actionTimeEpoch size
		return size;
	}

	extern Config sdkConfig;
	/**
	 * BaseActionData:
	 * @data: BaseData, structure contains all core metadata for each action
	 *
	 * Constructor of Base Action, all other actions inherit from BaseAction
	 **/
	BaseActionData::BaseActionData(BaseData data, bool _isEmpty) {
		m_actionType = data.ActionType;
		m_actionTimeEpoch = data.ActionTimeEpoch;
		m_playerGuid = data.PlayerGuid;
		m_matchGuid = data.MatchGuid;
		m_isEmpty = _isEmpty;
#ifdef _DEBUG
		if (m_isEmpty) {
			sdkConfig.emptyActionsAmount++;
			sdkConfig.totalCreatedEmptyActions++;
		}
		else {
			sdkConfig.actionsAmount++;
			sdkConfig.totalCreatedActions++;
		}
#endif
	};

	BaseActionData::BaseActionData(const BaseActionData& data) {
		m_actionType = data.m_actionType;
		m_actionTimeEpoch = data.m_actionTimeEpoch;
		m_playerGuid = data.m_playerGuid;
		m_matchGuid = data.m_matchGuid;
		m_isEmpty = data.m_isEmpty;
#ifdef _DEBUG
		if (m_isEmpty) {
			sdkConfig.emptyActionsAmount++;
			sdkConfig.totalCreatedEmptyActions++;
		}
		else {
			sdkConfig.actionsAmount++;
			sdkConfig.totalCreatedActions++;
		}
#endif
	}

	/**
	 * BaseActionData:
	 *
	 **/
	BaseActionData::~BaseActionData() {
#ifdef _DEBUG
		if (m_isEmpty) {
			sdkConfig.emptyActionsAmount--;
		}
		else {
			sdkConfig.actionsAmount--;
		}
#endif
	};

	/**
	 * IsValid:
	 *
	 * Check if action is valid, if action is not valid we will delete the
	 * game!
	 **/
	bool BaseActionData::IsValid() {
		bool isActionValid = Validator::ValidateStringLength(m_playerGuid, 1, 36);
		isActionValid &= Validator::ValidateStringChars(m_playerGuid);
		isActionValid &= Validator::ValidateStringLength(m_matchGuid, 1, 36);
		isActionValid &= Validator::ValidateStringChars(m_matchGuid);
		isActionValid &= Validator::ValidateItemValue(
			m_actionTimeEpoch, sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
			sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
		isActionValid &= Validator::ValidateActionType((unsigned int)m_actionType);

		// empty action is passed when game container or action buffer are full
		// for game deletion purposes
		isActionValid &= !m_isEmpty;

		return isActionValid;
	}

	/**
	 * ToString:
	 *
	 * For sending action stream to Getgud
	 **/
	void BaseActionData::ToString(std::ostringstream& oss) {

	}

	/**
	 * Clone:
	 *
	 **/
	BaseActionData* BaseActionData::Clone() {
		return new BaseActionData(*this);
	}

}  // namespace GetgudSDK