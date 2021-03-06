
#include <stack>
#include "Diag.hpp"
// inital Logging Level


spdlog::level::level_enum Diag::_nLogLevel = LOGV_ERROR;    // default loglevel
bool Diag::fileInfo = true;
bool Diag::println = true;
bool Diag::printLabel = true;
std::stack<DiagConfig *> Diag::config;

const std::map<std::string, DiagLevel> Diag::diagMapStr
{
    {"silent", LOGV_SILENT}, 
    {"info", LOGV_INFO},
    {"warnings", LOGV_WARN},      
    {"error", LOGV_ERROR},
    {"trace", LOGV_TRACE},        
    {"debug", LOGV_DEBUG}
};

const std::map<DiagLevel, std::string> Diag::diagMap
{
    {LOGV_SILENT, "silent"}, 
    {LOGV_WARN, "warnings"},
    {LOGV_ERROR, "error"},
    {LOGV_INFO, "info"},
    {LOGV_TRACE, "trace"}, 
    {LOGV_DEBUG, "debug"}
};


const std::string *Diag::getDiagLevelName(DiagLevel level) {
    if (diagMap.find(level) == diagMap.end() ) {
        ERR("No such diagnostic level value {}", level);
    }
    return &diagMap.find(level)->second;
}

const DiagLevel Diag::getDiagLevel(std::string level) {
    if (diagMapStr.find(level) == diagMapStr.end() ) {
        WARN("No such diagnostic level name {}", level);
        WARN("Returning default value: info");
    }
    return diagMapStr.find(level)->second;
}

void Diag::push() {

    auto *dc = new DiagConfig();

    dc->println = println;
    dc->_nLogLevel = _nLogLevel;
    dc->fileInfo = fileInfo;
    dc->printLabel = printLabel;

    config.push(dc);

}

void Diag::pop() {
    
    DiagConfig *dc = config.top();
    
    println = dc->println;
    // _nLogLevel = dc->_nLogLevel;
    Diag::setLogLevel(dc->_nLogLevel);
    fileInfo = dc->fileInfo;
    printLabel = dc->printLabel;
    config.pop();
    delete dc;

}