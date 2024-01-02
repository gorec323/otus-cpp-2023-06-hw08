#include <iostream>
#include <boost/program_options.hpp>
#include "bayanscanner.hpp"
#include "version.h"

using namespace std;

///
/// \brief hashStrToEnum Формирование типа хэша из строки параметров программы
/// \param str
/// \return
///
bayan::HashType hashStrToEnum(const std::string &str)
{
    if (str == "md5")
        return bayan::HashType::Md5;

    if (str == "crc32")
        return bayan::HashType::Crc32;

    return bayan::HashType::Md5;
}

int main(int argc, char* argv[])
{
    using namespace std;

    namespace bpo = boost::program_options;

    // Группа параметров, которые не влияют на работу программы
    bpo::options_description generic("Generic options");

    const auto OPT_HELP = "help";
    const auto OPT_VERSION = "version";

    generic.add_options()
        (OPT_VERSION, "print version string")
        (OPT_HELP, "Print a summary of the command-line usage of bayan and exit")
        ;


    const auto OPT_INCLUDE_PATH = "include-path";
    const auto OPT_EXCLUDE_PATH = "exclude";
    const auto OPT_DEPTH = "depth";
    const auto OPT_NAME_PATTERN = "name";
    const auto OPT_INAME_PATTERN = "iname";
    const auto OPT_SIZE = "size";
    const auto OPT_BLOCK_SIZE = "block-size";
    const auto OPT_HASH = "hash";

    // Группа параметров, которые влияют на работу программы
    bpo::options_description desc("Base options");
    desc.add_options()
            (OPT_INCLUDE_PATH, bpo::value<std::vector<bfs::path>>(), "Include path(s)")
            (OPT_EXCLUDE_PATH, bpo::value<std::vector<bfs::path>>(), "Exclude path(s)")
            (OPT_DEPTH, bpo::value<std::size_t>()->default_value(0), "Scan depth: 0 - only target dir")
            (OPT_NAME_PATTERN, bpo::value<std::vector<std::string>>(), "File name pattern")
            (OPT_INAME_PATTERN, bpo::value<std::vector<std::string>>(), "Like -name, but the match is case insensitive")
            (OPT_SIZE, bpo::value<std::intmax_t>()->default_value(1), "Minimal file size for scan in bytes")
            (OPT_BLOCK_SIZE, bpo::value<std::size_t>()->default_value(8), "File read block size in bytes")
            (OPT_HASH, bpo::value<std::string>()->default_value("md5"), "Hash algorithm: crc, md5");
    ;

    bpo::positional_options_description pos;
    pos.add(OPT_INCLUDE_PATH, -1);

    bpo::options_description all;
    all.add(generic).add(desc);

    const auto parsed = bpo::command_line_parser(argc, argv).options(all).positional(pos).run();

    bpo::variables_map vm;
    bpo::store(parsed, vm);

    if (vm.count(OPT_HELP) > 0) {
        std::cout << "Usage: " << all << '\n';
        return 0;
    }

    if (vm.count(OPT_VERSION) > 0) {
        std::cout << PROJECT_VERSION << '\n';
        return 0;
    }

    if (vm.count(OPT_INCLUDE_PATH) == 0) {
        std::cout << "Usage: " << argv[0] << " include dir..." << '\n' << all << '\n';
        return 0;
    }

    auto optValue = [&vm](const char * const name, auto &value) {
        if (vm.count(name) > 0)
            value = vm[name].as<std::remove_reference_t<decltype(value)>>();
    };

    bayan::ScanOptions options;
    optValue(OPT_INCLUDE_PATH, options.m_scanPaths);
    optValue(OPT_EXCLUDE_PATH, options.m_excludesPaths);
    optValue(OPT_INAME_PATTERN, options.m_iPatterns);

    optValue(OPT_DEPTH, options.m_scanDepth);
    optValue(OPT_SIZE, options.m_fileSize);
    optValue(OPT_BLOCK_SIZE, options.m_blockSize);

    { // блок обработки типа хэша
        std::string hashStr;
        optValue(OPT_HASH, hashStr);
        options.m_hashType = hashStrToEnum(hashStr);
    }

    bayan::BayanScanner scanner {options};
    scanner.run();

    return 0;
}
