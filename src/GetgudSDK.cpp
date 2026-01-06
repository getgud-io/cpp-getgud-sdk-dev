#include "../include/GetgudSDK.h"
#include <exception>
#include <string>
#include "config/Config.h"
#include "game_container/GameContainer.h"
#include "senders/game_sender/GameSender.h"
#include "logger/Logger.h"
#include "senders/player_updater/PlayerUpdater.h"
#include "senders/report_sender/ReportSender.h"
#include "utils/Utils.h"

namespace GetgudSDK {
	extern SharedGameSenders sharedGameSenders;
	extern SharedPlayerUpdaters sharedPlayerUpdaters;
	extern SharedReportSenders sharedReportSenders;
	extern Config sdkConfig;
	extern Logger logger;
	extern ActionsBuffer actionsBuffer;
	extern GameContainer gameContainer;

	/**
	 * Init:
	 *
	 * Init Getgud SDK
	 **/
	bool Init() {
		return Init("", false);
	}

	/**
	* Init:
	*
	* Init Getgud SDK
	* *configFileFullPath: The config file full path
	**/
	bool Init(std::string configFileFullPath) {
		return Init(configFileFullPath, false);
	}

	/**
	* Init:
	*
	* Init Getgud SDK
	* *passAsContent: true - read the configFile as a string
	* *passAsContent: false - read the configFile as a file path to the config
	**/
	bool Init(std::string configFile, bool passAsContent) {
		bool init_result = false;
		try {
			sdkConfig.logToFile = true;
			curl_global_init(CURL_GLOBAL_DEFAULT);
			if (sdkConfig.LoadSettings(configFile, passAsContent))
			{
				logger.Log(LogType::DEBUG, "Loaded config with the following parameters: \n" + sdkConfig.ToString());
				init_result = true;
			}
			else
			{
				logger.Log(LogType::_ERROR, "Config can not be loaded, SDK will not work properly.");
				sdkConfig.logToFile = false;
			}
		}
		catch (std::exception& _error) {
			logger.Log(LogType::FATAL, std::string("GetgudSDK::Init->Couldn't initialize Getgud SDK: ") + std::string(_error.what()));
		}

		return init_result;
	}

	/**
	 * StartGame:
	 * Start new game
	 **/
	std::string StartGame(int titleId,
		std::string privateKey,
		std::string serverGuid,
		std::string gameMode,
		std::string serverLocation) {
		std::string gameGuid;

		try {

			logger.Log(LogType::DEBUG, std::string("String a new Game with the following parameters: Title ID: " + std::to_string(titleId) + " | serverGuid: " + serverGuid + " | gameMode: " + gameMode + " | serverLocation: " + serverLocation));

			if (sharedGameSenders.gameSenders.empty()) {

				GetgudSDK::GameSender* gameSender = nullptr;

				{
					// lock_guard scope
					// Create first Game Sender, if hypermode is on there will be more than  
					std::lock_guard<std::mutex> locker(sharedGameSenders.gameSendersMutex);
					gameSender = new GameSender();
				}

				// Start all services
				gameSender->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
			}

			gameGuid = gameContainer.AddGame(titleId, privateKey, serverGuid, gameMode, serverLocation);

		}
		catch (std::exception& _error) {
			logger.Log(LogType::FATAL, std::string("GetgudSDK::StartGame->Couldn't start new game: ") + std::string(_error.what()));
		}

		return gameGuid;
	}

