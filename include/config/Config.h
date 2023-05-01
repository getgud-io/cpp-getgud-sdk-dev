#include <string>
#include <map>

#include "../actions/Helpers.h"

namespace GetGudSdk {
  /**
   * ActionStreamFileConfig:
   *
   * Define REST API keys in order to send Action Stream, so we can easily rename
   * fields in case API changes
   * TODO: not match_guid, but match_guid_field_name, use the same for other fields
   **/
  struct ActionStreamFileConfig {
    const std::string match_guid = "match_guid";
    const std::string player_guid = "player_guid";
    const std::string action_type = "action_type";
    const std::string action_time_epoch = "action_time_epoch";
    const std::string position = "position";
    const std::string initial_health = "initial_health";
    const std::string character_guid = "character_guid";
    const std::string team_id = "team_id";
    const std::string victim_player_guid = "victim_player_guid";
    const std::string damage_done = "damage_done";
    const std::string weapon_guid = "weapon_guid";
    const std::string health_gained = "health_gained";
  };


  /**
  * SdkFileConfig:
  *
  * Define names of the fields in SDK, so we can easily change names
  * in case we want a better name for a variable
  * TODO: not rest_api_link, but rest_api_link_field_name, use the same for other fields
  **/
  struct SdkFileConfig {
    const std::string rest_api_link = "rest_api_link";
    const std::string private_key = "private_key";
    const std::string send_packet_interval_seconds = "send_packet_interval_seconds";
    const std::string send_packet_at_percentage = "send_packet_at_percentage";
    const std::string game_senders_limit = "game_senders_limit";
    const std::string actions_buffer_max_size = "actions_buffer_max_size";
    const std::string packets_size_mb = "packets_size_mb ";
    const std::string api_timeout_seconds = "api_timeout_seconds";
    const std::string max_games = "max_games";
    const std::string max_matches_per_game = "max_matches_per_game";
    const std::string live_game_timeout_seconds = "live_game_timeout_seconds";
    const std::string hyper_speed_feature_enabled = "hyper_speed_feature_enabled";
    const std::string hyper_speed_at_buffer_percentage = "hyper_speed_at_buffer_percentage";
    const std::string log_level = "log_level";
    const std::string log_type = "log_type";
  };

  /**
   * Config:
   * @rest_api_link: DNS address to middleware
   * @private_key: Private key which you get when you create_title
   * @send_packet_interval_seconds: Time interval between sending packets
   * @game_senders_limit: Maximum number of game senders allowed
   * @actions_buffer_max_size: Maximum size of the actions buffer
   * @packets_size_mb: Maximum size of a packet in MB
   * @api_timeout_seconds: API timeout in seconds
   * @max_games: Maximum number of games allowed
   * @max_matches_per_game: Maximum number of matches per game
   * @live_game_timeout_seconds: Live game timeout in seconds
   * @hyper_speed_feature_enabled: Flag to enable or disable hyper-speed feature; TODO: hyper_mode not hyper_speed
   * @hyper_speed_at_buffer_percentage: Percentage of buffer usage to trigger hyper-speed
   * @hyper_speed_at_actions_size: Size of actions in buffer to trigger a game sender
   * @rest_api_keys: Configuration for REST API keys
   * @sdk_keys: Configuration for SDK keys
   * @config_path: Path to config file
   * @config_filename: Name of the config file
   * @logs_path: Path to logs folder
   * @logs_filename: Name of the logs file
   * @log_level: Log level setting
   * @log_type: Log type setting
   *
   * Define a configuration for the SDK features, so all SDK features can be controlled by
   * the user. Config is a mix of ActionStreamFileConfig and SdkFileConfig
   * TODO: add varible for delay to send
   * TODO: only this class should be in header file, hide structs
   **/
  class Config {
  public:
    std::string rest_api_link; //+ 
    std::string private_key; //+ 
    unsigned int send_packet_interval_seconds = 0; //+
    unsigned int send_packet_at_percentage = 0; //+
    unsigned int game_senders_limit = 0; //+
    unsigned int actions_buffer_max_size = 0; //+
    unsigned int packets_size_mb = 0; //+
    unsigned int api_timeout_seconds = 0; //+
    unsigned int max_games = 0; //+ 
    unsigned int max_matches_per_game = 0; //+
    unsigned int live_game_timeout_seconds = 0; //+
    bool hyper_speed_feature_enabled = 0;
    unsigned int hyper_speed_at_buffer_percentage = 0;
    const ActionStreamFileConfig rest_api_keys; //+
    const SdkFileConfig sdk_keys; //+
    std::string config_path = "./GetGudSdk/";
    std::string config_filename = "config.txt";
    std::string logs_path = "./GetGudSdk/"; //+
    std::string logs_filename = "logs.txt"; //+
    LogLevel log_level = LogLevel::Full; //+
    LogType log_type = LogType::MESSAGE; //+

    /**
    * Config:
    * 
    * Constructor of Config, properties will manage the SDK features
    **/
    Config();

    /**
    * load_settings:
    * 
    * Get values of variables from the config file
    **/
    bool load_settings();

  private:
    /**
    * read_user_config_file:
    * 
    * Read all config values from the file
    * TODO: test this
    **/
    std::map<std::string, std::string> read_user_config_file();

    /**
    * find_value:
    * @values: Map with all config values
    * @key: specific key we need value for in the Map
    * 
    * Find specific value in the map
    **/
    std::string find_value(
      std::map<std::string,
      std::string>& values,
      std::string key
    );

    /**
    * get_config_value:
    * @user_config: Map with all config values
    * @key: specific key we need value for in the Map
    * @value_out: the variable we need to set from the config
    * 
    * Set specific value from the config map
    **/
    void get_config_value(
      std::map<std::string, std::string>& user_config,
      std::string key_in,
      std::string& value_out
    );

    /**
    * get_config_value:
    * @user_config: Map with all config values
    * @key: specific key we need value for in the Map
    * @value_out: the variable we need to set from the config
    * 
    * Set specific value from the config map
    **/
    void get_config_value(
      std::map<std::string,
      std::string>& user_config,
      std::string key_in,
      unsigned int& value_out
    );

    /**
    * get_config_value:
    * @user_config: Map with all config values
    * @key: specific key we need value for in the Map
    * @value_out: the variable we need to set from the config
    * 
    * Set specific value from the config map
    **/
    void get_config_value(
      std::map<std::string,
      std::string>& user_config,
      std::string key_in,
      bool& value_out
    );

    /**
    * get_config_value_enum:
    * @user_config: Map with all config values
    * @key: specific key we need value for in the Map
    * @value_out: the variable we need to set from the config TODO: just return this variable, don't set it inside, this is inconsistent
    * 
    * Set specific value from the config map
    **/
    template <typename T>
    void get_config_value_enum(
      std::map<std::string, std::string>& user_config,
      std::string key_in,
      T& value_out
    );
  };
}  // namespace GetGudSdk