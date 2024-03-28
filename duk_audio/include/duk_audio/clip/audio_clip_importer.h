//
// Created by Ricardo on 25/03/2024.
//

#ifndef DUK_AUDIO_AUDIO_CLIP_IMPORTER_H
#define DUK_AUDIO_AUDIO_CLIP_IMPORTER_H

#include <duk_resource/importer.h>

#include <duk_audio/clip/audio_clip_pool.h>

namespace duk::audio {

struct AudioClipImporterCreateInfo {
    AudioClipPool* audioClipPool;
};

class AudioClipImporter : public duk::resource::ResourceImporter {
public:
    explicit AudioClipImporter(const AudioClipImporterCreateInfo& audioClipImporterCreateInfo);

    const std::string& tag() const override;

    void load(const duk::resource::Id& id, const std::filesystem::path& path) override;

private:
    AudioClipPool* m_audioClipPool;
};

}// namespace duk::audio

#endif//DUK_AUDIO_AUDIO_CLIP_IMPORTER_H
