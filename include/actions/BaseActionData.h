#pragma once

#include <map>
#include <string>

#include "Helpers.h"

namespace GetGudSdk {
    /**
     * Define Base Action. Other actions can be created with the use of BaseAction
     * Other actions inherit from BaseAction too
     **/
    class BaseActionData {
    public:
        int id = 0;                           // Id of the action
        unsigned int size = 0;                // Size of the action in bytes
        Actions action_type = Actions::none;
        long action_time_epoch = 0;
        std::string
            player_guid;  // player_guid of the player who performed base action

        std::string
            match_guid;  // match_guid of the match where base action took place

    public:
        /**
         * BaseActionData:
         * @data: BaseData, structure contains all core metadata for each action
         *
         * Constructor of Base Action, all other actions inherit from BaseAction
         **/
        BaseActionData(BaseData data);

        BaseActionData() = delete;

        /**
         * BaseActionData:
         * @copy: BaseData instance to copy from
         *
         * Constructor of Base Action, all other actions inherit from BaseAction
         **/
        BaseActionData(BaseActionData& copy);

        /**
        * ~BaseActionData:
        *
        * Destrctor, called when we delete actions from the system
        **/
        virtual ~BaseActionData();

        /**
         * operator:
         * @a: left action
         * @b: right action
         *
         * Compares actions to each other wrt timestamp, 
         * uses by std::sort, where sorting based on action_time_epoch
         **/
        bool operator()(BaseActionData& a, BaseActionData& b);

       /**
        * get_data:
        *
        * Mainly from inheritance from other action classes, BaseActionData doesn't need this
        **/
        virtual std::map<std::string, std::string> get_data();

        /**
        * get_action_stream:
        *
        * Mainly from inheritance from other action classes, BaseActionData doesn't need this
        **/
        virtual std::string get_action_stream();

        /**
        * clone:
        *
        * Clone action
        **/
        virtual BaseActionData* clone();

        /**
         * calculate_size:
         *
         * Virtual function that will be replaced by an action,
         * Update size of an action, calls from the cunstructors
         **/
        virtual void calculate_size();

        /**
         * get_data_size:
         *
         * returns size in bytes of an action
         **/
        unsigned int get_data_size();
    };
}  // namespace GetGudSdk