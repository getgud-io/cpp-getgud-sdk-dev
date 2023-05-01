#include "../../include/config/Config.h"

//file modification verification
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#define stat _stat
#endif

#include <iostream>
#include <memory>
#include <fstream>

time_t config_edit_time = 0;

namespace GetGudSdk {
Config sdk_config;

/**
* Config:
* 
* Constructor of Config, properties will manage the SDK features
**/
Config::Config() {
}

/**
* load_settings:
* 
* Get values of variables from the config file
* TODO: rename sdk_config_key_names
**/
bool Config::load_settings() {
  std::map<std::string, std::string> values = read_user_config_file(); 

  get_config_value(values, sdk_keys.rest_api_link, rest_api_link);
  get_config_value(values, sdk_keys.private_key, private_key);
  get_config_value(values, sdk_keys.send_packet_interval_seconds, send_packet_interval_seconds);
  get_config_value(values, sdk_keys.send_packet_at_percentage, send_packet_at_percentage);
  get_config_value(values, sdk_keys.game_senders_limit, game_senders_limit);
  get_config_value(values, sdk_keys.actions_buffer_max_size, actions_buffer_max_size);
  get_config_value(values, sdk_keys.packets_size_mb , packets_size_mb );
  get_config_value(values, sdk_keys.api_timeout_seconds, api_timeout_seconds);
  get_config_value(values, sdk_keys.max_games, max_games);
  get_config_value(values, sdk_keys.max_matches_per_game, max_matches_per_game);
  get_config_value(values, sdk_keys.live_game_timeout_seconds, live_game_timeout_seconds);
  get_config_value(values, sdk_keys.hyper_speed_feature_enabled, hyper_speed_feature_enabled);
  get_config_value(values, sdk_keys.hyper_speed_at_buffer_percentage, hyper_speed_at_buffer_percentage);

  get_config_value_enum(values, sdk_keys.log_level, log_level);
  get_config_value_enum(values, sdk_keys.log_type, log_type);

  return true;
}

/**
* read_user_config_file:
* 
* Read all config values from the file
* TODO: test this
**/
std::map<std::string, std::string> Config::read_user_config_file() {
  std::map<std::string, std::string> output_map;
  std::string key;
  std::string value;

  bool key_started = false;
  bool key_ended = false;
  bool value_started = false;

  // TODO: rename all the variables here, make this function CLEAR
  std::ifstream in(config_path + config_filename);
  char c;

  // Read config values from the file so we can later map them to map
  if (in.is_open()) {
    while (in.good()) {
      in.get(c);

      if (c == '\t')
        continue;

      if (key_started && c != '\"' && c != ',' && c != '\n') {
        key += c;
      } else if (value_started && c != '\"' && c != ',' && c != '\n' &&
                 c != '}') {
        value += c;
      }

      if (c == '\"' || c == ',' || (c == '\n' && key_ended) ||
          (c == ':' && key_ended && !value_started)) {
        if (value_started && c == '\"')
          continue;

        if (key_ended && !value_started) {
          value_started = true;
        } else if (key_ended && value_started) {
          value_started = false;
        } else if (!key_ended && !key_started) {
          key_started = true;
        } else if (!key_ended && key_started) {
          key_started = false;
          key_ended = true;
        }
      }
      // map key value to map
      if ((c == ',' || c == '}') && key_ended) {
        output_map.insert(std::make_pair(key, value));
        key.clear();
        value.clear();
        key_started = false;
        key_ended = false;
        value_started = false;
      }
    }
  }

  in.close();

  return output_map;
}

/**
* find_value:
* @values: Map with all config values
* @key: specific key we need value for in the Map
* 
* Find specific value in the map
**/
std::string Config::find_value(std::map<std::string, std::string>& values,
                      std::string key) {
  std::string value;

  auto itr_out = values.find(key);
  if (itr_out != values.end())
    value = itr_out->second;

  return value;
}

/**
* get_config_value:
* @user_config: Map with all config values
* @key: specific key we need value for in the Map
* @value_out: the variable we need to set from the config TODO: just return this variable, don't set it inside, this is inconsistent
* 
* Set specific value from the config map
**/
void Config::get_config_value(std::map<std::string, std::string>& user_config,
                              std::string key_in,
                              std::string& value_out) {
  std::string value = find_value(user_config, sdk_keys.rest_api_link);
  if (!value.empty())
    value_out = value;
}

/**
* get_config_value:
* @user_config: Map with all config values
* @key: specific key we need value for in the Map
* @value_out: the variable we need to set from the config
* 
* Find specific value from the config map by key
**/
void Config::get_config_value(std::map<std::string, std::string>& user_config,
                              std::string key_in,
                              unsigned int& value_out) {
  std::string value = find_value(user_config, sdk_keys.rest_api_link);
  if (!value.empty())
    value_out = std::stoi(value);
}

void Config::get_config_value(std::map<std::string, std::string>& user_config,
                              std::string key_in,
                              bool& value_out) {
  std::string value = find_value(user_config, sdk_keys.rest_api_link);
  if (!value.empty())
    value_out = std::stoi(value);
}

/**
* get_config_value_enum:
* @user_config: Map with all config values
* @key: specific key we need value for in the Map
* @value_out: the variable we need to set from the config
* 
* Set specific value from the config map
**/
template <typename T>
void Config::get_config_value_enum(
    std::map<std::string, std::string>& user_config,
    std::string key_in,
    T& value_out) {
  std::string value = find_value(user_config, sdk_keys.rest_api_link);
  if (!value.empty())
    value_out = static_cast<T>(std::stoi(value));
}

}