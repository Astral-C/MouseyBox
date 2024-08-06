#include <system/Archive.hpp>
#define BSTREAM_IMPLEMENTATION
#include <bStream/bstream.h>
#include <map>
#include <algorithm>
#define POCKETLZMA_LZMA_C_DEFINE
#include <system/pocketlzma.hpp>
#include <system/Log.hpp>

namespace mb {

    uint32_t PadTo32(uint32_t x){
        return ((x + (32-1)) & ~(32-1));
    }
    
    void File::SetName(std::string n){
        mName = n;
    }

    void File::SetData(uint8_t* data, size_t size){
        mData = new uint8_t[size];
        mSize = size;
        memcpy(mData, data, size);
    }

    std::shared_ptr<Directory> Directory::New(std::shared_ptr<Archive> archive){
        std::shared_ptr<Directory> dir = std::make_shared<Directory>();
        archive->mDirectories.push_back(dir);
        return dir;
    }

    void Directory::SetName(std::string n){
        mName = n;
    }

    void Directory::AddSubdirectory(std::shared_ptr<Directory> f){
        f->mParent = GetPtr();
        mDirectories.push_back(f);
    }

    void Directory::AddFile(std::shared_ptr<File> f){
        mFiles.push_back(f);
    }

    std::shared_ptr<File> Directory::GetFile(std::filesystem::path path){
        if(path.begin() == path.end()) return nullptr;


        for(auto file : mFiles){
            if(file->GetName() == path.begin()->string()){
                if(((++path.begin()) == path.end())){
                    return file;
                } /*else {
                    if(file->MountAsArchive()){
                        std::filesystem::path subPath;
                        for(auto it = (++path.begin()); it != path.end(); it++) subPath  = subPath / it->string();
                        return (*file)->GetFile(subPath);
                    }
                }*/
            }
        }

        std::shared_ptr<File> file = nullptr;

        for(auto dir : mDirectories){
            if(dir->GetName() == path.begin()->string()){
                std::filesystem::path subPath;
                for(auto it = (++path.begin()); it != path.end(); it++) subPath  = subPath / it->string();
                file = dir->GetFile(subPath);
            }
        }

        return file;
    }

    std::shared_ptr<Directory> Directory::GetFolder(std::filesystem::path path){
        if(path.begin() == path.end()) return nullptr;


        for(auto folder : mDirectories){
            if(folder->GetName() == path.begin()->string()){
                if(((++path.begin()) == path.end())){
                    return folder;
                }
            }
        }

        std::shared_ptr<Directory> folder = nullptr;

        for(auto dir : mDirectories){
            if(dir->GetName() == path.begin()->string()){
                std::filesystem::path subPath;
                for(auto it = (++path.begin()); it != path.end(); it++) subPath  = subPath / it->string();
                folder = dir->GetFolder(subPath);
            }
        }

        return folder;
    }

        /*  Cheese Archive Spec
            Header
                CHSE - Fourcc/magic
                uint32_t archive size
                uint32_t dir count
                uint32_t dir offset
                uint32_t file node count
                uint32_t file node offset
                uint32_t file data size
                uint32_t file data offset
                uint32_t strings size
                uint32_t strings offset

            Dir Nodes
                uint16_t first file idx
                uint16_t file count
                uint32_t name offset 

            File Nodes
                uint8_t type: 0 = File, 1 = Dir
                uint8_t pad: file -> compressed
                uint16_t id: file -> id
                uint32_t size: file -> size of file, dir-> index of parent dir
                uint32_t offset: file -> offset of file data, dir-> file count
                uint32_t name offset 

            File Data
                uint8_t [file data size]
            Names
                list of null terminated strings, aligned to 32
        */

