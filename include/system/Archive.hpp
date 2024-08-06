#ifndef __MB_ARCHIVE_H__
#define __MB_ARCHIVE_H__
#include <string>
#include <format>
#include <cstdint>
#include <memory>
#include <vector>
#include <filesystem>

namespace mb {

    class Archive;

    class File : public std::enable_shared_from_this<File> {
            friend Archive;
            uint8_t* mData { nullptr };
            size_t mSize { 0 };
            uint32_t mFileId { 0 };
            std::string mName { "" };
            bool mCompressed { false };
        public:
            std::shared_ptr<File> CreateFile();

            void SetCompressed(bool c) { mCompressed = c; }

            std::string GetName() { return mName; }
            void SetName(std::string);

            size_t GetSize() { return mSize; }

            uint8_t* GetData() { return mData; }
            void SetData(uint8_t*, size_t);

            File(){}
            ~File(){
                if(mData != nullptr) delete[] mData;
            }

    };

    class Directory : public std::enable_shared_from_this<Directory> {
            friend Archive;
            std::string mName { "" };
            std::weak_ptr<Directory> mParent {};
            std::vector<std::shared_ptr<Directory>> mDirectories {};
            std::vector<std::shared_ptr<File>> mFiles {};
        public:
            std::shared_ptr<Directory> CreateDirectory();

            std::string GetName() { return mName; }
            void SetName(std::string);
            void AddSubdirectory(std::shared_ptr<Directory>);
            void AddFile(std::shared_ptr<File>);
            
            std::shared_ptr<File> GetFile(std::filesystem::path);
            std::shared_ptr<Directory> GetFolder(std::filesystem::path);

            static void PrintStructure(std::shared_ptr<Directory>, int);

            static std::shared_ptr<Directory> New(std::shared_ptr<Archive>);

            std::vector<std::shared_ptr<Directory>>& GetFolders() { return mDirectories; }
            std::vector<std::shared_ptr<Directory>>* GetFoldersPtr() { return &mDirectories; }

            std::vector<std::shared_ptr<File>>& GetFiles() { return mFiles; }
            std::vector<std::shared_ptr<File>>* GetFilesPtr() { return &mFiles; }

            std::shared_ptr<Directory> GetPtr() {
                return shared_from_this();
            }

            Directory(){}
            ~Directory(){}
    };
    
    class Archive :  public std::enable_shared_from_this<Archive>{
            friend Directory;
            std::vector<std::shared_ptr<Directory>> mDirectories {};

        public:

            static std::shared_ptr<Archive> New(){
                return std::make_shared<Archive>();
            }

            static std::shared_ptr<Archive> New(std::string rootname){
                std::shared_ptr<Directory> dir = std::make_shared<Directory>();
                dir->SetName(rootname);
                
                std::shared_ptr<Archive> arc = std::make_shared<Archive>();
                arc->mDirectories.push_back(dir);
                return arc;
            }

            bool Load(uint8_t*, size_t);
            bool Load(std::filesystem::path);
            bool Save(std::filesystem::path);

            std::shared_ptr<Directory> GetRoot() { return mDirectories[0]; }
            Archive(){}
            ~Archive(){}
        
    };
}

#endif