#include "ReportSender.h"
#include "../logger/Logger.h"
#include "../../include/config/Config.h"
#include "../game_container/GameContainer.guy.h"

namespace GetGudSdk {
extern Logger logger;
extern ActionsBuffer actions_buffer;
extern GameContainer game_container;
extern Config sdk_config;

ReportSender::ReportSender() 
{
    init_curl();
}

void ReportSender::start(int sleep_interval_milli) 
{
    thread_working = true;
    logger.log(LogType::MESSAGE, LogLevel::Processes, "Report Sender is starting with the sleep interval of" + sleep_in_milli); 

    updater_thread = std::thread([&]() 
    {
        std::this_thread::sleep_for(std::chrono::seconds(sleep_in_milli * 1000));
        send_next_report_batch(); 
    });
}

bool ReportSender::add_reports(std::vector<ReportData*> arriving_report_vector) 
{
    // before doing anything, make sure the report vector has enough room for more reports
    if (aGame_vector.size() > sdk_config.reports_max_buffer_size)
    {
        logger.log(LogType::WARNING, LogLevel::Processes, std::string("ReportSender::add_reports->Reached max report buffer size, cannot add more reports"));
        return false;
    }

    report_sender_mutex.lock();

    // insert the arriving new reports to the report vector, to be consumed later on by the background thread
    for(int index = 0; index < arriving_report_vector.size(); index++)
    {
        report_vector.insert(report_vector.begin(), arriving_report_vector[index].clone());
    }
    
    report_sender_mutex.unlock();
}


void ReportSender::send_next_report_batch() 
{
    // check the max allowed reports that we can send in a single https request
    int reportsToSend = sdk_config.reports_bulk_size;

    // take the above amount of reports from the vector and send them using curl

    // DONE.
}

CURLcode ReportSender::send_report_packet(std::string& packet) 
{
  if (!curl) 
  {
    return CURLcode::CURLE_SEND_ERROR;
  }

  // set packet string to send
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, packet.c_str());             
  
  // send data
  CURLcode send_code = curl_easy_perform(curl); 

  return send_code;
}

void ReportSender::init_curl() 
{
    logger.log(LogType::MESSAGE, LogLevel::Responses, "REST Api connection prepared");

    curl = curl_easy_init();
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charset: utf-8");

    curl_easy_setopt(curl, CURLOPT_POST, 1); 
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_URL, sdk_config.send_reports_api_link);
}

ReportSender::~ReportSender() 
{
    logger.log(LogType::MESSAGE, LogLevel::Processes, "Game sender disposed");
    stop_required = true;
    updater_thread.detach();
}


}  // namespace GetGudSdk