	/**
	 * StartGame:
	 *
	 * Start new game
	 **/
	std::string StartGame(std::string serverGuid, std::string gameMode, std::string serverLocation) {
		std::string gameGuid;

		try {

			logger.Log(LogType::DEBUG, std::string("String a new Game with the following parameters: serverGuid: " + serverGuid + " | gameMode: " + gameMode + " | serverLocation: " + serverLocation));

			if (sharedGameSenders.gameSenders.empty()) {
				GetgudSDK::GameSender* gameSender = nullptr;
				{  // lock_guard scope
				  // Create first Game Sender, if hypermode is on there will be more than
				  // 1
					std::lock_guard<std::mutex> locker(sharedGameSenders.gameSendersMutex);
					gameSender = new GameSender();
				}
				// Start all services
				gameSender->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
			}

			const char* titleIdHolder = std::getenv("GETGUD_TITLE_ID");
			const char* privateKeyHolder = std::getenv("GETGUD_PRIVATE_KEY");

			if (titleIdHolder == nullptr || privateKeyHolder == nullptr) {
				logger.Log(LogType::FATAL,
					std::string(
						"GetgudSDK::StartGame->Environment variables GETGUD_TITLE_ID and "
						"GETGUD_PRIVATE_KEY are required"));
			}

			std::string titleId(titleIdHolder);
			std::string privateKey(privateKeyHolder);

			gameGuid = gameContainer.AddGame(std::stoi(titleId.c_str()), privateKey,
				serverGuid, gameMode, serverLocation);
		}
		catch (std::exception& _error) {
			logger.Log(LogType::FATAL,
				std::string("GetgudSDK::StartGame->Couldn't start new game") +
				std::string(_error.what()));
		}

		return gameGuid;
	}

	/**
	 * Start match:
	 *
	 * Start a new match for an existing game
	 **/
	std::string StartMatch(std::string gameGuid,
		std::string matchMode,
		std::string mapName) {

		return StartMatch(gameGuid, matchMode, mapName, "");
	}

	/**
	 * Start match:
	 *
	 * Start a new match for an existing game
	 **/
	std::string StartMatch(std::string gameGuid,
		std::string matchMode,
		std::string mapName,
		std::string customField) {

		std::string matchGuid;

		try {

			logger.Log(LogType::DEBUG, std::string("String a new Match with the following parameters: gameGuid: " + gameGuid + " | matchMode: " + matchMode + " | mapName: " + mapName + " | customField: " + customField));

			matchGuid = gameContainer.AddMatch(gameGuid, matchMode, mapName, customField);

		}
		catch (std::exception& _error) {

			logger.Log(LogType::FATAL, std::string("GetgudSDK::StartMatch->Couldn't start new match: ") + std::string(_error.what()));
		}
		return matchGuid;
	}

	/**
	 * MarkEndGame:
	 *
	 * Mark started game as finished
	 * @param blocking - If true, waits until all queued actions are sent before returning
	 **/
	bool MarkEndGame(std::string gameGuid, bool blocking) {

		bool gameEnded = false;

		try {

			logger.Log(LogType::DEBUG, std::string("Marking End Game for the following Game Guid: gameGuid: " + gameGuid + " blocking: " + (blocking ? "true" : "false")));

			gameEnded = gameContainer.MarkEndGame(gameGuid, blocking);

		}
		catch (std::exception& _error) {

			logger.Log(LogType::FATAL, std::string("GetgudSDK::MarkEndGame->: ") + std::string(_error.what()));
		}
		return gameEnded;
	}

	bool SetMatchWinTeam(std::string matchGuid, std::string teamGuid) {

		bool retValue = false;

		try {

			logger.Log(LogType::DEBUG, std::string("Marking Match Win Team for the following match Guid: " + matchGuid));

			retValue = gameContainer.SetMatchWinTeam(matchGuid, teamGuid);

		}
		catch (std::exception& _error) {

			logger.Log(LogType::FATAL, std::string("GetgudSDK::SetMatchWinTeam->: ") + std::string(_error.what()));
		}
		return retValue;
	}


	/**
	 * SendInMatchReport:
	 *
	 * Send a report which belongs to specifc match which is now live
	 **/
	bool SendInMatchReport(ReportInfo reportInfo) {
		bool inMatchReportAdded = false;
		try {
			inMatchReportAdded = gameContainer.AddInMatchReport(reportInfo);
		}
		catch (std::exception& _error) {
			logger.Log(
				LogType::FATAL,
				std::string(
					"GetgudSDK::SendInMatchReport->Couldn't add in match report: ") +
				std::string(_error.what()));
		}

		return inMatchReportAdded;
	}

