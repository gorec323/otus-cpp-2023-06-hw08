#pragma once

#include <fstream>
#include <boost/filesystem/path.hpp>

namespace bayan {

namespace bfs = boost::filesystem;

///
/// \brief The HashType enum Тип хэша
///
enum class HashType
{
    Crc32, //< Crc32
    Md5 //< Md5
};

///
/// \brief The FilePartitionHashCalculator class Калькулятор хэша для блока данных
///
class FilePartitionHashCalculator
{
public:
    FilePartitionHashCalculator(const bfs::path &path, HashType ht, std::size_t partitionSize = 1):
        m_fs {path.native(), std::ios::in | std::ios::binary},
        m_partitionSize {partitionSize},
        m_hashType {ht}
    {
        m_fs.seekg(0);
    }

    FilePartitionHashCalculator(const FilePartitionHashCalculator &) = delete;
    FilePartitionHashCalculator(FilePartitionHashCalculator &&) = default;

    virtual ~FilePartitionHashCalculator() = default;

    ///
    /// \brief calcNextHash Расчёт хэша для следующего блока
    ///
    void calcNextHash()
    {
        std::string buf(m_partitionSize, '0');

        if (!m_fs.read(&buf[0], buf.size())) {
            m_finished = true;
            m_fs.close();
        }

        calcNextHashImpl(buf.c_str(), buf.size());
    }

    ///
    /// \brief isEqual Проверка на равенство хэша текущего блока
    /// \param other Констатнтная ссылка на сравниваемый калькулятор
    /// \return true в случае одинакового хэша, иначе false
    ///
    bool isEqual(const FilePartitionHashCalculator &other) const
    {
        if ((m_hashType == other.m_hashType) && (m_partitionSize == other.m_partitionSize)
                && (m_finished == other.m_finished))

            return isEqualImpl(other);

        return false;
    }

    ///
    /// \brief finished Признак того, что файл считан до конца
    /// \return
    ///
    bool finished() const noexcept;

protected:
    ///
    /// \brief isEqualImpl Виртуальная функция сравнения хэшей
    /// \param other Другой калькулятор
    /// \return true в случае одинакового хэша, иначе false
    ///
    virtual bool isEqualImpl(const FilePartitionHashCalculator &other) const = 0;

    ///
    /// \brief calcNextHashImpl Виртуальная функция расчёта хэша
    /// \param data Указатель на блок данных
    /// \param size Размер блока
    ///
    virtual void calcNextHashImpl(const char * const data, std::size_t size) = 0;

private:
    std::ifstream m_fs;
    const std::size_t m_partitionSize;
    const HashType m_hashType;
    bool m_finished {false};
};

///
/// \brief makeHashCalculator Фабричная функция для собздания калькулятора хэша
/// \param path Путь к файлу
/// \param ht Тип хэша
/// \param partitionSize Размер блока для построения хэша
/// \return
///
std::unique_ptr<FilePartitionHashCalculator> makeHashCalculator(const bfs::path &path, HashType ht, std::size_t partitionSize);

}
