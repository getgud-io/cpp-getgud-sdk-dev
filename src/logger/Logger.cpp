#include "Logger.h"
#include <fstream>
#include <iostream>
#include "../../include/config/Config.h"
#include "../utils/Utils.h"

namespace GetGudSdk {
Logger logger;  // const initialization at this stage

extern Config sdk_config;
/**
* write_to_file:
* @out_string: file in which to write the output file
* 
* Write the whole log string into the output file so it can be later accessed 
* by client for debugging purposes
**/
void Logger::write_to_file(std::string out_string) {
  std::string logs_path = sdk_config.logs_path;
  std::string logs_filename = sdk_config.logs_filename;

  write_mutex.lock();
  std::ofstream logsfile;
  logsfile.open(std::string(logs_path + logs_filename).c_str(),
                std::ios_base::app);
  logsfile << out_string;
  logsfile.close();
  write_mutex.unlock();
}

/**
* write_message:
* @message: message which will be added to the output file
* 
* Write the message log into the file so it can be later accessed by client
* for debugging purposes
**/
void Logger::write_message(std::string message) {
  LogType type = sdk_config.log_type;

  // if client didn't specify to log messages then we are not logging the message
  if (type != LogType::MESSAGE) 
    return;
  messages_amount++;
  std::string current_time = get_current_time_string();
  std::string message_tag = "::Message-> ";
  std::string message_end =
      "\n\t| total messages: " + std::to_string(messages_amount) + "\n";

  write_to_file(current_time + message_tag + message + message_end);
}

/**
* write_warning:
* @warning: warning which will be added to the output file
* 
* Write the warning log into the file so it can be later accessed by client
* for debugging purposes
**/
void Logger::write_warning(std::string warning) {
  LogType type = sdk_config.log_type;

  // if client didn't specify to log warning or message then we are not logging the warning
  if (type == LogType::WARNING ||
      type == LogType::MESSAGE) 
  {
    warnings_amount++;
    std::string current_time = get_current_time_string();
    std::string warning_tag = "::Warning-> ";
    std::string warning_end =
        "\n\t| total warnings: " + std::to_string(warnings_amount) + "\n";

    write_to_file(current_time + warning_tag + warning + warning_end);
  }
}

/**
* write_error:
* @error: error which will be added to the output file
* 
* Write the error log into the file so it can be later accessed by client
* for debugging purposes
**/
void Logger::write_error(std::string error) {
  // write anyway
  errors_amount++;
  std::string current_time = get_current_time_string();
  std::string error_tag = "::Warning-> ";
  std::string error_end =
      "\n\t| total errors: " + std::to_string(errors_amount) + "\n";

  write_to_file(current_time + error_tag + error + error_end);
}

/**
* write_exception:
* @exception: exception which will be added to the output file
* 
* Write the exception log into the file so it can be later accessed by client
* for debugging purposes
**/
void Logger::write_exception(std::string exception) {
  // write anyway
  exceptions_amount++;
  std::string current_time = get_current_time_string();
  std::string exception_tag = "::Warning-> ";
  std::string exception_end =
      "\n\t| total errors: " + std::to_string(exceptions_amount) + "\n";

  write_to_file(current_time + exception_tag + exception + exception_end);
}

/**
* log:
* @_type: 
* @_level:
* @log_string:
* 
* Central logging operation, decides what to log, how to log and where to log
**/
void Logger::log(LogType _type, LogLevel _level, std::string log_string) {
  LogLevel level = sdk_config.log_level;

  // if client didn't specify anything to log then we are not logging at all
  if (level == LogLevel::NoLogs)
    return;
  // if client specified to log at least something
  if (level == LogLevel::Full ||                           
      (_level == LogLevel::Actions && level == _level) ||  // TODO: what is level == _level??
      (_level <= level && _level != LogLevel::Actions))    // TODO: what the fuck is this part?
  {
    // TODO: let's use if, switch is not clear at all
    // TODO: i don't understand how this switch works why do we treat some strings as errors
    // others we treat as warnings... . Mainly don't undestand how LogType::SMTHING works
    switch (_type)
    {
      case LogType::_ERROR:
        write_error(log_string);
        break;
      case LogType::WARNING:
        write_warning(log_string);
        break;
      case LogType::MESSAGE:
        write_message(log_string);
        break;
    }
  }
}
}  // namespace GetGudSdk