#include "file/file_manager.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "file/map_file.hpp"
#include <sgc/data/helpers.hpp>

sgc::data::AssetId LoadImageAsset(const std::filesystem::path& path)
{
    auto &programContext = program::GetProgramContext();
    auto &assetManager = programContext.assetManager;

    auto imageData = sgc::data::ReadFile(path);
    auto imageId = sgc::data::HashAsset(path.string()); 
    
    if(imageData.empty()) {
        throw program::AssetLoadException("Failed to load image asset: " + path.string());
    }

    assetManager->AddAsset<sgc::asset::ImageAsset>(imageId, std::make_shared<sgc::asset::ImageAsset>(imageData));

    return imageId;
}

sgc::data::AssetId LoadTilesetAsset(sgc::data::AssetId imageId, int tileWidth, int tileHeight)
{
    auto &programContext = program::GetProgramContext();
    auto &assetManager = programContext.assetManager;

    auto tilesetAsset = sgc::asset::TilesetAsset{
        imageId,
        static_cast<sgc::math::ival>(tileWidth),
        static_cast<sgc::math::ival>(tileHeight)
    };

    auto tilesetId = sgc::data::HashAsset(std::to_string(imageId));
    assetManager->AddAsset<sgc::asset::TilesetAsset>(tilesetId, std::make_shared<sgc::asset::TilesetAsset>(tilesetAsset));

    return tilesetId;
}

void file::FileManager::NewMapFile(std::filesystem::path filePath, size_t tileWidth, size_t tileHeight)
{
    if(tileWidth <= 0 || tileHeight <= 0) {
        throw program::TileSizeException("Tile size must be greater than zero.");
    }

    auto imageId = LoadImageAsset(filePath);
    auto tilesetId = LoadTilesetAsset(imageId, static_cast<int>(tileWidth), static_cast<int>(tileHeight));

    auto newFile = std::make_unique<MapFile>();
    newFile->m_document = std::make_unique<MapDocument>(tilesetId);    

    m_openFiles.push_back(std::move(newFile));
    SelectDocument(m_openFiles.size() - 1);

    return;
}

void file::FileManager::OpenFile(std::filesystem::path filePath)
{
    //!
    return;
}

void file::FileManager::SaveFile(size_t index)
{
    
    return;
}

void file::FileManager::CloseFile(size_t index)
{
    if(!m_openFiles.empty() && index < m_openFiles.size()) {
        m_openFiles.erase(m_openFiles.begin() + index);

        SelectDocument(m_openFiles.empty() ? 0 : m_openFiles.size() - 1);
    }

    return;
}

void file::FileManager::SelectDocument(size_t index)
{
    if (index < m_openFiles.size()) {
        m_selectedDocument.file = m_openFiles[index].get();
        m_selectedDocument.index = index;
    }
    else if (m_openFiles.empty()) {
        m_selectedDocument.file = nullptr;
        m_selectedDocument.index = 0;
    }
}

file::MapDocument* file::FileManager::GetSelectedDocument() const
{
    if(m_selectedDocument.file != nullptr) 
    {
        auto docs = m_selectedDocument.file->GetMapDocuments();

        if(!docs.empty()) {
            return docs[m_selectedDocument.index];
        }        
    }    

    return nullptr;
}

file::IFile* file::FileManager::GetSelectedFile() const
{
    return m_selectedDocument.file;
}

size_t file::FileManager::GetSelectedFileIndex() const
{
    return m_selectedDocument.index;
}