    bool Archive::Save(std::filesystem::path p){
        bStream::CFileStream stream(p.string(), bStream::OpenMode::Out);

        bStream::CMemoryStream directorySegment(0x08 * mDirectories.size(), bStream::Endianess::Little, bStream::OpenMode::Out);
        bStream::CMemoryStream fileSegment(0x0F * mDirectories.size(), bStream::Endianess::Little, bStream::OpenMode::Out);
        bStream::CMemoryStream fileDataSegment(0x20, bStream::Endianess::Little, bStream::OpenMode::Out);
        bStream::CMemoryStream nameTableSegment(0x20, bStream::Endianess::Little, bStream::OpenMode::Out);

        std::map<std::string, size_t> NameTable;

        uint16_t curFileIdx = 0;

        for(size_t d = 0; d < mDirectories.size(); d++){
            std::shared_ptr<Directory> dir = mDirectories[d];
            directorySegment.writeUInt16(curFileIdx);
            directorySegment.writeUInt16(dir->mFiles.size() + dir->mDirectories.size());
            
            if(NameTable.contains(dir->mName)){
                directorySegment.writeUInt32(NameTable.at(dir->mName));
            } else {
                NameTable.insert({dir->mName, nameTableSegment.tell()});
                directorySegment.writeUInt32(nameTableSegment.tell());

                nameTableSegment.writeString(dir->mName);
                nameTableSegment.writeUInt8(0);
            }

            for(size_t subd = 0; subd < dir->mDirectories.size(); subd++){
                std::shared_ptr<Directory> subdir = dir->mDirectories[subd];
                fileSegment.writeUInt8(1);
                fileSegment.writeUInt8(0);
                fileSegment.writeUInt16(0xFFFF);
                fileSegment.writeUInt32(std::find(mDirectories.begin(), mDirectories.end(), subdir) - mDirectories.begin());
                fileSegment.writeUInt32(subdir->mFiles.size() + subdir->mDirectories.size());
                if(NameTable.contains(subdir->mName)){
                    fileSegment.writeUInt32(NameTable.at(subdir->mName));
                } else {
                    NameTable.insert({subdir->mName, nameTableSegment.tell()});
                    fileSegment.writeUInt32(nameTableSegment.tell());

                    nameTableSegment.writeString(subdir->mName);
                    nameTableSegment.writeUInt8(0);
                }
                curFileIdx++;
            }

            for(size_t f = 0; f < dir->mFiles.size(); f++){
                std::shared_ptr<File> file = dir->mFiles[f];

                std::vector<uint8_t> CompressedData;
                if(file->mCompressed){
                    plz::PocketLzma p;
                    p.usePreset(plz::Preset::BestCompression);
                    std::vector<uint8_t> FileData;
                    std::copy(file->mData, file->mData + file->mSize, std::back_inserter(FileData));
                    p.compress(FileData, CompressedData);
                    mb::Log::Debug(std::format("Compressed File {}, OG size is {} Compressed Size is {}",  file->mName, FileData.size(), CompressedData.size()));
                }

                fileSegment.writeUInt8(0);

                if(file->mCompressed){
                    fileSegment.writeUInt8(1);
                    fileSegment.writeUInt16(curFileIdx);
                    fileSegment.writeUInt32(CompressedData.size());
                } else {
                    fileSegment.writeUInt8(0);
                    fileSegment.writeUInt16(curFileIdx);
                    fileSegment.writeUInt32(file->mSize);
                }

                fileSegment.writeUInt32(fileDataSegment.tell());
                if(NameTable.contains(file->mName)){
                    fileSegment.writeUInt32(NameTable.at(file->mName));
                } else {
                    NameTable.insert({file->mName, nameTableSegment.tell()});
                    fileSegment.writeUInt32(nameTableSegment.tell());

                    nameTableSegment.writeString(file->mName);
                    nameTableSegment.writeUInt8(0);
                }

                
                if(file->mCompressed){
                    fileDataSegment.writeBytes(CompressedData.data(), CompressedData.size());
                } else {
                    fileDataSegment.writeBytes(file->mData, file->mSize);
                }

                curFileIdx++;
            }
        }

        stream.writeUInt32(1163085891); //CHSE
        stream.writeUInt32(0x28 + directorySegment.getSize() + fileSegment.getSize() + fileDataSegment.getSize() + nameTableSegment.getSize());
        
        stream.writeUInt32(mDirectories.size());
        stream.writeUInt32(0x28);

        stream.writeUInt32(curFileIdx);
        stream.writeUInt32(0x28 + directorySegment.getSize());
        
        stream.writeUInt32(fileDataSegment.getSize());
        stream.writeUInt32(0x28 + directorySegment.getSize() + fileSegment.getSize());

        stream.writeUInt32(nameTableSegment.getSize());
        stream.writeUInt32(0x28 + directorySegment.getSize() + fileSegment.getSize() + fileDataSegment.getSize());

        stream.writeBytes(directorySegment.getBuffer(), directorySegment.getSize());
        stream.writeBytes(fileSegment.getBuffer(), fileSegment.getSize());

        stream.writeBytes(fileDataSegment.getBuffer(), fileDataSegment.getSize());

        stream.writeBytes(nameTableSegment.getBuffer(), nameTableSegment.getSize());

        return true;
    }


