#pragma once

#include "scanoptions.hpp"

#include <map>
#include <unordered_map>

namespace bayan {

///
/// \brief The BayanScanner class Класс санер для поиска одинаковых файлов с бережным
/// обращение к диску
///
class BayanScanner
{
public:
    ///
    /// \brief BayanScanner Коснтруктор
    /// \param options Опции сканироавния
    ///
    BayanScanner(const ScanOptions &options);

    ///
    /// \brief run Запуск сканирования
    ///
    void run();

private:

    struct FileInfo;

    using FileInfoVectorItC = std::vector<FileInfo>::iterator;

    using FileInfoGroup = std::pair<FileInfoVectorItC, FileInfoVectorItC>;

    ///
    /// \brief checkAndAddBySize Проверка фильтра на размер файла и добавление в \param files в случае успеха
    /// \param file Путь к файлу
    /// \param fileSizeFilter Фильтр размера файла. Если положительное число, то ищутся файлы с размером >= указынному, иначе меньше
    /// \param files Вектор для заполнения в случае успеха
    ///
    void checkAndAddBySize(const bfs::path &file, intmax_t fileSizeFilter, std::vector<BayanScanner::FileInfo> &files) const;

    ///
    /// \brief printDublicates Вывод на экран групп дублирующихся файлов
    /// \param dublicates Вектор групп
    ///
    void printDublicates(const std::vector<FileInfoGroup> &dublicates) const;

    ///
    /// \brief makeGroupsByHash Формирование групп дублирующихся файлов из группы с одинаковым размером
    /// \param b Итератор на начало группы с файлами одинакового размера
    /// \param e Итератор на следующий элемент группы с файлами одинакового размера
    /// \return Вектор групп на итераторы с одинаковыми файлами
    ///
    std::vector<FileInfoGroup> makeGroupsByHash(FileInfoVectorItC b, FileInfoVectorItC e) const;

    ///
    /// \brief scanDirs Сканирование директорий
    /// \param includePaths Список путей сканирования
    /// \param excludePaths Список игнорируемых путей сканирования
    /// \param iNamePatterns Регистронезависимый фильтр имени
    /// \param fileSize Фильтр размера файла
    /// \param depth глубина сканирования
    /// \return  Вектор групп итераторов одинаковых файлов
    ///
    std::vector<FileInfoGroup> scanDirs(const std::vector<bfs::path> &includePaths, const std::vector<bfs::path> &excludePaths,
                const std::vector<std::string> &iNamePatterns, std::intmax_t fileSize, std::size_t depth) const;

    ScanOptions m_options;
};

}