	/**
	 * SendChatMessage:
	 *
	 *  Send a message which belongs to specifc match which is now live
	 **/
	bool SendChatMessage(std::string matchGuid, ChatMessageInfo messageInfo) {
		bool chatMessageSent = false;
		try {
			chatMessageSent = gameContainer.AddChatMessage(matchGuid, messageInfo);
		}
		catch (std::exception& _error) {
			logger.Log(
				LogType::FATAL,
				std::string(
					"GetgudSDK::SendChatMessage->Couldn't send match chat message: ") +
				std::string(_error.what()));
		}

		return chatMessageSent;
	}

	/**
	 * SendActions:
	 *
	 * Add actions to an action buffer
	 **/
	bool SendActions(const std::deque<BaseActionData*>& actions) {
		bool actionsSent = false;
		int actionsSize = 0;
		try {
			actionsSize = actions.size();
			actionsSent = actionsBuffer.AddActions(actions);
		}
		catch (std::exception& _error) {
			logger.Log(LogType::FATAL, std::string("GetgudSDK::SendActions->Couldn't "
				"send actions to Action Buffer: ") +
				std::string(_error.what()));
		}

		return actionsSent;
	}

	/**
	 * SendAction:
	 *
	 * Send a single action to an action buffer
	 **/
	bool SendAction(BaseActionData* action) {
		bool sendResult = false;
		std::deque<BaseActionData*> actions;
		try {
			actions = { action };
			sendResult = SendActions(actions);
		}
		catch (std::exception& _error)
		{
			logger.Log(LogType::FATAL, std::string("GetgudSDK::SendAction->Couldn't "
				"send actions to Action Buffer: ") +
				std::string(_error.what()));
		}
		return sendResult;
	}

