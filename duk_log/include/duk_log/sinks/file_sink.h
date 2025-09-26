//
// Created by Ricardo on 29/06/2024.
//

#ifndef DUK_LOG_FILE_SINK_H
#define DUK_LOG_FILE_SINK_H

#include <duk_log/sink.h>

#include <filesystem>
#include <fstream>

namespace duk::log {

class FileSink : public Sink {
public:
    FileSink(const std::filesystem::path& filename, const std::string& name, Level level);

    void flush(Level level, const std::string& message) override;

private:
    std::mutex m_mutex;
    std::ofstream m_file;
};

}// namespace duk::log

#endif//DUK_LOG_FILE_SINK_H
