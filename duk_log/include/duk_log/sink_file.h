//
// Created by Ricardo on 29/06/2024.
//

#ifndef DUK_LOG_SINK_FILE_H
#define DUK_LOG_SINK_FILE_H

#include <duk_log/sink.h>

#include <filesystem>
#include <fstream>

namespace duk::log {

class SinkFile : public Sink {
public:
    SinkFile(const std::filesystem::path& filename, Level level);

    void flush(Level level, const std::string& message) override;

private:
    std::mutex m_mutex;
    std::ofstream m_file;
};

}

#endif //DUK_LOG_SINK_FILE_H
