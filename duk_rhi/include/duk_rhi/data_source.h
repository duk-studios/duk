//
// Created by Ricardo on 07/04/2023.
//

#ifndef DUK_RHI_DATA_SOURCE_H
#define DUK_RHI_DATA_SOURCE_H

#include <duk_hash/hash.h>
#include <duk_macros/macros.h>

namespace duk::rhi {

class DataSource {
public:

    DataSource();

    virtual ~DataSource();

    void update_hash();

    DUK_NO_DISCARD hash::Hash hash() const;

protected:

    DUK_NO_DISCARD virtual hash::Hash calculate_hash() const = 0;

private:
    hash::Hash m_hash;
};

}

#endif //DUK_RHI_DATA_SOURCE_H
