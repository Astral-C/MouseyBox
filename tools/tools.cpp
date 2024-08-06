#include <filesystem>
#include <system/Archive.hpp>
#include <bStream/bstream.h>
#include "argparse.hpp"
#include <string>
#include <set>

std::set<std::string> CompressExtensions = {};

void ExtractFolder(std::shared_ptr<mb::Directory> folder){
    std::filesystem::create_directory(folder->GetName());
    std::filesystem::current_path(std::filesystem::current_path() / folder->GetName());

    for(auto file : folder->GetFiles()){
        bStream::CFileStream extractFile(file->GetName(), bStream::Endianess::Big, bStream::OpenMode::Out);
        extractFile.writeBytes(file->GetData(), file->GetSize());
    }

    for(auto subdir : folder->GetFolders()){
        ExtractFolder(subdir);
    }

    std::filesystem::current_path(std::filesystem::current_path().parent_path());
}

void PackFolder(std::shared_ptr<mb::Archive> arc, std::shared_ptr<mb::Directory> folder, std::filesystem::path path){
    std::filesystem::current_path(path);
    
    for (auto const& dir_entry : std::filesystem::directory_iterator(path)){
        if(std::filesystem::is_directory(dir_entry.path())){
            std::shared_ptr<mb::Directory> subdir = mb::Directory::New(arc);
            subdir->SetName(dir_entry.path().filename().string());
            folder->AddSubdirectory(subdir);
            
            PackFolder(arc, subdir, dir_entry.path());

        } else {
            std::shared_ptr<mb::File> file = std::make_shared<mb::File>();

            bStream::CFileStream fileStream(dir_entry.path().string(), bStream::Endianess::Big, bStream::OpenMode::In);
            
            uint8_t* fileData = new uint8_t[fileStream.getSize()];
            fileStream.readBytesTo(fileData, fileStream.getSize());
            
            file->SetData(fileData, fileStream.getSize());
            file->SetName(dir_entry.path().filename().string());

            if(CompressExtensions.contains(dir_entry.path().extension().string().substr(1))){
                std::cout << "[Compressing file " << dir_entry.path() << "]" << std::endl;
                file->SetCompressed(true);
            }

            folder->AddFile(file);

            delete[] fileData;
        }
    }
    std::filesystem::current_path(std::filesystem::current_path().parent_path());
}

void PackArchive(std::filesystem::path path){
    std::shared_ptr<mb::Archive> archive = mb::Archive::New(path.filename().string());

    std::string ext = ".chse";

    PackFolder(archive, archive->GetRoot(), std::filesystem::current_path() / path);

    archive->Save(path.filename().string()+ext);


}

int main(int argc, char* argv[]){
    int level; //compression level for yaz0

    argparse::ArgumentParser mouseytools("MouseyBoxArcTools", "0.0.1", argparse::default_arguments::help);

    mouseytools.add_argument("-i", "--input").required().help("File/Directory to operate on");
    mouseytools.add_argument("-c", "--compress-ext").help("File extensions to compress, split with ','");
    mouseytools.add_argument("-x", "--extract").help("Extract input archive").flag();
    mouseytools.add_argument("-p", "--pack").help("Pack input folder").flag();

    try {
        mouseytools.parse_args(argc, argv);
    } catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        std::cerr << mouseytools;
        return 1;
    }

    auto path = std::filesystem::path(mouseytools.get<std::string>("--input"));

    if(mouseytools.is_used("--compress-ext")){
        auto extensions = std::istringstream(mouseytools.get<std::string>("--compress-ext"));

        for(std::string ext; std::getline(extensions, ext, ',');){
            CompressExtensions.insert(ext);
        }
    }

    if(!std::filesystem::exists(path)){
        std::cerr << "Couldn't find path " << path.string() << std::endl;
        return 1;
    }

    if(mouseytools.is_used("--extract")){
        if(std::filesystem::is_directory(path)){
            std::cerr << path.string() << " is a directory" << std::endl;
            return 1;
        }

        std::shared_ptr<mb::Archive> archive = mb::Archive::New("root");
        
        if(!archive->Load(path)){
            std::cerr << "Couldn't parse file " << path.string() << std::endl;
            return 1;
        }

        ExtractFolder(archive->GetRoot());

    } else if(mouseytools.is_used("--pack")){
        if(!std::filesystem::is_directory(path)){
            std::cerr << path.string() << " is not a directory" << std::endl;
            return 1;
        }

        PackArchive(path);
    }

    return 0;
}