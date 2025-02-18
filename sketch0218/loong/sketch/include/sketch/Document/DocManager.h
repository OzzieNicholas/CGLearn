#pragma once

#include "sketch/Core.h"
#include <string.h>
#include <map>
#include <string>
#include <vector>
#include <functional>

namespace Engine
{
    class DObject;
    class Path;
}

namespace sketch 
{
    class Document;
    class MaterialManager;
    class ConfigureManager;
    class ProfileManager;

    class DocManager 
    {
    public:
        DocManager();
        ~DocManager();
        void release();

        bool init();
        void onAppClose();

        void newDocument(const Engine::Path& projPath, const std::wstring& env); //create new document
        void loadDocument(const Engine::Path& projPath, const std::wstring& backup, bool readOnly, const std::wstring& progName, std::function<void(bool)> onFinish); //load document
        void closeDocument();

        bool save(const Engine::Path& projPath, bool async, const std::wstring& progName);
        bool saveTemp(const std::wstring& progName);
        bool saveIndex(const Engine::Path& projPath, bool async);

        inline bool isLoading() const { return m_loading; }

        inline int* version() const { return (int *)m_version; }

        inline Document* document() const { return m_pDocument; }

        inline ConfigureManager* cfgMgr() const { return m_pCfgMgr; }

        inline bool isClosing() const { return m_closing; }

        /**
         * Returns the value of the last auto save time.
         *
         * @return The value of the last auto save time.
         */
        inline long long lastAutoSaveTm() const { return m_lastAutoSaveTm; }

        inline long long lastSaveFrame() const { return m_lastSaveFrame; }

        inline long long lastChangeFrame() const { return m_lastChangeFrame; }

        void updateSaveFrame();
        void updateModifyFrame();

        void update(unsigned long long tm);

    protected:
        int m_version[2]; //major, minor

        Document* m_pDocument;
        ConfigureManager* m_pCfgMgr;

        long long m_lastAutoSaveTm;
        long long m_lastSaveFrame;
        long long m_lastChangeFrame;

        bool m_loading;
        bool m_closing;        
    };

}