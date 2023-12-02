/// 13/11/2023
/// importer.h

#ifndef DUK_IMPORT_IMPORTER_H
#define DUK_IMPORT_IMPORTER_H

#include <duk_import/image/image_importer.h>
#include <duk_tools/singleton.h>

namespace duk::import {

class Importer {
public:

    Importer();

    ~Importer();

    std::unique_ptr<duk::rhi::ImageDataSource> load_image(const std::filesystem::path& path, duk::rhi::PixelFormat desiredPixelFormat);

private:
    std::vector<std::unique_ptr<ImageImporter>> m_imageImporters;
};

}

#endif // DUK_IMPORT_IMPORTER_H

