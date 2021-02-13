
#include <stack>
#include "Diag.hpp"
// inital Logging Level


DiagLevel Diag::_nLogLevel = DiagLevel::LOGV_INFO;    // default loglevel
int Diag::_nInfoLevel = 0;
bool Diag::fileInfo = true;
bool Diag::println = true;
bool Diag::printLabel = true;
std::stack<DiagConfig *> Diag::config;

const std::map<std::string, DiagLevel> Diag::diagMap{
    {"silent", DiagLevel::LOGV_SILENT}, {"info", DiagLevel::LOGV_INFO},
    {"warnings", DiagLevel::LOGV_WARN},      {"error", DiagLevel::LOGV_ERROR},
    {"trace", DiagLevel::LOGV_TRACE},        {"debug", DiagLevel::LOGV_DEBUG}};


void Diag::push() {

    auto *dc = new DiagConfig();

    dc->println = println;
    dc->_nLogLevel = _nLogLevel;
    dc->_nInfoLevel = _nInfoLevel;
    dc->fileInfo = fileInfo;
    dc->printLabel = printLabel;

    config.push(dc);

}

void Diag::pop() {
    
    DiagConfig *dc = config.top();
    
    println = dc->println;
    _nLogLevel = dc->_nLogLevel;
    _nInfoLevel = dc->_nInfoLevel;
    fileInfo = dc->fileInfo;
    printLabel = dc->printLabel;
    config.pop();
    delete dc;

}