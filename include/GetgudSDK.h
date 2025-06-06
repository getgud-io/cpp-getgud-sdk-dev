#pragma once

#if defined(__cplusplus) && defined(_WIN32)
#ifndef GETGUDSDK_EXPORTS
#define GETGUDSDK_API __declspec(dllexport)
#else
#define GETGUDSDK_API __declspec(dllimport)
#endif
#else // not win32
#define GETGUDSDK_API __attribute__((visibility("default")))
#endif

#include "actions/AttackActionData.h"
#include "actions/DamageActionData.h"
#include "actions/DeathActionData.h"
#include "actions/HealActionData.h"
#include "actions/Helpers.h"
#include "actions/PositionActionData.h"
#include "actions/SpawnActionData.h"
#include "actions/AffectActionData.h"

#include <deque>

namespace GetgudSDK {

	GETGUDSDK_API bool Init();
	GETGUDSDK_API bool Init(std::string configFileFullPath);
	/**
	 * Init:
	 *
	 * Init Getgud SDK
	 * *passAsContent: true - read the configFile as a string
	 * *passAsContent: false - read the configFile as a file path
	 **/
	GETGUDSDK_API bool Init(std::string configFile, bool passAsContent);
	GETGUDSDK_API std::string StartGame(int titleId,
		std::string privateKey,
		std::string serverGuid,
		std::string gameMode,
		std::string serverLocation);
	GETGUDSDK_API std::string StartGame(std::string serverGuid, std::string gameMode, std::string serverLocation);
	GETGUDSDK_API std::string StartMatch(std::string gameGuid, std::string matchMode, std::string mapName);
	GETGUDSDK_API std::string StartMatch(std::string gameGuid, std::string matchMode, std::string mapName,std::string customField);
	GETGUDSDK_API bool MarkEndGame(std::string gameGuid);
	GETGUDSDK_API bool SetMatchWinTeam(std::string matchGuid, std::string teamGuid);
	GETGUDSDK_API bool SendInMatchReport(ReportInfo reportInfo);
	GETGUDSDK_API bool SendChatMessage(std::string matchGuid, ChatMessageInfo messageInfo);
	GETGUDSDK_API bool SendActions(const std::deque<BaseActionData*>& actions);
	GETGUDSDK_API bool SendAction(BaseActionData* action);
	GETGUDSDK_API bool SendAffectAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& affectGuid,
		const std::string& playerGuid,
		AffectState affectState);
	GETGUDSDK_API bool SendAttackAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const std::string& weaponGuid);
	GETGUDSDK_API bool SendDamageAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const std::string& victimPlayerGuid,
		float damageDone,
		const std::string& weaponGuid);
	GETGUDSDK_API bool SendHealAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		float healthGained);
	GETGUDSDK_API bool SendSpawnAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const std::string& characterGuid,
		const std::string& teamGuid,
		float initialHealth,
		const PositionF& position,
		const RotationF& rotation);
	GETGUDSDK_API bool SendDeathAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const std::string& attackerGuid);
	GETGUDSDK_API bool SendPositionAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const PositionF& position,
		const RotationF& rotation);

	GETGUDSDK_API bool SendReports(int titleId,
		std::string privateKey,
		std::deque<ReportInfo>& reports);
	GETGUDSDK_API bool SendReports(std::deque<ReportInfo>& reports);
	GETGUDSDK_API bool UpdatePlayers(int titleId,
		std::string privateKey,
		std::deque<PlayerInfo>& players);
	GETGUDSDK_API bool UpdatePlayers(std::deque<PlayerInfo>& players);
	GETGUDSDK_API void Dispose();

}  // namespace GetgudSDK