    void Directory::PrintStructure(std::shared_ptr<Directory> dir, int level){
        for(int x = 0; x < level; x++) std::cout << "  ";
        std::cout << dir->GetName() << std::endl;
        
        for(auto subdir : dir->mDirectories){
            Directory::PrintStructure(subdir, level+1);
        }

        for(auto file : dir->mFiles){
            for(int x = 0; x < level+1; x++) std::cout << "  ";
            std::cout << file->GetName() << std::endl;
        }

    }

    bool Archive::Load(uint8_t* data, size_t size){
        bStream::CMemoryStream stream(data, size, bStream::Endianess::Little, bStream::OpenMode::In);
        Log::Debug(std::format("System Endianess is {}", bStream::getSystemEndianess() == bStream::Endianess::Little ? "Little" : "Big"));
        stream.seek(0x08);

        uint32_t dirNodeCount = stream.readUInt32();
        uint32_t dirNodeOffset = stream.readUInt32();

        uint32_t fileNodeCount = stream.readUInt32();
        uint32_t fileOffset = stream.readUInt32();

        uint32_t fileDataSize = stream.readUInt32();
        uint32_t fileDataOffset = stream.readUInt32();

        uint32_t stringsSize = stream.readUInt32();
        uint32_t stringsOffset = stream.readUInt32();

        char* nameTable = new char[stringsSize];
        stream.seek(stringsOffset);
        stream.readBytesTo((uint8_t*)nameTable, stringsSize);

        for(size_t d = 0; d < dirNodeCount; d++){ mDirectories.push_back(std::make_shared<Directory>()); }

        stream.seek(dirNodeOffset);
        for(size_t d = 0; d < dirNodeCount; d++){
            // u16, u16, u32
            std::shared_ptr<Directory> dir = mDirectories[d];
            uint16_t firstFile = stream.readUInt16();
            uint16_t fileCount = stream.readUInt16();
            dir->SetName(std::string(nameTable + stream.readUInt32()));
            Log::Debug(std::format("Reading Directory {}", dir->GetName()));

            size_t dirPos = stream.tell();

            stream.seek(fileOffset + (firstFile * 16));
            for(int f = 0; f < fileCount; f++){
                uint8_t type = stream.readUInt8();
                uint8_t compressed = stream.readUInt8();
                uint16_t id = stream.readUInt16();

                if(type == 1){
                    uint32_t subdirIdx = stream.readUInt32();
                    //padding
                    stream.readUInt32();
                    stream.readUInt32();
                    dir->AddSubdirectory(mDirectories[subdirIdx]);
                    Log::Debug("Subdir Node");
                } else {
                    uint32_t fileSize = stream.readUInt32();
                    uint32_t fileOffset = stream.readUInt32();
                    uint32_t nameOffset = stream.readUInt32();
                    size_t retPos = stream.tell(); 
                    
                    std::shared_ptr<File> file =  std::make_shared<File>();
                    file->SetName(std::string(nameTable + nameOffset));
                    
                    Log::Debug(std::format("Reading File Node {:x} {:x} {} | {}", fileSize, fileOffset, file->GetName(), compressed));

                    std::vector<uint8_t> fileData(fileSize);
                    
                    stream.seek(fileOffset + fileDataOffset);

                    Log::Debug(std::format("Loading File At {:x}", fileOffset + fileDataOffset));

                    stream.readBytesTo(fileData.data(), fileSize);

                    if(compressed == 1){
                        plz::PocketLzma p;
                        std::vector<uint8_t> fileDecompressed;
                        p.decompress(fileData, fileDecompressed);
                        file->SetData(fileDecompressed.data(), fileDecompressed.size());
                        
                    } else {
                        file->SetData(fileData.data(), fileSize);
                    }
                    
                    dir->AddFile(file);
                    stream.seek(retPos);
                }
            }

            stream.seek(dirPos);
        }

        delete[] nameTable;

        return true;
    }

