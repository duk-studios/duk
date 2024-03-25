//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_IMPORT_AUDIO_IMPORTER_H
#define DUK_IMPORT_AUDIO_IMPORTER_H

#include <duk_import/resource_importer.h>

#include <duk_audio/audio_clip_pool.h>

namespace duk::import {

struct AudioClipImporterCreateInfo {
    duk::audio::AudioClipPool* audioClipPool;
};

class AudioClipImporter : public ResourceImporter {
public:
    explicit AudioClipImporter(const AudioClipImporterCreateInfo& audioClipImporterCreateInfo);

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

private:
    duk::audio::AudioClipPool* m_audioClipPool;
};

}// namespace duk::import

#endif//DUK_IMPORT_AUDIO_IMPORTER_H
