#include "pch.h"
#include "ReportSender.h"
#include "../../config/Config.h"
#include "../../logger/Logger.h"

namespace {

/**
 * CURLWriteCallback:
 *
 * Curl custom callback to grab response from Getgud
 **/
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
extern SharedReportSenders sharedReportSenders;

CredentialsReportData::CredentialsReportData(int titleId,
                                             std::string privateKey,
                                             ReportInfo reportInfo)
    : ReportData(reportInfo), m_titleId(titleId), m_privateKey(privateKey) {}

bool CredentialsReportData::IsValid() {
  bool isActionValid = ReportData::IsValid();
  isActionValid &= Validator::ValidateStringLength(m_privateKey, 1, 10000);
  isActionValid &= Validator::ValidateStringChars(m_privateKey);
  isActionValid &= Validator::ValidateItemValue(m_titleId, 1, INT_MAX);
  return isActionValid;
}

/**
 * ReportSender:
 *
 **/
ReportSender::ReportSender() {
  InitCurl();
  sharedReportSenders.reportSendersCount++;
}

/**
 * Start:
 *
 * Start new Report Sender thread
 **/
void ReportSender::Start(int sleepIntervalMilli) {
  m_sleepTimeMilli = sleepIntervalMilli;
  m_threadWorking = true;
  logger.Log(LogType::DEBUG,
             "Report Sender is starting with the sleep interval of " +
                 std::to_string(sleepIntervalMilli));

  m_updaterThread = std::thread([&]() {
    // Stagger the creation of the threads to avoid them all waking up at the
    // same time
    std::this_thread::sleep_for(std::chrono::milliseconds(
        sdkConfig.hyperModeThreadCreationStaggerMilliseconds));
    while (m_threadWorking) {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTimeMilli));
      SendNextReportBatch();
    }
    this->~ReportSender();
  });
}

/**
 * AddReports:
 *
 * Add new reports to send to Getgud
 **/
bool ReportSender::AddReports(int titleId,
                              std::string privateKey,
                              std::deque<ReportInfo>& arrivingReportVector) {
  // before doing anything, make sure the report vector has enough room for more
  // reports
  if (m_reportVector.size() > sdkConfig.reportsMaxBufferSizeInBytes) {
    logger.Log(LogType::DEBUG,
               std::string("ReportSender::AddReports->Reached max report "
                           "buffer size, cannot add more reports"));
    return false;
  }

  m_reportSenderMutex.lock();

  // insert the arriving new reports to the report vector, to be consumed later
  // on by the background thread
  for (int index = 0; index < arrivingReportVector.size(); index++) {
    CredentialsReportData* credentialsReportData = new CredentialsReportData(
        titleId, privateKey, arrivingReportVector[index]);

    if (!credentialsReportData->IsValid()) {
      delete credentialsReportData;
      m_reportSenderMutex.unlock();
      return false;
    }

    m_reportVector.push_back(credentialsReportData);
    m_reportBufferSize += GetReportDataSize();
  }
  logger.Log(
      LogType::DEBUG,
      std::string("Added " + std::to_string(arrivingReportVector.size()) +
                  " action(s) to Report Sender"));
  m_reportVector.shrink_to_fit();
  m_reportSenderMutex.unlock();
  return true;
}

/**
 * SendNextReportBatch:
 *
 * Send next batch of reports to Getgud
 **/
void ReportSender::SendNextReportBatch() {
  // Check the max allowed reports that we can send in a single https request
  int maxReportsToSendAtOnce = sdkConfig.maxReportsToSendAtOnce;

  std::deque<CredentialsReportData*> reportsBatch;
  CredentialsReportData* reportData;

  m_reportSenderMutex.lock();

  // check if there are any reports to send
  if (m_reportVector.size() == 0) {
    m_reportVector.clear();
    m_reportSenderMutex.unlock();
    return;
  }

  // the first arriving report will determine the title id for the next packet
  // -> find reports with the same title to create the packet of reports
  int packetTitleId = m_reportVector[0]->m_titleId;
  std::string packetPrivateKey = m_reportVector[0]->m_privateKey;
  for (int index = 0; index < m_reportVector.size() &&
                      reportsBatch.size() < maxReportsToSendAtOnce;
       index++) {
    reportData = m_reportVector[index];
    if (reportData->m_titleId == packetTitleId) {
      reportsBatch.push_back(reportData);
      m_reportVector.erase(m_reportVector.begin() + index);
      index--;
      m_reportBufferSize -= GetReportDataSize();
    }
  }
  m_reportVector.shrink_to_fit();
  m_reportSenderMutex.unlock();
  if (reportsBatch.empty())
    return;

  logger.Log(LogType::DEBUG,
             std::string("Popped " + std::to_string(reportsBatch.size()) +
                         " Reports for sending"));

  // Convert the reportsBatch into reports packet
  std::string reportsPacket;
  reportsPacket += "{ \n";
  reportsPacket += "	\"privateKey\": \"" + packetPrivateKey + "\",\n";
  reportsPacket += "	\"titleId\": " + std::to_string(packetTitleId) + ",\n";
  reportsPacket += +"\"reports\":[";
  for (auto& report : reportsBatch) {
    reportsPacket += report->ToString(true) + ",";  // report is outside match
    delete report;
  }
  reportsPacket.pop_back();  // pop the last delimiter
  reportsPacket += +"]}";

  // Send new reports to Getgud
  SendReportPacket(reportsPacket);
}

