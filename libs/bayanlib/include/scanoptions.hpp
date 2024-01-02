#pragma once

#include <boost/filesystem.hpp>
#include "filepartitionhashcalculator.hpp"

namespace bfs = boost::filesystem;

namespace bayan {

///
/// \brief The ScanOptions struct Опции скнирования файловой системы
///
struct ScanOptions
{
    std::vector<bfs::path> m_scanPaths;
    std::vector<bfs::path> m_excludesPaths;
    std::vector<std::string> m_iPatterns;
    std::size_t m_scanDepth {0};
    std::intmax_t m_fileSize {0};
    std::size_t m_blockSize {0};
    HashType m_hashType {HashType::Crc32};
};

}
