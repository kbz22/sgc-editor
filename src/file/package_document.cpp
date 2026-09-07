#include "file/package_document.hpp"

file::PackageDocument::PackageDocument(PackageFile& packageFile) :
    m_packageFile(packageFile)
{}

bool file::PackageDocument::IsActivable() const
{
    return false;
}

bool file::PackageDocument::IsDirty() const
{
    return m_packageFile.IsDirty();
}

bool file::PackageDocument::IsContainer() const
{
    return true;
}

const std::wstring& file::PackageDocument::GetName() const
{
    return m_packageFile.GetName();
}

file::ListableType file::PackageDocument::GetListableType() const
{
    return ListableType::PackageFile;
}