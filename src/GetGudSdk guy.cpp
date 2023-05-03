#include "../include/GetGudSdk.h"
#include "game_sender/GameSender.h"
#include "logger/Logger.h"
#include "utils/Utils.h"

#include "../include/config/Config.h"

namespace GetGudSdk {
  std::vector<GameSender*> senders;

  extern Config sdk_config;
  extern Logger logger;
  extern ActionsBuffer actions_buffer;
  extern GameContainer game_container;
  
  extern ReportSender game_container;
  game_container.start();

  bool send_reports(std::vector<ReportData*> report_vector)
  {
      bool result = ReportSender.add_reports(report_vector);

      return result;
  }


} // namespace GetGudSdk