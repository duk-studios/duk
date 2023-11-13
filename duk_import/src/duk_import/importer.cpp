/// 13/11/2023
/// importer.cpp

#include <duk_import/importer.h>
#include <duk_import/image/image_importer_stb.h>

namespace duk::import {

Importer::Importer() {
    m_imageImporters.emplace_back(std::make_unique<ImageImporterStb>());
}

std::unique_ptr<duk::rhi::ImageDataSource> Importer::load_image(const std::filesystem::path& path, duk::rhi::PixelFormat desiredFormat) {
    for (auto& importer : m_imageImporters) {
        if (importer->accepts(path, desiredFormat)) {
            return importer->load(path, desiredFormat);
        }
    }
    throw std::runtime_error("failed to load image: could not find suitable ImageImporter");
}

}