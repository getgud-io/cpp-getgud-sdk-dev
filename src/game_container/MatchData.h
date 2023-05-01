#pragma once

#include "../../include/actions/BaseActionData.h"
#include "ChatMessageData.h"
#include "ReportData.h"
#include <deque>
#include <mutex>

namespace GetGudSdk
{
	class MatchData
	{
	private:
		std::string match_guid;
		std::string match_mode;
		std::string map_name;
		unsigned int actions_size = 0;
		unsigned int size_in_bytes = 0;
		// TODO: add reports and chat history
		std::deque<BaseActionData*> match_actions;

	private:
		/**
		* calculate_size:
		*
		* Calculate size of match metadata
		**/
		void calculate_size();

	public:
		/**
		* MatchData:
		* @_match_guid: guid of the match
		* @_match_mode: mode of the match
		* @_map_name: map name of the match
		*  
		* Match data is part of Game Packet that will sent to GetGud. 
		* Every match has metadata, actions, chat and reports associated with it
		**/
		MatchData(std::string _match_guid, std::string _match_mode, std::string _map_name);
		~MatchData();

		/**
		* get_match_guid:
		*
		**/
		std::string& get_match_guid();

		/**
		* get_map_name:
		*
		**/
		std::string& get_map_name();

		/**
		* get_match_mode:
		*
		**/
		std::string& get_match_mode();

		/**
		* get_match_size:
		*
		**/
		unsigned int get_match_size();

	// TODO: should be private, only accessible by GameSender
	public: 

		/**
		* push_actions:
		* @_actions: deque of actions that need to be added to the match
		*
		* Add actions to the match metadata of game packet
		**/
		unsigned int push_actions(std::deque<BaseActionData*>& _actions);

		/**
		* get_actions:
		*
		**/
		std::deque<BaseActionData*>& get_actions();

		/**
		* get_sorted_actions:
		* @actions_out: 
		*
		* Sort all alctions of this match w.r.t action timestamp
		* TODO: describe difference between sort_actions and get_sorted_actions
		**/
		void get_sorted_actions(std::deque<BaseActionData*>& actions_out);

		/**
		* sort_actions:
		*
		* Sort all alctions of this match w.r.t action timestamp
		**/
		void sort_actions();

		/**
		* dispose_actions:
		*
		* Delete all the actions which belong to this match in the game packet
		**/
		void dispose_actions();
	};
}