#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <glog/logging.h>

#define LOG_TRACE LOG(INFO)
#define LOG_DEBUG LOG(INFO)
#define LOG_INFO  LOG(INFO)
// detail logging
#define DLOG_TRACE LOG(INFO) << __PRETTY_FUNCTION__ << " this=" << this << " "

#define LOG_ERROR LOG(ERROR)
#define LOG_FATAL LOG(FATAL)
#define LOG_WARN LOG(WARNING)
// detail warnings
#define DLOG_WARN LOG(WARNING) << __PRETTY_FUNCTION__ << " this=" << this << " "

#endif 
