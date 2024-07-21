//
// Created by Ricardo on 30/06/2024.
//

#include <duk_log/file_sink.h>

namespace duk::log {

FileSink::FileSink(const std::filesystem::path& filename, const std::string& name, Level level)
    : Sink(name, level) {
    m_file.open(filename);
}

void FileSink::flush(Level level, const std::string& message) {
    if (!m_file.is_open()) {
        return;
    }
    std::lock_guard lock(m_mutex);
    m_file << message << std::endl;
    m_file.flush();
}

}// namespace duk::log
