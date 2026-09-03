#include "file/file_manager.hpp"
#include "program/program.hpp"
#include "program/except.hpp"
#include "file/map_file.hpp"
#include "file/tileset_file.hpp"
#include "file/package_file.hpp"
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
    newFile->m_document = std::make_unique<MapDocument>(
        NewMapDocument(name, tilesetId)
    );

    DocumentLocation selectedDoc{ newFile.get(), 0 };    

    m_openFiles.push_back(std::move(newFile));
    SetActiveDocument(selectedDoc);

    m_onFileUpdatedCallback(nullptr);

    return;
}

file::MapDocument file::FileManager::NewMapDocument(std::wstring name, sgc::data::AssetId tilesetId)
{
    auto newDocument = MapDocument(name, tilesetId);

    newDocument.RegisterOnSetDirtyCallback([this](bool dirty) {

        if(!dirty) {
            return;
        }

        auto &programContext = program::GetProgramContext();
        programContext.packageSection->UpdateTreeViewItems(programContext);
        programContext.packageSection->Update();
    });

    return newDocument;
}

void file::FileManager::NewTilesetFile(std::wstring name, std::filesystem::path filePath, size_t tileWidth, size_t tileHeight, AssetManager &assetManager)
{
    auto newFile = std::make_unique<TilesetFile>(assetManager);
    newFile->m_tilesetDocument = std::make_unique<TilesetDocument>(
        NewTilesetDocument(name, filePath, tileWidth, tileHeight)
    );

    m_openFiles.push_back(std::move(newFile));

    m_onFileUpdatedCallback(nullptr);

    return;
}

file::TilesetDocument file::FileManager::NewTilesetDocument(std::wstring name, std::filesystem::path filePath, size_t tileWidth, size_t tileHeight)
{
    if(tileWidth <= 0 || tileHeight <= 0) {
        throw program::TileSizeException("Tile size must be greater than zero.");
    }

    auto imageId = LoadImageAsset(name, filePath);
    auto tilesetId = LoadTilesetAsset(name, imageId, static_cast<int>(tileWidth), static_cast<int>(tileHeight));

    return {name, tilesetId, imageId};
}

void file::FileManager::NewPackageFile(std::wstring name, std::vector<MapDocument*> mapDocuments, std::vector<TilesetDocument*> tilesetDocuments, AssetManager &assetManager)
{
    auto newFile = std::make_unique<file::PackageFile>(name, assetManager);

    for(auto mapDoc : mapDocuments) {
        newFile->AddMapDocument(std::unique_ptr<MapDocument>(mapDoc));
    }
    for(auto tilesetDoc : tilesetDocuments) {
        newFile->AddTilesetDocument(std::unique_ptr<TilesetDocument>(tilesetDoc));
    }

    m_openFiles.push_back(std::move(newFile));

    m_onFileUpdatedCallback(nullptr);

    return;
}

void file::FileManager::OpenFile(std::filesystem::path filePath, AssetManager *assetManager)
{
    auto extension = filePath.extension().wstring();
    DocumentLocation openedDoc{ nullptr, 0 };

    if(extension == defaults::MapFileExtension.data())
    {
        auto newFile = std::make_unique<MapFile>();
        auto newFilePtr = newFile.get();
        newFile->SetFilePath(filePath);
        newFile->Open();

        newFile->m_document->RegisterOnSetDirtyCallback([this, newFilePtr](bool dirty) {
            m_onMapDirtyCallback(newFilePtr->m_document.get(), dirty);
        });

        m_openFiles.push_back(std::move(newFile));

        openedDoc.file = m_openFiles.back().get();        
        SetActiveDocument(openedDoc);
        m_onFileUpdatedCallback(newFile.get());
    }
    else if(extension == defaults::TilesetFileExtension.data()) 
    {
        if(assetManager == nullptr) {
            throw program::AssetLoadException("AssetManager is null. Cannot load tileset file.");
        }

        auto newFile = std::make_unique<TilesetFile>(*assetManager);
        newFile->SetFilePath(filePath);
        newFile->Open();
        m_openFiles.push_back(std::move(newFile));
        m_onFileUpdatedCallback(newFile.get());
    }
    else if(extension == defaults::PackageFileExtension.data())
    {
        if(assetManager == nullptr) {
            throw program::AssetLoadException("AssetManager is null. Cannot load package file.");
        }

        auto newFile = std::make_unique<PackageFile>(filePath.stem().wstring(), *assetManager);
        newFile->SetFilePath(filePath);
        newFile->Open();
        m_openFiles.push_back(std::move(newFile));
        m_onFileUpdatedCallback(m_openFiles.back().get());
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
        m_onFileUpdatedCallback(nullptr);
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

        if(m_activeDocument.file == m_openFiles[index].get()) {
            m_activeDocument.file = nullptr;
            m_activeDocument.index = 0;
        }
        
        m_openFiles.erase(m_openFiles.begin() + index);

        m_onFileUpdatedCallback(nullptr);
    }

    return;
}

void file::FileManager::SelectDocument(const DocumentLocation &document)
{
    auto clearSelection = [&]() {
        m_selectedDocument.file = nullptr;
        m_selectedDocument.index = 0;
    };

    if(!document.file) 
    {
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

    return;
}

void file::FileManager::SetActiveDocument(const DocumentLocation &document)
{
    auto clearActive = [&]() {
        m_activeDocument.file = nullptr;
        m_activeDocument.index = 0;
    };

    if(!document.file) 
    {
        clearActive();
    }
    else
    {
        auto doc = document.file->GetMapDocument(document.index);

        if(!doc) 
        {
            clearActive();
        }
        else
        {
            m_activeDocument.file = document.file;
            m_activeDocument.index = document.index;
        }
    }

    m_onActiveDocumentChangedCallback(m_activeDocument);
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

std::vector<file::PackageFile*> file::FileManager::GetAllPackages() const
{
    std::vector<PackageFile*> allPackages{};

    for (const auto& file : m_openFiles) {
        if(file->GetFileType() == FileType::Package) {
            allPackages.push_back(static_cast<PackageFile*>(file.get()));
        }        
    }

    return allPackages;
}

file::DocumentLocation file::FileManager::GetSelectedDocumentLocation() const
{
    return m_selectedDocument;
}

file::DocumentLocation file::FileManager::GetActiveDocumentLocation() const
{
    return m_activeDocument;
}

void file::FileManager::RegisterOnActiveDocumentChangedCallback(std::function<void(DocumentLocation)> callback)
{
    m_onActiveDocumentChangedCallback = callback;
}

void file::FileManager::RegisterOnFileUpdatedCallback(std::function<void(IFile*)> callback)
{
    m_onFileUpdatedCallback = callback;
}

void file::FileManager::RegisterOnMapDirtyCallback(std::function<void(MapDocument*, bool)> callback)
{
    m_onMapDirtyCallback = callback;
}