	/**
	 * SendAffectAction:
	 *
	 **/
	bool SendAffectAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const std::string& affectGuid,
		AffectState affectState) {
		bool sendResult = false;
		try {
			AffectActionData* affectAction =
				new AffectActionData(matchGuid, actionTimeEpoch, playerGuid, affectGuid, affectState);
			std::deque<BaseActionData*> actions = { affectAction };
			sendResult = SendActions(actions);
			delete affectAction;
		}
		catch (std::exception& _error)
		{
			logger.Log(LogType::FATAL, std::string("GetgudSDK::SendAffectAction->Couldn't "
				"send actions to Action Buffer: ") +
				std::string(_error.what()));
		}
		return sendResult;
	}

	/**
	 * SendAttackAction:
	 *
	 **/
	bool SendAttackAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const std::string& weaponGuid) {
		bool sendResult = false;
		try {
			AttackActionData* attackAction =
				new AttackActionData(matchGuid, actionTimeEpoch, playerGuid, weaponGuid);
			std::deque<BaseActionData*> actions = { attackAction };
			sendResult = SendActions(actions);
			delete attackAction;
		}
		catch (std::exception& _error)
		{
			logger.Log(LogType::FATAL, std::string("GetgudSDK::SendAttackAction->Couldn't "
				"send actions to Action Buffer: ") +
				std::string(_error.what()));
		}
		return sendResult;
	}

	/**
	 * SendDamageAction:
	 *
	 **/
	bool SendDamageAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const std::string& victimPlayerGuid,
		float damageDone,
		const std::string& weaponGuid) {
		bool sendResult = false;
		try {
			DamageActionData* damageAction =
				new DamageActionData(matchGuid, actionTimeEpoch, playerGuid,
					victimPlayerGuid, damageDone, weaponGuid);
			std::deque<BaseActionData*> actions = { damageAction };
			sendResult = SendActions(actions);
			delete damageAction;
		}
		catch (std::exception& _error)
		{
			logger.Log(LogType::FATAL, std::string("GetgudSDK::SendDamageAction->Couldn't "
				"send actions to Action Buffer: ") +
				std::string(_error.what()));
		}
		return sendResult;
	}

	/**
	 * SendHealAction:
	 *
	 **/
	bool SendHealAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		float healthGained) {
		bool sendResult = false;
		try {
			HealActionData* healAction =
				new HealActionData(matchGuid, actionTimeEpoch, playerGuid, healthGained);
			std::deque<BaseActionData*> actions = { healAction };
			sendResult = SendActions(actions);
			delete healAction;
		}
		catch (std::exception& _error)
		{
			logger.Log(LogType::FATAL, std::string("GetgudSDK::SendHealAction->Couldn't "
				"send actions to Action Buffer: ") +
				std::string(_error.what()));
		}
		return sendResult;
	}

	/**
	 * SendSpawnAction:
	 *
	 **/
	bool SendSpawnAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const std::string& characterGuid,
		const std::string& teamGuid,
		float initialHealth,
		const PositionF& position,
		const RotationF& rotation) {
		bool sendResult = false;
		try {
			SpawnActionData* spawnAction =
				new SpawnActionData(matchGuid, actionTimeEpoch, playerGuid, characterGuid,
					teamGuid, initialHealth, position, rotation);
			std::deque<BaseActionData*> actions = { spawnAction };
			sendResult = SendActions(actions);
			delete spawnAction;
		}
		catch (std::exception& _error)
		{
			logger.Log(LogType::FATAL, std::string("GetgudSDK::SendSpawnAction->Couldn't "
				"send actions to Action Buffer: ") +
				std::string(_error.what()));
		}
		return sendResult;
	}

	/**
	 * SendDeathAction:
	 *
	 **/
	bool SendDeathAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const std::string& attackerGuid) {
		bool sendResult = false;
		try {
			DeathActionData* deathAction =
				new DeathActionData(matchGuid, actionTimeEpoch, playerGuid, attackerGuid);
			std::deque<BaseActionData*> actions = { deathAction };
			sendResult = SendActions(actions);
			delete deathAction;
		}
		catch (std::exception& _error)
		{
			logger.Log(LogType::FATAL, std::string("GetgudSDK::SendDeathAction->Couldn't "
				"send actions to Action Buffer: ") +
				std::string(_error.what()));
		}
		return sendResult;
	}

	/**
	 * SendPositionAction:
	 *
	 **/
	bool SendPositionAction(const std::string& matchGuid,
		long long actionTimeEpoch,
		const std::string& playerGuid,
		const PositionF& position,
		const RotationF& rotation) {
		bool sendResult = false;
		try {
			PositionActionData* positionAction = new PositionActionData(
				matchGuid, actionTimeEpoch, playerGuid, position, rotation);
			std::deque<BaseActionData*> actions = { positionAction };
			sendResult = SendActions(actions);
			delete positionAction;
		}
		catch (std::exception& _error)
		{
			logger.Log(LogType::FATAL, std::string("GetgudSDK::SendPositionAction->Couldn't "
				"send actions to Action Buffer: ") +
				std::string(_error.what()));
		}
		return sendResult;
	}

	/**
	 * SendReports:
	 *
	 * Send reports which are outside of the live match
	 **/
	bool SendReports(int titleId,
		std::string privateKey,
		std::deque<ReportInfo>& reports) {
		try {
			if (sharedReportSenders.reportSendersCount == 0) {
				std::lock_guard<std::mutex> locker(sharedReportSenders.reportSendersMutex);
				if (sharedReportSenders.reportSendersCount == 0)  // double check in mutex
				{
					sharedReportSenders.reportSender = new ReportSender();
					sharedReportSenders.reportSender->Start(
						sdkConfig.gameSenderSleepIntervalMilliseconds);
				}
			}

			return sharedReportSenders.reportSender->AddReports(titleId, privateKey,
				reports);
		}
		catch (std::exception& _error) {
			logger.Log(
				LogType::FATAL,
				std::string(
					"GetgudSDK::SendReports->Failed, reports can not be sent: ") +
				std::string(_error.what()));
			return false;
		}
	}

	/**
	 * SendReports:
	 *
	 * Send reports which are outside of the live match
	 **/
	bool SendReports(std::deque<ReportInfo>& reports) {
		std::string titleId;
		std::string privateKey;
		bool result = false;
		try {

			const char* titleIdVar = std::getenv("GETGUD_TITLE_ID");
			const char* privateKeyVar = std::getenv("GETGUD_PRIVATE_KEY");

			if (titleIdVar == nullptr || privateKeyVar == nullptr) {
				logger.Log(
					LogType::FATAL,
					std::string(
						"GetgudSDK::SendReports->Environment variables GETGUD_TITLE_ID and "
						"GETGUD_PRIVATE_KEY are required"));
				return false;
			}

			titleId = std::string(titleIdVar);
			privateKey = std::string(privateKeyVar);
			result = SendReports(std::stoi(titleId.c_str()), privateKey, reports);
		}
		catch (std::exception& _error) {
			logger.Log(
				LogType::FATAL,
				std::string(
					"GetgudSDK::SendReports->Failed, reports can not be sent: ") +
				std::string(_error.what()));
			return false;
		}

		return result;

	}

	/**
	 * UpdatePlayers:
	 *
	 * Update player info outside of the live match
	 **/
	bool UpdatePlayers(int titleId,
		std::string privateKey,
		std::deque<PlayerInfo>& players) {
		bool result = false;
		try {
			if (sharedPlayerUpdaters.playerUpdatersCount == 0) {
				std::lock_guard<std::mutex> locker(
					sharedPlayerUpdaters.playerUpdatersMutex);
				if (sharedPlayerUpdaters.playerUpdatersCount == 0)  // double check in mutex
				{
					sharedPlayerUpdaters.playerUpdater = new PlayerUpdater();
					sharedPlayerUpdaters.playerUpdater->Start(
						sdkConfig.gameSenderSleepIntervalMilliseconds);
				}
			}
			result = sharedPlayerUpdaters.playerUpdater->AddPlayers(
				titleId, privateKey, players);
		}
		catch (std::exception& _error) {
			logger.Log(LogType::FATAL,
				std::string("GetgudSDK::UpdatePlayers->Player data "
					"can not be updated: ") +
				std::string(_error.what()));
		}
		return result;
	}

	/**
	 * UpdatePlayers:
	 *
	 * Update player info outside of the live match
	 **/
	bool UpdatePlayers(std::deque<PlayerInfo>& players) {
		std::string titleId;
		std::string privateKey;
		bool result = false;
		try {
			const char* titleIdVar = std::getenv("GETGUD_TITLE_ID");
			const char* privateKeyVar = std::getenv("GETGUD_PRIVATE_KEY");

			if (titleIdVar == nullptr || privateKeyVar == nullptr) {
				logger.Log(
					LogType::FATAL,
					std::string(
						"GetgudSDK::UpdatePlayers->Environment variables GETGUD_TITLE_ID and "
						"GETGUD_PRIVATE_KEY are required"));
				return false;
			}

			titleId = std::string(titleIdVar);
			privateKey = std::string(privateKeyVar);
			result = UpdatePlayers(std::stoi(titleId), privateKey, players);
		}
		catch (std::exception& _error) {
			logger.Log(LogType::FATAL,
				std::string("GetgudSDK::UpdatePlayers->Player data "
					"can not be updated: ") +
				std::string(_error.what()));
		}
		return result;
	}

	/**
	 * Dispose:
	 *
	 **/
	void Dispose() {
		try {

			actionsBuffer.Dispose();
			gameContainer.Dispose();

			if (sharedReportSenders.reportSender != nullptr) {
				sharedReportSenders.reportSender->Dispose();
				sharedReportSenders.reportSender = nullptr;
			}

			if (sharedPlayerUpdaters.playerUpdater != nullptr) {
				sharedPlayerUpdaters.playerUpdater->Dispose();
				sharedPlayerUpdaters.playerUpdater = nullptr;
			}

			for (auto& senderThread : sharedGameSenders.gameSenders) {
				senderThread->Dispose();
			}

			sharedGameSenders.gameSenders.clear();
			sharedGameSenders.gameSendersCount = 0;

			curl_global_cleanup();
			

#ifdef _DEBUG
			sdkConfig.actionsAmount = 0;
			sdkConfig.totalCreatedActions = 0;
			sdkConfig.emptyActionsAmount = 0;
			sdkConfig.totalCreatedEmptyActions = 0;
#endif

			logger.Log(LogType::DEBUG, std::string("SDK stopped.."));
		}
		catch (std::exception& _error) {
			logger.Log(LogType::FATAL,
				std::string("GetgudSDK::Dispose->Couldn't dispose SDK: ") + std::string(_error.what()));
		}
	}

}  // namespace GetgudSDK