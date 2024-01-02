#include <iostream>
#include <cstring>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include "filepartitionhashcalculator.hpp"

namespace bayan {

class Crc32PartitionHashCalculator: public FilePartitionHashCalculator
{
public:
    Crc32PartitionHashCalculator(const bfs::path &path, std::size_t partitionSize = 1):
        FilePartitionHashCalculator(path, HashType::Crc32, partitionSize)
    {
    }

    // FilePartitionHashCalculator interface
public:
    void calcNextHashImpl(const char * const data, std::size_t size) override
    {
        boost::crc_32_type seed;
        seed.process_bytes(data, size);

        m_currentHash = seed.checksum();
    }

    bool isEqualImpl(const FilePartitionHashCalculator &other) const override
    {
        try {
           const auto &crc32Calc = static_cast<const Crc32PartitionHashCalculator &>(other);
           return m_currentHash == crc32Calc.m_currentHash;
        }  catch (const std::bad_cast &e) {
            std::cerr << e.what() << std::endl;
        }

        return false;
    }

private:
    boost::crc_32_type::value_type m_currentHash {0};
};

class Md5PartitionHashCalculator: public FilePartitionHashCalculator
{
public:
    Md5PartitionHashCalculator(const bfs::path &path, std::size_t partitionSize = 1):
        FilePartitionHashCalculator(path, HashType::Md5, partitionSize)
    {
    }

    // FilePartitionHashCalculator interface
public:
    void calcNextHashImpl(const char * const data, std::size_t size) override
    {
        using boost::uuids::detail::md5;
        md5 hash;
        md5::digest_type digest;

        hash.process_bytes(data, size);
        hash.get_digest(m_currentHash);
    }

    bool isEqualImpl(const FilePartitionHashCalculator &other) const override
    {
        try {
           const auto &md5Calc = static_cast<const Md5PartitionHashCalculator &>(other);
           return std::memcmp(m_currentHash, md5Calc.m_currentHash, sizeof(m_currentHash)) == 0;
        }  catch (const std::bad_cast &e) {
            std::cerr << e.what() << std::endl;
        }

        return false;
    }

private:
    boost::uuids::detail::md5::digest_type m_currentHash;
};

std::unique_ptr<FilePartitionHashCalculator> makeHashCalculator(const boost::filesystem::path &path, HashType ht, std::size_t partitionSize)
{
    switch (ht) {
    case HashType::Crc32:
        return std::make_unique<Crc32PartitionHashCalculator>(path, partitionSize);

    case HashType::Md5:
        return std::make_unique<Md5PartitionHashCalculator>(path, partitionSize);

    }

    return {};
}

bool FilePartitionHashCalculator::finished() const noexcept
{
    return m_finished;
}

}
