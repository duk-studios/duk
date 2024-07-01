//
// Created by Ricardo on 30/06/2024.
//

#include <duk_log/sink_file.h>

namespace duk::log {

SinkFile::SinkFile(const std::filesystem::path& filename, Level level)
    : Sink(level) {
    m_file.open(filename);
}

void SinkFile::flush(Level level, const std::string& message) {
    if (!m_file.is_open()) {
        return;
    }
    std::lock_guard lock(m_mutex);
    m_file << message << std::endl;
    m_file.flush();
}

}
