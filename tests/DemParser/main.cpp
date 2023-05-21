#include <GetGudSdk.h>
#include <iostream>

#include <chrono>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <thread>

std::vector<std::pair<std::string, std::vector<std::string>>> readData(
    std::string dem_file_address) {
  std::vector<std::pair<std::string, std::vector<std::string>>> output_map;

  std::string function_name;
  std::vector<std::string> function_params;
  std::string param;

  std::ifstream file_stream(dem_file_address);
  char next_character = 0;

  // Read config values from the file so we can later map them to map
  if (file_stream.is_open()) {
    file_stream.get(next_character);
    while (file_stream.good()) {
      while (next_character != ':' && next_character != '\n') {
        function_name += next_character;
        file_stream.get(next_character);
      }
      file_stream.get(next_character);
      while (next_character != '\n') {
        if (next_character == ';') {
          function_params.push_back(param);
          param.clear();
        } else {
          param += next_character;
        }
        file_stream.get(next_character);
      }

      if (!param.empty()) {
        function_params.push_back(param);
        param.clear();
      }

      file_stream.get(next_character);
      if (!function_name.empty())
        output_map.push_back(std::make_pair(function_name, function_params));
      else
        break;
      function_name.clear();
      function_params.clear();
      param.clear();
    }
  }

  file_stream.close();
  return output_map;
}

int main() {
  GetGudSdk::Init();
  auto demFile = readData("game_1.txt");
  std::vector<std::string> matchGuids;
  std::string gameGuid;
  for (auto& function : demFile) {
    if (function.first == "StartGame") {
      gameGuid = GetGudSdk::StartGame(std::stoi(function.second[0]),
                                      function.second[1], function.second[2],
                                      function.second[3]);
    } else if (function.first == "StartMatch") {
      std::string matchGuid = GetGudSdk::StartMatch(gameGuid, function.second[0],
                                                   function.second[1]);
      matchGuids.push_back(matchGuid);
    } else if (function.first == "SendInMatchReport") {
      int match_id = std::stoi(function.second[0]);
      GetGudSdk::SendInMatchReport(
          {
           function.second[0], function.second[1], std::stoi(function.second[2]),
           std::stoi(function.second[3]), function.second[4],
           std::stoi(function.second[5]), std::stoi(function.second[6]),
           std::stol(function.second[7]), std::stoi(function.second[8]),
           std::stol(function.second[9])});
    } else if (function.first == "SendChatMessage") {
      int match_id = std::stoi(function.second[0]);
      GetGudSdk::SendChatMessage(matchGuids[match_id],
                                 {
                                  function.second[1], function.second[2],
                                  std::stol(function.second[3])});
    } else if (function.first == "MarkEndGame") {
      GetGudSdk::MarkEndGame(gameGuid);
    } else if (function.first == "DD") {
      int match_id = std::stoi(function.second[0]);
      GetGudSdk::SendDeathAction(matchGuids[match_id], function.second[1],
                                 std::stoi(function.second[2]));
    } else if (function.first == "D") {
      int match_id = std::stoi(function.second[0]);
      GetGudSdk::SendDamageAction(matchGuids[match_id], function.second[1],
                                  function.second[2], function.second[3],
                                  std::stoi(function.second[4]),
                                  std::stol(function.second[5]));
    } else if (function.first == "A") {
      int match_id = std::stoi(function.second[0]);
      GetGudSdk::SendAttackAction(matchGuids[match_id], function.second[1],
                                  function.second[2], std::stoi(function.second[3]));
    } else if (function.first == "S") {
      int match_id = std::stoi(function.second[0]);
      GetGudSdk::SendSpawnAction(matchGuids[match_id], function.second[1],
                                 function.second[2],
                                 std::stoi(function.second[3]), std::stoi(function.second[4]),
          {std::stof(function.second[5]), std::stof(function.second[6]),
           std::stof(function.second[7])},
          {std::stof(function.second[8]), std::stof(function.second[9])}, std::stol(function.second[10]));
    } else if (function.first == "P") {
      int match_id = std::stoi(function.second[0]);
      GetGudSdk::SendPositionAction(
          matchGuids[match_id], function.second[1],
          {std::stof(function.second[2]), std::stof(function.second[3]),
           std::stof(function.second[4])},
          {std::stof(function.second[5]), std::stof(function.second[6])},
          std::stol(function.second[7]));
    } else if (function.first == "H") {
      int match_id = std::stoi(function.second[0]);
      GetGudSdk::SendHealAction(matchGuids[match_id], function.second[1], std::stoi(function.second[2]), std::stol(function.second[3]));
    }
  }

  while (true) {
  };  // let SDK run in background separetly from the main thread, in order to
      // see the behavior

  return 0;
}