/**
 * SendReportPacket:
 *
 * Send packet to getgud
 **/
void ReportSender::SendReportPacket(std::string& packet) {
  if (!m_curl || packet.empty()) {
    return;
  }
  m_curlReadBuffer.clear();
  curl_easy_setopt(m_curl, CURLOPT_URL, sdkConfig.sendReportsURL.c_str());
  curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, packet.c_str());

  // time when we should stop trying to send packet
  auto stopWaitingTime =
      std::chrono::system_clock::now() +
      std::chrono::milliseconds(sdkConfig.apiWaitTimeMilliseconds);

  CURLcode sendCode = CURLE_UNKNOWN_OPTION;

  while (sendCode != CURLE_OK &&
         stopWaitingTime > std::chrono::system_clock::now()) {
    // send prepared packet
    sendCode = curl_easy_perform(m_curl);

    // make small delay in order to save hardware usage
    std::this_thread::sleep_for(std::chrono::microseconds(50));
  }

  if (sendCode != CURLcode::CURLE_OK) {
    if (m_curlReadBuffer.find("\"ErrorType\"") != std::string::npos) {
      logger.Log(LogType::DEBUG,
        "GameSender::SendUpdatePlayerPacket->Failed to send throttle request: " +
        m_curlReadBuffer);
    }
    logger.Log(LogType::_ERROR, "Failed to send packet: " +
                                    std::string(curl_easy_strerror(sendCode)));
  }
  else
  {
    if (m_curlReadBuffer.find("\"ErrorType\"") != std::string::npos) {
      logger.Log(LogType::DEBUG,
        "GameSender::SendUpdatePlayerPacket->Failed to send throttle request: " +
        m_curlReadBuffer);
    }
  }
}

/**
 * InitCurl:
 *
 * Prepare CURL for sending requests
 **/
void ReportSender::InitCurl() {
  logger.Log(LogType::DEBUG, "REST Api connection prepared in ReportSender");

  m_curl = curl_easy_init();

  if (!m_curl) {
    logger.Log(LogType::WARN, "ReportSender->InitCurl Couldn't init curl");
  }
  m_headers = curl_slist_append(m_headers, "Accept: application/json");
  m_headers = curl_slist_append(m_headers, "Content-Type: application/json");
  m_headers = curl_slist_append(m_headers, "charset: utf-8");

  curl_easy_setopt(m_curl, CURLOPT_POST, 1);
  curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);
  curl_easy_setopt(m_curl, CURLOPT_URL, sdkConfig.sendReportsURL.c_str());
  curl_easy_setopt(m_curl, CURLOPT_TIMEOUT_MS,
                   sdkConfig.apiTimeoutMilliseconds);
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, CURLWriteCallback);
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_curlReadBuffer);
}

/**
 * ~ReportSender:
 *
 **/
ReportSender::~ReportSender() {
  if (m_curl) {
    curl_easy_cleanup(m_curl);
  }
  curl_slist_free_all(m_headers);
  logger.Log(LogType::DEBUG, "Report Sender stopped..");
}

/**
 * Dispose:
 *
 **/
void ReportSender::Dispose() {
  m_threadWorking = false;
  m_reportSenderMutex.lock();
  // Iterate through all the actions in the buffer and delete them
  for (auto* reportData : m_reportVector) {
    delete reportData;
  }

  m_reportVector.clear();
  m_reportBufferSize = 0;

  m_reportSenderMutex.unlock();
  m_updaterThread.detach();

  {
    std::lock_guard<std::mutex> locker(sharedReportSenders.reportSendersMutex);
    sharedReportSenders.reportSendersCount--;
  }
}

}  // namespace GetGudSdk