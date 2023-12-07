//
// Created by rov on 12/5/2023.
//

#include <duk_import/resource/resource_importer.h>

namespace duk::import {

ResourceImporter::~ResourceImporter() = default;

bool operator==(const ResourceDescription& lhs, const ResourceDescription& rhs) {
    return lhs.id == rhs.id;
}

bool operator<(const ResourceDescription& lhs, const ResourceDescription& rhs) {
    return lhs.id < rhs.id;
}

}

