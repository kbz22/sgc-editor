#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <filesystem>
#include "file/ifile.hpp"
#include "file/document_location.hpp"

namespace program 
{    
    class Session
    {
        private:
            nlohmann::json m_json{};            
            std::wstring m_sessionFileName = L"session.json";
            const std::string m_openedFilesString = "open-files";
            const std::string m_activeDocumentFileString = "active-document-file";
            const std::string m_activeDocumentIndexString = "active-document-index";
            const std::string m_windowPositionXString = "window-position-x";
            const std::string m_windowPositionYString = "window-position-y";
            const std::string m_windowWidthString = "window-width";
            const std::string m_windowHeightString = "window-height";
            const std::string m_cameraXString = "camera-x";
            const std::string m_cameraYString = "camera-y";
            const std::string m_zoomString = "zoom";

        public:
            void Load();
            void Save();

            void RebuildFileList(std::vector<std::unique_ptr<file::IFile>> const &files);
            std::vector<std::filesystem::path> GetSessionPaths();

            void SetWindowPositionAndSize(int x, int y, int width, int height);
            bool GetWindowPositionAndSize(int &x, int &y, int &width, int &height);

            void SetCameraPosition(float x, float y);
            bool GetCameraPosition(float &x, float &y);
            void SetZoom(float zoom);
            bool GetZoom(float &zoom);

            void SetActiveDocument(file::DocumentLocation &location);
            std::filesystem::path GetActiveDocumentPath();
            size_t GetActiveDocumentIndex();

    };
}