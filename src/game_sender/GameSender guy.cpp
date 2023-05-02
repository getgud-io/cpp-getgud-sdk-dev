#include "GameSender.h"
#include "../logger/Logger.h"
#include "../../include/config/Config.h"
#include "../game_container/GameContainer.guy.h"

namespace GetGudSdk {
extern Logger logger;
extern ActionsBuffer actions_buffer;
extern GameContainer game_container;
extern Config sdk_config;

GameSender::GameSender() 
{
    init_curl();
}

GameSender::~GameSender() 
{
    logger.log(LogType::MESSAGE, LogLevel::Processes, "Game sender disposed");
    stop_required = true;
    updater_thread.detach();
}

void GameSender::init_curl() 
{
    logger.log(LogType::MESSAGE, LogLevel::Responses, "REST Api connection prepared");

    curl = curl_easy_init();
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");

    // TODO: describe what this does
    curl_easy_setopt(curl, CURLOPT_POST, 1); 
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, sdk_config.rest_api_link);
}

void GameSender::start(int sleep_interval_milli) 
{
    thread_working = true;
    logger.log(LogType::MESSAGE, LogLevel::Processes, "Game Sender is starting with the sleep interval of" + sleep_in_milli); 

    updater_thread = std::thread([&]() 
    {
        std::this_thread::sleep_for(std::chrono::seconds(sleep_in_milli * 1000));
        send_next_game(); 
    });
}

void GameSender::send_next_game() 
{
    std::string game_out;

    // first thing is first, grab all the actions from from the buffer and inject them to the game container
    bool result = game_container.add_actions(actions_buffer.popActions()); // this might fail for many reasons (memory space is one of them)
    if (result == false) logger.log(LogType::WARNING, LogLevel::Processes, "GameSender::send_next_game()->Failed to add actions to GameContainer (it's probably full)"); 

    // get the next game we need to send. might be that there are no games to send
    GameData game_data_to_send = game_container.pop_next_game_to_process();
    if(game_data_to_send == null) return;

    // convert the game to a sendable string and send it to Getgud.io's could using curl
    send_game_packet(game_data_to_send.game_to_string(&game_out));
}

CURLcode GameSender::send_packet(std::string& packet) {
  if (!curl) 
  {
    return CURLcode::CURLE_SEND_ERROR;
  }

  // set packet string to send
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                   packet.c_str());             
  
  // send data
  CURLcode send_code = curl_easy_perform(curl); 

  return send_code;
}


}  // namespace GetGudSdk