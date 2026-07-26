#pragma once

#include <sgc/data/asset.hpp>

namespace file {

    class TilesetDocument {

        private:
            sgc::data::AssetId m_tilesetId{0};

        public:
            TilesetDocument(sgc::data::AssetId tilesetId);
            virtual ~TilesetDocument() = default;

            sgc::data::AssetId GetTilesetAssetId() const;

    };

}