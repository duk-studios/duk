/// 12/11/2023
/// image_importer_stb.h

#ifndef DUK_IMPORT_IMAGE_IMPORTER_STB_H
#define DUK_IMPORT_IMAGE_IMPORTER_STB_H

#include <duk_import/image/image_importer.h>

namespace duk::import {

class ImageImporterStb : public ImageImporter {
public:

    bool accepts(const std::filesystem::path& path) override;

    std::unique_ptr<duk::rhi::ImageDataSource> load(const std::filesystem::path& path) override;

};

}

#endif // DUK_IMPORT_IMAGE_IMPORTER_STB_H

