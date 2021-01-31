
#include "Diag.hpp"
// inital Logging Level


DiagLevel Diag::_nLogLevel = DiagLevel::LOGV_DEBUG;
bool Diag::fileInfo = true;
bool Diag::println = true;

const std::map<std::string, DiagLevel> Diag::diagMap{
    {"silent", DiagLevel::LOGV_SILENT}, {"info", DiagLevel::LOGV_INFO},
    {"warnings", DiagLevel::LOGV_WARN},      {"error", DiagLevel::LOGV_ERROR},
    {"trace", DiagLevel::LOGV_TRACE},        {"debug", DiagLevel::LOGV_DEBUG}};
