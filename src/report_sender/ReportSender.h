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

  class CredentialsReportData : public ReportData {
 public:
    int m_titleId = 0;
    std::string m_privateKey;
    CredentialsReportData(int titleId,
                          std::string privateKey,
                          ReportInfo reportInfo);
    bool IsValid() override;
  };

class ReportSender {
 public:
  ReportSender();
  void Start(int sleepIntervalMilli);
  bool AddReports(int titleId,
                  std::string privateKey,
                  std::deque<ReportInfo>& reportVector);
  ~ReportSender();
  void Dispose();

 private:
  // holds all the reports being sent by the client to the SDK
  std::deque<CredentialsReportData*> m_reportVector;

  int m_sleepTimeMilli = 0;
  std::thread m_updaterThread;
  bool m_threadWorking = false;
  curl_slist* m_headers = NULL;
  CURL* m_curl = nullptr;
  std::string m_curlReadBuffer;
  std::mutex m_reportSenderMutex;
  unsigned int m_reportBufferSize = 0;

 private:
  void InitCurl();
  void SendNextReportBatch();
  void SendReportPacket(std::string& packet);
};
}  // namespace GetGudSdk