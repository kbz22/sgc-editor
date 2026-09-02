#pragma once

#include <sgc/asset/runtimebuilder.hpp>
#include <sgc/asset/mapasset.hpp>
#include "file/map_document.hpp"
#include "file/document_info.hpp"

namespace sgc::asset {

    template<>
    struct RuntimeBuilder<file::MapDocument> 
    {
        static std::unique_ptr<file::MapDocument> Build(const file::MapDocumentInfo& info, const sgc::asset::MapAsset& asset);
    };

}