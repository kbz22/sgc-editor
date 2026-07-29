#pragma once

#include <sgc/data/asset.hpp>

namespace file {

    class TilesetDocument {

        private:
            std::wstring name;
            sgc::data::AssetId m_tilesetId{0};
            sgc::data::AssetId m_imageId{0};

        public:
            TilesetDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId imageId);
            virtual ~TilesetDocument() = default;

            sgc::data::AssetId GetTilesetAssetId() const;
            const std::wstring& GetName() const;
            sgc::data::AssetId GetImageAssetId() const;

    };

}