#include "GameContainer.h"

namespace GetGudSdk
{
	//create a new shared packets buffer
	GameContainer game_container;

	bool GameContainer::add_game(GameData game_data)
	{
		games_buffer_locker.lock();

		//send end game if game already exists
		for (auto& game : games) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_data.get_game_guid()) //if game found
			{
				games_buffer_locker.unlock();
				return false; //game guid already exists
			}
		}

		games.push_back(game_data);
		games_buffer_locker.unlock();
		return true;
	}

	bool GameContainer::set_end_game(std::string game_guid)
	{
		games_buffer_locker.lock();
		for (auto& game : games) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_guid) //if game found
			{
				auto current_point = std::chrono::system_clock::now(); // get current time

				game.set_end_game(current_point + std::chrono::seconds(5)); // add 5 sec delay to current time (config)
			}
		}
		games_buffer_locker.unlock();
		return true;
	}

	bool GameContainer::add_match(std::string game_guid, MatchData match_data)
	{
		bool result = false;
		games_buffer_locker.lock();
		for (auto& game : games) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_guid) //if game found
			{
				result = game.add_match(match_data);
			}
		}
		games_buffer_locker.unlock();
		return result;
	}

	bool GameContainer::add_report(std::string match_guid, ReportData report_data)
	{
		games_buffer_locker.lock();

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

		games_buffer_locker.unlock();

		return true;
	}

	bool GameContainer::add_chat_message(std::string match_guid, ChatMessageData message_data)
	{
		games_buffer_locker.lock();

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

		games_buffer_locker.unlock();

		return true;
	}

	void GameContainer::dispose()
	{
		games_buffer_locker.lock();
		games.clear();
		games_buffer_locker.unlock();
	}

	bool GameContainer::add_actions(std::string match_guid, std::deque<BaseActionData*>& actions_buffer)
	{
		bool result = false;
		bool match_found = false;
		games_buffer_locker.lock();
		for (auto& game : games) //parse every game in the buffer
		{
			for (auto& match : game.get_matches_buffer()) //parse every match
			{
				if (match.first == match_guid)
				{
					result = match.second.push_actions(actions_buffer);
					match_found = true;
					break;
				}
			}
			
			if (match_found) //no need additional games check if match found already
				break;
		}
		games_buffer_locker.unlock();
		return result;
	}

	bool GameContainer::add_sorted_actions(std::string match_guid, std::deque<BaseActionData*>& actions_buffer)
	{
		bool result = false;
		bool match_found = false;
		games_buffer_locker.lock();
		for (auto& game : games) //parse every game in the buffer
		{
			for (auto& match : game.get_matches_buffer()) //parse every match
			{
				if (match.first == match_guid)
				{
					result = match.second.push_sorted_actions(actions_buffer);
					match_found = true;
					break;
				}
			}

			if (match_found) //no need additional games check if match found already
				break;
		}
		games_buffer_locker.unlock();
		return result;
	}

	void GameContainer::get_game_stream(std::string& stream_out, bool erase)
	{
		games_buffer_locker.lock();
		if (games.empty())
		{
			games_buffer_locker.unlock();
			return;
		}

		stream_out.clear();

		stream_out += "{ \n";
		stream_out += "	\"privateKey\": \"" + private_key + "\",\n";

		if (erase)
		{
			GameData first_game = games[0];
			games.pop_front();
			games_buffer_locker.unlock();
			first_game.game_to_string(stream_out);
		}
		else
		{
			games[0].game_to_string(stream_out);
			games_buffer_locker.unlock();
		}
		stream_out += "}"; //end packet
	}

#ifdef _DEBUG
	std::deque<GameData>& GameContainer::get_buffer()
	{ 
		return games;
	};

	std::map<std::string, MatchData>* GameContainer::get_matches(std::string game_guid)
	{
		for (auto& game : games) //parse every game in the buffer
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