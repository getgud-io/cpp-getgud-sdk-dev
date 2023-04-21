#include "GameContainer.h"

namespace GetGudSdk
{
	//create a new shared packets buffer
	GameContainer game_container;

	bool GameContainer::add_game(GameData game_data)
	{
		buffer_locker.lock();

		//send end game if game already exists
		for (auto& game : buffer) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_data.get_game_guid()) //if game found
			{
				buffer_locker.unlock();
				return false; //game guid already exists
			}
		}

		buffer.push_back(game_data);
		buffer_locker.unlock();
		return true;
	}

	bool GameContainer::set_end_game(std::string game_guid)
	{
		buffer_locker.lock();
		for (auto& game : buffer) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_guid) //if game found
			{
				auto current_point = std::chrono::system_clock::now(); // get current time

				game.set_end_game(current_point + std::chrono::seconds(5)); // add 5 sec delay to current time (config)
			}
		}
		buffer_locker.unlock();
		return true;
	}

	bool GameContainer::add_match(std::string game_guid, MatchData match_data)
	{
		bool result = false;
		buffer_locker.lock();
		for (auto& game : buffer) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_guid) //if game found
			{
				result = game.add_match(match_data);
			}
		}
		buffer_locker.unlock();
		return result;
	}

	bool GameContainer::add_report(std::string match_guid, ReportData report_data)
	{
		buffer_locker.lock();

		auto reports_it = reports.find(match_guid); //try to find a match in buffer

		if (reports_it != reports.end()) //if match exists
		{
			reports_it->second.push_back(report_data); //append report to match
		}
		else //if match doesnt exist
		{
			std::deque<ReportData> _report_buffer; //make new buffer
			_report_buffer.push_back(report_data); //append report

			reports[match_guid] = _report_buffer; //assign report to match
		}

		buffer_locker.unlock();

		return true;
	}

	bool GameContainer::add_chat_message(std::string match_guid, ChatMessageData message_data)
	{
		buffer_locker.lock();

		auto messages_it = chat_messages.find(match_guid); //try to find a match in buffer

		if (messages_it != chat_messages.end()) //if match exists
		{
			messages_it->second.push_back(message_data); //append report to match
		}
		else //if match doesnt exist
		{
			std::deque<ChatMessageData> _messages_buffer; //make new buffer
			_messages_buffer.push_back(message_data); //append report

			chat_messages[match_guid] = _messages_buffer; //assign report to match
		}

		buffer_locker.unlock();

		return true;
	}

	void GameContainer::dispose()
	{
		buffer_locker.lock();
		buffer.clear();
		buffer_locker.unlock();
	}

#ifdef _DEBUG
	std::deque<GameData>& GameContainer::get_buffer()
	{ 
		return buffer; 
	};

	std::map<std::string, MatchData>* GameContainer::get_matches(std::string game_guid)
	{
		for (auto& game : buffer) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_guid) //if game found
			{
				return &game.get_matches_buffer();
			}
		}

		return nullptr;
	};
#endif

}