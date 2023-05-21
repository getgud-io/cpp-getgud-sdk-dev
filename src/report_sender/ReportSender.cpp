#include "ReportSender.h"
#include "../config/Config.h"
#include "../logger/Logger.h"

namespace {  // unnamed namespace
size_t CURLWriteCallback(char* contents,
                         size_t size,
                         size_t nmemb,
                         void* userp) {
  if (userp == nullptr)
    return 0;
  // curl call bacl to grab the response from middleware correctly
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

}  // namespace

namespace GetGudSdk {
extern Logger logger;
extern Config sdkConfig;

ReportSender::ReportSender() {
  InitCurl();
}

void ReportSender::Start(int sleepIntervalMilli) {
  sleepTimeMilli = sleepIntervalMilli;
  threadWorking = true;
  logger.Log(LogType::DEBUG,
             "Player Updater is starting with the sleep interval of " +
                 std::to_string(sleepIntervalMilli));

  updaterThread = std::thread([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(
        sdkConfig.hyperModeThreadCreationStaggerMilliseconds));
    while (threadWorking) {
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMilli));
      SendNextReportBatch();
    }
  });
}

bool ReportSender::AddReports(int titleId,
                              std::string privateKey,
                              std::deque<ReportInfo*>& arrivingReportVector) {
  // before doing anything, make sure the report vector has enough room for more
  // reports
  if (reportVector.size() > sdkConfig.reportsMaxBufferSizeInBytes) {
    logger.Log(LogType::DEBUG,
               std::string("ReportSender::AddReports->Reached max report "
                           "buffer size, cannot add more reports"));
    return false;
  }

  reportSenderMutex.lock();

  // insert the arriving new reports to the report vector, to be consumed later
  // on by the background thread
  for (int index = 0; index < arrivingReportVector.size(); index++) {
    // TODO change pointer in order to prevents pointers convertion
    reportVector.push_back(
        new ReportData(titleId, privateKey, *arrivingReportVector[index]));
    reportBufferSize += GetReportDataSize();
  }

  reportSenderMutex.unlock();
  return true;
}

void ReportSender::SendNextReportBatch() {
  // Check the max allowed reports that we can send in a single https request
  int maxReportsToSendAtOnce = sdkConfig.maxReportsToSendAtOnce;

  std::vector<ReportData*> reportsBatch;
  std::vector<ReportData*>::iterator it;
  ReportData* reportData;

  reportSenderMutex.lock();

  // check if there are any reports to send
  int reportsCount = reportVector.size();
  if (reportsCount == 0) {
    reportSenderMutex.unlock();
    return;
  }

  // the first arriving report will determine the title id for the next packet
  // -> find reports with the same title to create the packet of reports
  int packetTitleId = reportVector[0]->titleId;
  for (int index = 0; index < reportsCount || index < maxReportsToSendAtOnce;
       index++) {
    reportData = reportVector[index];
    if (reportData->titleId == packetTitleId) {
      reportsBatch.push_back(reportData);
      it = reportVector.begin() + index;
      reportVector.erase(it);
      reportsCount--;
      reportBufferSize -= GetReportDataSize();
    }
  }

  reportSenderMutex.unlock();

  // Convert the reportsBatch into reports packet
  std::string reportsPacket;
  reportsPacket += +"{\"reports\":[";
  for (ReportData* report : reportsBatch) {
    reportsPacket += report->ToString();
  }
  reportsPacket += +"]}";

  
  SendReportPacket(reportsPacket);

}

void ReportSender::SendReportPacket(std::string& packet) {
  if (!curl || packet.empty()) {
    return;
  }
  curlReadBuffer.clear();
  curl_easy_setopt(curl, CURLOPT_URL, sdkConfig.sendReportsURL.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, packet.c_str());

  // time when we should stop trying to send packet
  auto stopWaitingTime =
      std::chrono::system_clock::now() +
      std::chrono::milliseconds(sdkConfig.apiWaitTimeMilliseconds);

  CURLcode sendCode = CURLE_UNKNOWN_OPTION;

  while (sendCode != CURLE_OK &&
         stopWaitingTime > std::chrono::system_clock::now()) {
    // send prepared packet
    sendCode = curl_easy_perform(curl);

    // make small delay in order to save hardware usage
    std::this_thread::sleep_for(std::chrono::microseconds(50));
  }

  if (sendCode == CURLcode::CURLE_OK) {
    logger.Log(LogType::DEBUG, "Packet sent: " + curlReadBuffer);
  } else {
    logger.Log(LogType::_ERROR, "Failed to send packet: " +
                                    std::string(curl_easy_strerror(sendCode)));
  }
}

void ReportSender::InitCurl() {
  logger.Log(LogType::DEBUG, "REST Api connection prepared in ReportSender");

  curl = curl_easy_init();

  if (!curl) {
    logger.Log(LogType::WARN, "ReportSender->initCurl Couldn't init curl");
  }
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  // TODO: describe what this does
  curl_easy_setopt(curl, CURLOPT_POST, 1);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, sdkConfig.sendReportsURL.c_str());
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, sdkConfig.apiTimeoutMilliseconds);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlReadBuffer);
}

ReportSender::~ReportSender() {
  reportSenderMutex.lock();
  logger.Log(LogType::DEBUG, "Disposing Report Sender");
  // Iterate through all the actions in the buffer and delete them
  for (auto* reportData : reportVector) {
    delete reportData;
  }

  reportVector.clear();
  reportBufferSize = 0;
  Dispose();  // TODO: do i call inside or outside lock?

  reportSenderMutex.unlock();
}

void ReportSender::Dispose() {
  threadWorking = false;
  updaterThread.detach();

  if (curl) {
    curl_easy_cleanup(curl);
  }
  curl_slist_free_all(headers);
  logger.Log(LogType::DEBUG, "Report Sender Disposed");
}

}  // namespace GetGudSdk