    bool Archive::Load(std::filesystem::path p){
        bStream::CFileStream stream(p.string(), bStream::Endianess::Little, bStream::OpenMode::In);
        stream.seek(0x08);

        uint32_t dirNodeCount = stream.readUInt32();
        uint32_t dirNodeOffset = stream.readUInt32();

        uint32_t fileNodeCount = stream.readUInt32();
        uint32_t fileOffset = stream.readUInt32();

        uint32_t fileDataSize = stream.readUInt32();
        uint32_t fileDataOffset = stream.readUInt32();

        uint32_t stringsSize = stream.readUInt32();
        uint32_t stringsOffset = stream.readUInt32();

        char* nameTable = new char[stringsSize];
        stream.seek(stringsOffset);
        stream.readBytesTo((uint8_t*)nameTable, stringsSize);

        for(size_t d = 0; d < dirNodeCount; d++){ mDirectories.push_back(std::make_shared<Directory>()); }

        stream.seek(dirNodeOffset);
        for(size_t d = 0; d < dirNodeCount; d++){
            // u16, u16, u32
            std::shared_ptr<Directory> dir = mDirectories[d];
            uint16_t firstFile = stream.readUInt16();
            uint16_t fileCount = stream.readUInt16();
            dir->SetName(std::string(nameTable + stream.readUInt32()));

            size_t dirPos = stream.tell();

            stream.seek(fileOffset + (firstFile * 16));
            for(size_t f = 0; f < fileCount; f++){
                uint8_t type = stream.readUInt8();
                uint8_t compressed = stream.readUInt8();
                uint16_t id = stream.readUInt16();

                if(type == 1){
                    uint32_t subdirIdx = stream.readUInt32();
                    //padding
                    stream.readUInt32();
                    stream.readUInt32();
                    dir->AddSubdirectory(mDirectories[subdirIdx]);
                } else {
                    uint32_t fileSize = stream.readUInt32();
                    uint32_t fileOffset = stream.readUInt32();
                    uint32_t nameOffset = stream.readUInt32();
                    size_t retPos = stream.tell(); 
                    
                    std::shared_ptr<File> file =  std::make_shared<File>();
                    file->SetName(std::string(nameTable + nameOffset));
                    
                    std::vector<uint8_t> fileData(fileSize);
                    
                    stream.seek(fileOffset + fileDataOffset);

                    stream.readBytesTo(fileData.data(), fileSize);

                    if(compressed == 1){
                        plz::PocketLzma p;
                        std::vector<uint8_t> fileDecompressed;
                        p.decompress(fileData, fileDecompressed);
                        file->SetData(fileDecompressed.data(), fileDecompressed.size());
                        
                    } else {
                        file->SetData(fileData.data(), fileSize);
                    }
                    
                    dir->AddFile(file);
                    stream.seek(retPos);
                }
            }

            stream.seek(dirPos);
        }

        delete[] nameTable;

        return true;
    }

}
