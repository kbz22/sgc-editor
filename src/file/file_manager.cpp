#include "file/file_manager.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "file/map_file.hpp"
#include "file/tileset_file.hpp"
#include <sgc/data/helpers.hpp>

sgc::data::AssetId LoadImageAsset(std::wstring name, const std::filesystem::path& path)
{
    auto &programContext = program::GetProgramContext();
    auto &assetManager = programContext.assetManager;
    auto hashableName = L"image/" + name;

    auto imageData = sgc::data::ReadFile(path);
    auto imageId = sgc::data::HashAsset(hashableName);
    
    if(imageData.empty()) {
        throw program::AssetLoadException("Failed to load image asset: " + path.string());
    }
    
    assetManager->AddAsset<sgc::asset::ImageAsset>(imageId, std::make_shared<sgc::asset::ImageAsset>(imageData));

    return imageId;
}

sgc::data::AssetId LoadTilesetAsset(std::wstring name, sgc::data::AssetId imageId, int tileWidth, int tileHeight)
{
    auto &programContext = program::GetProgramContext();
    auto &assetManager = programContext.assetManager;
    auto hashableName = L"tileset/" + name;

    auto tilesetAsset = sgc::asset::TilesetAsset{
        imageId,
        static_cast<sgc::math::ival>(tileWidth),
        static_cast<sgc::math::ival>(tileHeight)
    };

    auto tilesetId = sgc::data::HashAsset(hashableName);
    assetManager->AddAsset<sgc::asset::TilesetAsset>(tilesetId, std::make_shared<sgc::asset::TilesetAsset>(tilesetAsset));

    return tilesetId;
}

void file::FileManager::NewMapFile(std::wstring name, sgc::data::AssetId tilesetId)
{
    auto newFile = std::make_unique<MapFile>();
    newFile->m_document = std::make_unique<MapDocument>(name, tilesetId);

    DocumentLocation selectedDoc{ newFile.get(), 0 };

    newFile->m_document->RegisterOnSetDirtyCallback([this](bool dirty) {

        if(!dirty) {
            return;
        }

        auto &programContext = program::GetProgramContext();
        programContext.packageSection->UpdateTreeViewItems(programContext);
        programContext.packageSection->Update();
    });

    m_openFiles.push_back(std::move(newFile));
    // SelectDocument(selectedDoc);
    SetActiveDocument(selectedDoc);

    return;
}

void file::FileManager::NewTilesetFile(std::wstring name, std::filesystem::path filePath, size_t tileWidth, size_t tileHeight, AssetManager &assetManager)
{
    if(tileWidth <= 0 || tileHeight <= 0) {
        throw program::TileSizeException("Tile size must be greater than zero.");
    }

    auto imageId = LoadImageAsset(name, filePath);
    auto tilesetId = LoadTilesetAsset(name, imageId, static_cast<int>(tileWidth), static_cast<int>(tileHeight));

    auto newFile = std::make_unique<TilesetFile>(assetManager);
    newFile->m_tilesetDocument = std::make_unique<TilesetDocument>(name, tilesetId, imageId);

    m_openFiles.push_back(std::move(newFile));    

    return;
}

void file::FileManager::OpenFile(std::filesystem::path filePath)
{
    auto extension = filePath.extension().wstring();
    DocumentLocation openedDoc{ nullptr, 0 };

    if(extension == defaults::MapFileExtension.data())
    {
        auto newFile = std::make_unique<MapFile>();
        newFile->SetFilePath(filePath);
        newFile->Open();

        newFile->m_document->RegisterOnSetDirtyCallback([this](bool dirty) {

            if(!dirty) {
                return;
            }

            auto &programContext = program::GetProgramContext();
            programContext.packageSection->UpdateTreeViewItems(programContext);
            programContext.packageSection->Update();
        });

        m_openFiles.push_back(std::move(newFile));

        openedDoc.file = m_openFiles.back().get();
        // SelectDocument(openedDoc);
        SetActiveDocument(openedDoc);
    }
    else if(extension == defaults::TilesetFileExtension.data()) 
    {
        auto &programContext = program::GetProgramContext();
        auto &assetManager = programContext.assetManager;

        auto newFile = std::make_unique<TilesetFile>(*assetManager);
        newFile->SetFilePath(filePath);
        newFile->Open();
        m_openFiles.push_back(std::move(newFile));
    }
    else
    {
        auto errorMsg = "Unsupported file extension: " + filePath.extension().string();
        throw program::AssetLoadException(errorMsg);
    }

    return;
}

void file::FileManager::SaveFile(size_t index)
{
    if(index < m_openFiles.size()) {
        m_openFiles[index]->Save();
    }
    return;
}

void file::FileManager::CloseFile(size_t index)
{
    if(!m_openFiles.empty() && index < m_openFiles.size()) {
        
        if(m_selectedDocument.file == m_openFiles[index].get()) {
            m_selectedDocument.file = nullptr;
            m_selectedDocument.index = 0;
        }
        
        m_openFiles.erase(m_openFiles.begin() + index);        
    }

    return;
}

void file::FileManager::SelectDocument(const DocumentLocation &document)
{
    auto clearSelection = [&]() {
        m_selectedDocument.file = nullptr;
        m_selectedDocument.index = 0;
    };

    if(!document.file) {
        clearSelection();
        return;
    }

    auto mapDoc = document.file->GetMapDocument(document.index);
    auto tilesetDoc = document.file->GetTilesetDocument(document.index);

    if(mapDoc) {
        m_selectedDocument.file = document.file;
        m_selectedDocument.index = document.index;
    }
    else if(tilesetDoc) {
        m_selectedDocument.file = document.file;
        m_selectedDocument.index = document.index;
    }
    else {
        clearSelection();
    }
}

void file::FileManager::SetActiveDocument(const DocumentLocation &document)
{
    auto clearActive = [&]() {
        m_activeDocument.file = nullptr;
        m_activeDocument.index = 0;
    };

    if(!document.file) {
        clearActive();
        return;
    }

    auto doc = document.file->GetMapDocument(document.index);

    if(!doc) {
        clearActive();
        return;
    }

    m_activeDocument.file = document.file;
    m_activeDocument.index = document.index;
}

file::MapDocument* file::FileManager::GetActiveDocument() const
{
    if(m_activeDocument.file != nullptr) 
    {
        auto docs = m_activeDocument.file->GetMapDocuments();

        if(!docs.empty()) {
            return docs[m_activeDocument.index];
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
    for(size_t i = 0; i < m_openFiles.size(); ++i) {
        if(m_openFiles[i].get() == m_selectedDocument.file) {
            return i;
        }
    }

    return 0;
}

size_t file::FileManager::GetFileIndex(IFile* file) const
{
    for(size_t i = 0; i < m_openFiles.size(); ++i) {
        if(m_openFiles[i].get() == file) {
            return i;
        }
    }

    return 0;
}

std::vector<file::IFile*> file::FileManager::GetOpenFiles() const
{
    std::vector<IFile*> openFiles{};
    for(const auto& file : m_openFiles) {
        openFiles.push_back(file.get());
    }
    return openFiles;
}

std::vector<file::TilesetDocument*> file::FileManager::GetAllTilesetDocuments() const
{
    std::vector<TilesetDocument*> allTilesetDocs{};

    for (const auto& file : m_openFiles) {
        auto tilesetDocs = file->GetTilesetDocuments();
        allTilesetDocs.insert(allTilesetDocs.end(), tilesetDocs.begin(), tilesetDocs.end());
    }

    return allTilesetDocs;
}