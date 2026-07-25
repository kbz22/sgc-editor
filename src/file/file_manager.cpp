#include "file/file_manager.hpp"

void file::FileManager::NewFile(std::unique_ptr<IFile> file)
{
    m_openFiles.push_back(std::move(file));
    m_selectedDocument.file = m_openFiles.back().get();
    m_selectedDocument.index = m_openFiles.size() - 1;
}

void file::FileManager::OpenFile(std::unique_ptr<IFile> file)
{
    //!
    return;
}

void file::FileManager::SaveFile(size_t index)
{
    //!
    return;
}

void file::FileManager::CloseFile(size_t index)
{
    //!
    return;
}

void file::FileManager::SelectDocument(size_t index)
{
    if (index < m_openFiles.size()) {
        m_selectedDocument.file = m_openFiles[index].get();
        m_selectedDocument.index = index;
    }
}

file::SelectedDocument file::FileManager::GetSelectedDocument() const
{
    return m_selectedDocument;
}