#pragma once

#include "file/itreeviewlistable.hpp"
#include "file/idocument.hpp"
#include <sgc/data/asset.hpp>
#include <sgc/asset/metadataasset.hpp>

namespace file 
{
    using sgc::asset::AssetMetadata;

    class TilesetDocument : public IDocument
    {
        private:
            std::wstring m_name;
            sgc::data::AssetId m_tilesetId{0};
            sgc::data::AssetId m_imageId{0};
            sgc::data::AssetId m_tilesetDocumentId{0};
            sgc::data::AssetId m_metadataAssetId{0};

        public:
            TilesetDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId imageId);
            TilesetDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId imageId, sgc::data::AssetId metadataAssetId);
            virtual ~TilesetDocument() = default;            

            const std::wstring& GetName() const override;
            bool IsContainer() const override;
            bool IsDirty() const override;
            bool IsActivable() const override;

            ListableType GetListableType() const override;

            sgc::data::AssetId GetTilesetAssetId() const;
            sgc::data::AssetId GetImageAssetId() const;
            sgc::data::AssetId GetMetadataAssetId() const;
            sgc::data::AssetId GetTilesetDocumentAssetId() const;

            AssetMetadata GenerateMetadataAsset() const;
    };

}