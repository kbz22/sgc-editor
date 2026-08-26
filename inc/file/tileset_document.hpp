#pragma once

#include "file/itreeviewlistable.hpp"
#include <sgc/data/asset.hpp>

namespace file {

    class TilesetDocument : public ITreeViewListable{

        private:
            std::wstring name;
            sgc::data::AssetId m_tilesetId{0};
            sgc::data::AssetId m_imageId{0};

        public:
            TilesetDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId imageId);
            virtual ~TilesetDocument() = default;            

            const std::wstring& GetName() const override;
            bool IsContainer() const override;
            bool IsDirty() const override;
            bool IsActivable() const override;

            ListableType GetListableType() const override;

            sgc::data::AssetId GetTilesetAssetId() const;
            sgc::data::AssetId GetImageAssetId() const;
            
    };

}