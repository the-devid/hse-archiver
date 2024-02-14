#include <iostream>
#include <cstring>
#include <fstream>
#include "encoder.h"
#include "archiver_constants.h"
#include "decoder.h"
#include "archiver_exceptions.h"

int main(int argc, char** argv) {
    try {
        if (argc < 2) {
            std::cerr << "No arguments!\n";
            return archiver::MAIN_BAD_RETURN;
        }
        if (std::strcmp(argv[1], "-c") == 0) {
            if (argc < 4) {
                std::cerr << "Less arguments than expected\n";
                return archiver::MAIN_BAD_RETURN;
            }
            std::string archive_name(argv[2]);
            std::ofstream archive_stream(archive_name);
            archiver::BitWriter writer(archive_stream);
            archiver::Encoder encoder;
            for (int file_ind = 3; file_ind < argc; ++file_ind) {
                std::string path_to_file(argv[file_ind]);
                encoder.EncodeFile(path_to_file, writer, file_ind + 1 == argc);
            }
        } else if (std::strcmp(argv[1], "-d") == 0) {
            if (argc != 3) {
                std::cerr << "Missing archive\'s name\n";
                return archiver::MAIN_BAD_RETURN;
            }
            std::string archive_name(argv[2]);
            std::ifstream archive_stream(archive_name);
            archiver::BitReader archive_bitstream(archive_stream);
            archiver::Decoder decoder;
            decoder.DecodeArchive(archive_bitstream);
        } else if (std::strcmp(argv[1], "-h") == 0) {
            std::cout
                << "This is simple archiver, using Huffman\'s coding\n\t* Write -h to get help message\n\t* Write -c "
                   "archive_name file1 [files...] to archive files\n\t* Write -d archive_name to dearchivate archive\n";
        }
    } catch (archiver::BitReadException& exception) {
        std::cerr << exception.what() << std::endl;
        return archiver::MAIN_BAD_RETURN;
    } catch (archiver::UnknownCharException& exception) {
        std::cerr << exception.what() << std::endl;
        return archiver::MAIN_BAD_RETURN;
    } catch (archiver::WrongArchiveException& exception) {
        std::cerr << exception.what() << "\nArchive is corrupted" << std::endl;
        return archiver::MAIN_BAD_RETURN;
    } catch (archiver::EmptyCodeException& exception) {
        std::cerr << exception.what() << std::endl;
        return archiver::MAIN_BAD_RETURN;
    } catch (std::bad_alloc& exception) {
        std::cerr << "Bad alloc: probably not enough memory" << std::endl;
        return archiver::MAIN_BAD_RETURN;
    } catch (std::exception& exception) {
        std::cerr << "Some error occurred during executing:\n" << exception.what() << std::endl;
        return archiver::MAIN_BAD_RETURN;
    }
    return 0;
}
