#include <boost/crc.hpp>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <regex>
#include <stack>
#include "bayanscanner.hpp"
#include "filepartitionhashcalculator.hpp"

namespace bayan {

///
/// \brief The BayanScanner::FileInfo struct Структура для хранения вспомогательной информации
/// для поиска дублей
///
struct BayanScanner::FileInfo: public std::pair<bfs::path, std::uintmax_t>
{
    constexpr FileInfo(const bfs::path &path, std::uintmax_t size): std::pair<bfs::path, std::uintmax_t>(path, size) {}
    FileInfo(const FileInfo &) = delete;
    FileInfo(FileInfo &&) noexcept = default;
    ~FileInfo() = default;

    FileInfo &operator=(const FileInfo &) = delete;
    FileInfo &operator=(FileInfo &&) = default;

    constexpr const bfs::path &filePath() const { return first; }
    constexpr std::uintmax_t fileSize() const { return second; }

    std::unique_ptr<FilePartitionHashCalculator> hashCalculator;
};

BayanScanner::BayanScanner(const ScanOptions &options):
    m_options(options)
{
}

void BayanScanner::run()
{
    scanDirs(m_options.m_scanPaths, m_options.m_excludesPaths, m_options.m_iPatterns, m_options.m_fileSize,
             m_options.m_scanDepth);
}

void BayanScanner::checkAndAddBySize(const boost::filesystem::path &file, intmax_t fileSizeFilter, std::vector<FileInfo> &files) const
{
    const auto fileSize = bfs::file_size(file);
    if (fileSizeFilter > 0) {
        if (fileSizeFilter > fileSize)
            return;
    } else if (fileSizeFilter < 0) {
        if (std::abs(fileSizeFilter) < fileSize)
            return;
    }

    files.emplace_back(BayanScanner::FileInfo{file, fileSize});
}

void BayanScanner::printDublicates(const std::vector<FileInfoGroup> &dublicates) const
{
    for (auto &&info : dublicates) {
        std::transform(info.first, info.second, std::ostream_iterator<std::string>(std::cout, ""), [](auto &&item)
        {
            return item.filePath().native() + " " + std::to_string(item.fileSize()) + '\n';
        });

        std::cout << std::endl;
    }
}

std::vector<BayanScanner::FileInfoGroup> BayanScanner::makeGroupsByHash(FileInfoVectorItC b, FileInfoVectorItC e) const
{
    if (b == e)
        return {};

    std::vector<FileInfoGroup> res;
    std::for_each(b, e, [this](auto &&fi)
    {
        fi.hashCalculator = makeHashCalculator(fi.filePath(), m_options.m_hashType, m_options.m_blockSize);
        fi.hashCalculator->calcNextHash();
    });

    auto l = b;
    // для хранения правой границы поиска в подгруппах
    std::stack<FileInfoVectorItC> rStack;
    rStack.push(e);
    while (l != e) {
        auto it = std::partition(l, rStack.top(), [l](const auto &fi)
        {
            return l->hashCalculator->isEqual(*fi.hashCalculator);
        });

        const auto d = std::distance(l, it);
        if (d == 1) {
            std::advance(l, 1);
            continue;
        } else if (d == 0) {
            l = rStack.top();
            rStack.pop();
            continue;
        }

        if (l->hashCalculator->finished()) {
            res.emplace_back(FileInfoGroup{l, it});
            l = it;
            if (rStack.top() != e)
                rStack.pop();

            continue;
        }

        if (rStack.top() != it)
            rStack.push(it);

        std::for_each(l, rStack.top(), [](auto &&fi)
        {
            fi.hashCalculator->calcNextHash();
        });
    }

    return res;
}

std::vector<BayanScanner::FileInfoGroup> BayanScanner::scanDirs(const std::vector<bfs::path> &includePaths, const std::vector<bfs::path> &excludePaths, const std::vector<std::string> &iNamePatterns, std::intmax_t fileSizeFilter, std::size_t depth) const
{
    std::vector<FileInfo> files;

    // составление списка файлов для дальнейшей обработки с учетом фильтров по размеру и
    // имени файла
    for (auto &&path : includePaths) {
        bfs::recursive_directory_iterator it(path, bfs::directory_options::skip_permission_denied);
        bfs::recursive_directory_iterator end;
        for (; it != end; ++it) {
//            std::cout << "scandirs depth = " << it.depth() << " " << it->path() << std::endl;
            if (bfs::is_regular_file(it->status()) && !bfs::is_symlink(it->symlink_status())) {
                if (iNamePatterns.empty())
                    checkAndAddBySize(it->path(), fileSizeFilter, files);

                for (auto &&pattern : iNamePatterns) {
                    const std::regex expr(pattern, std::regex_constants::icase);
                    if (std::regex_match(it->path().filename().native(), expr))
                        checkAndAddBySize(it->path(), fileSizeFilter, files);
                }
            } else if (bfs::is_directory(it.status())) {
                if (it.depth() >= depth)
                    it.disable_recursion_pending();

                const auto absPath = bfs::canonical(*it);
                for (auto &&exclude : excludePaths) {
                    if (absPath == bfs::canonical(exclude))
                        it.disable_recursion_pending();
                }
            }
        }
    }

    if (files.empty())
        return {};

    std::vector<FileInfoGroup> finalGroups;
    for (auto it = files.begin(), eit = files.end(); it != eit; ) {

        const auto oldIt = it;
        it = std::partition(oldIt, eit, [size = oldIt->fileSize()](auto &&fileInfo)
        {
            return size == fileInfo.fileSize();
        });

        // если файлов с таким размером больше нет, то у него нет дублей
        const auto d = std::distance(oldIt, it);

        if (d == 1)
            continue;

//        std::cout << "scanDirs " <<  d << std::endl;

        // формирование групп на основе хэшей
        auto &&sizeGroups = makeGroupsByHash(oldIt, it);

        // вывод промежуточных групп на экран
        printDublicates(sizeGroups);
        std::move(sizeGroups.begin(), sizeGroups.end(), std::back_inserter(finalGroups));
//        std::cout << "**** " << (std::distance(files.begin(), it) / static_cast<double>(files.size()) * 100) << std::endl;
    }

    return finalGroups;
}

}
