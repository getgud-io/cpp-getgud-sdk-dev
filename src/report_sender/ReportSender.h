#pragma once

#include <chrono>
#include <deque>
#include <mutex>
#include <thread>
#include <unordered_map>
#include "../game_container/ReportData.h"

#define CURL_STATICLIB
#include <curl/curl.h>

namespace GetGudSdk {

class ReportSender {
 public:
  ReportSender();
  void Start(int sleepIntervalMilli);
  bool AddReports(int titleId,
                  std::string privateKey,
                  std::deque<ReportInfo*>& reportVector);
  ~ReportSender();
  void Dispose();

 private:
  // holds all the reports being sent by the client to the SDK
  std::vector<ReportData*> reportVector;

  int sleepTimeMilli = 0;
  std::thread updaterThread;
  bool threadWorking = false;
  curl_slist* headers = NULL;
  CURL* curl = nullptr;
  std::string curlReadBuffer;
  std::mutex reportSenderMutex;
  unsigned int reportBufferSize = 0;

 private:
  void InitCurl();
  void SendNextReportBatch();
  void SendReportPacket(std::string& packet);
};
}  // namespace GetGudSdk