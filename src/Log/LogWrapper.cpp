﻿#include <cstdio>
#include <cstring>
#include <stdarg.h>
#include "Log/LogWrapper.h"

LogWrapper::LogWrapper() : log_level_(level_t::LOG_LW_DEBUG) {
    memset(log_buffer_, 0, sizeof(log_buffer_));
    auto_update_time_ = true;
    update();

    enable_print_file_location_ = true;
    enable_print_function_name_ = true;
    enable_print_log_type_ = true;
    enable_print_time_ = "[%Y-%m-%d %H:%M:%S]";
}

LogWrapper::~LogWrapper() {}

int32_t LogWrapper::init(level_t level) {
    log_level_ = level;

    return 0;
}

void LogWrapper::addLogHandle(log_handler_t h, level_t level_min, level_t level_max) {
    if (h) {
        log_router_t router;
        router.handle = h;
        router.level_min = level_min;
        router.level_max = level_max;
        log_handlers_.push_back(router);
    };
}

void LogWrapper::update() {
    log_time_cache_sec_ = time(nullptr);
    log_time_cache_sec_p_ = localtime(&log_time_cache_sec_);
}

void LogWrapper::log(level_t level_id, const char* level, const char* file_path, uint32_t line_number, const char* func_name, const char* fnt, ...) {
    if (auto_update_time_ && !enable_print_time_.empty()) {
        update();
    }

    if (!log_handlers_.empty()) {
        // format => "[Log    DEBUG][2015-01-12 10:09:08.]
        int start_index = 0;

        if (enable_print_log_type_ && nullptr != level) {
            start_index = sprintf(log_buffer_, "[Log %8s]", level);
            if (start_index < 0) {
                start_index = 14;
            }
        }

        // 是否需要毫秒级？std::chrono
        if (!enable_print_time_.empty()) {
            start_index += strftime(&log_buffer_[start_index], sizeof(log_buffer_) - start_index, enable_print_time_.c_str(), log_time_cache_sec_p_);
        }

        // 打印位置选项
        if (enable_print_file_location_ && enable_print_function_name_ && 
            nullptr != file_path && nullptr != func_name) {
            int res = sprintf(&log_buffer_[start_index], "[%s:%u(%s)]: ", file_path, line_number, func_name);
            start_index += res >= 0 ? res : 0;
        } else if (enable_print_file_location_ && nullptr != file_path) {
            int res = sprintf(&log_buffer_[start_index], "[%s:%u]: ", file_path, line_number);
            start_index += res >= 0 ? res : 0;
        } else if (enable_print_function_name_ && nullptr != func_name) {
            int res = sprintf(&log_buffer_[start_index], "[(%s)]: ", func_name);
            start_index += res >= 0 ? res : 0;
        }


        va_list va_args;
        va_start(va_args, fnt);

        start_index += vsnprintf(&log_buffer_[start_index], sizeof(log_buffer_) - start_index, fnt, va_args);

        va_end(va_args);

        log_buffer_[sizeof(log_buffer_) - 1] = 0;
        if (static_cast<size_t>(start_index) < sizeof(log_buffer_)) {
            log_buffer_[start_index] = 0;
        }

        for (std::list<log_router_t>::iterator iter = log_handlers_.begin(); iter != log_handlers_.end(); ++iter) {
            if (level_id >= iter->level_min && level_id <= iter->level_max) {
                iter->handle(level_id, level, log_buffer_);
            }
        }
    }
}

