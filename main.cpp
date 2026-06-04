/*
 * Практическое задание по алгоритмам сортировок - Вариант 4
 * Структура данных: цветы (название, цвет, аромат, регионы).
 * Реализованы: сортировка выбором, пирамидальная, быстрая, std::sort.
 * Данные генерируются автоматически. Результаты пишутся в CSV.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <functional>
#include <iomanip>
#include <random>

/**
 *  Данные о цветке.
 *
 * Поля сравнения (по приоритету): название -> цвет -> аромат.
 * Аромат ранжируется: "сильный" > "умеренный" > "слабый".
 */
struct Flower {
    std::string name;     ///< Название цветка
    std::string color;    ///< Цвет цветка
    std::string aroma;    ///< Аромат: "сильный", "умеренный", "слабый"
    std::string regions;  ///< Регионы распространения

    /**
     *  Числовой ранг аромата для сравнения.
     * return 2 - сильный, 1 - умеренный, 0 - слабый
     */
    int aromaRank() const {
        if (aroma == "сильный")   return 2;
        if (aroma == "умеренный") return 1;
        return 0;
    }

    ///  Лексикографическое сравнение: name -> color -> aromaRank
    bool operator<(const Flower& o) const {
        if (name  != o.name)  return name  < o.name;
        if (color != o.color) return color < o.color;
        return aromaRank() < o.aromaRank();
    }
    bool operator>(const Flower& o)  const { return o < *this; }
    bool operator<=(const Flower& o) const { return !(o < *this); }
    bool operator>=(const Flower& o) const { return !(*this < o); }
    bool operator==(const Flower& o) const { return !(*this < o) && !(o < *this); }
};

//  ЗАПИСЬ В CSV 

/**
 *  Записать массив цветков в CSV-файл.
 *  filename Путь к файлу
 *  flowers  Массив цветков
 */
void writeToCSV(const std::string& filename, const std::vector<Flower>& flowers) {
    std::ofstream file(filename);
    file << "name,color,aroma,regions\n";
    for (const auto& f : flowers)
        file << f.name << ',' << f.color << ',' << f.aroma << ',' << f.regions << '\n';
}

//  АЛГОРИТМЫ СОРТИРОВКИ 

/**
 *  Сортировка выбором (Selection Sort). O(n^2).
 *  arr Массив, сортируется на месте
 */
void selectionSort(std::vector<Flower>& arr) {
    int n = static_cast<int>(arr.size());
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j)
            if (arr[j] < arr[minIdx]) minIdx = j;
        if (minIdx != i) std::swap(arr[i], arr[minIdx]);
    }
}

/**
 *  Вспомогательное просеивание для пирамидальной сортировки.
 *  arr Массив
 *  n   Размер кучи
 *  i   Корень поддерева
 */
void heapify(std::vector<Flower>& arr, int n, int i) {
    int largest = i;
    int l = 2*i + 1, r = 2*i + 2;
    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;
    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

/**
 *  Пирамидальная сортировка (Heap Sort). O(n log n).
 *  arr Массив, сортируется на месте
 */
void heapSort(std::vector<Flower>& arr) {
    int n = static_cast<int>(arr.size());//зачем приведение типов
    for (int i = n/2 - 1; i >= 0; --i) heapify(arr, n, i);//строим макс-кучу
    for (int i = n - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

/**
 *  Разбиение Ломуто для быстрой сортировки.
 *  arr  Массив
 *  low  Левая граница
 *  high Правая граница
 * return Итоговый индекс опорного элемента
 */
int partition(std::vector<Flower>& arr, int low, int high) {
    Flower pivot = arr[high];
    int i = low - 1;//правая граница элементов меньше опорного
    for (int j = low; j < high; ++j)
        if (arr[j] <= pivot) std::swap(arr[++i], arr[j]);
    std::swap(arr[i+1], arr[high]);
    return i + 1;
}

/**
 *  Быстрая сортировка (Quick Sort). O(n log n) среднее.
 *  arr  Массив
 *  low  Левая граница
 *  high Правая граница
 */
void quickSort(std::vector<Flower>& arr, int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quickSort(arr, low, p - 1);
        quickSort(arr, p + 1, high);
    }
}

///  Обёртка quickSort для интерфейса std::function
void quickSortWrapper(std::vector<Flower>& arr) {
    if (!arr.empty()) quickSort(arr, 0, (int)arr.size() - 1);
}

//  ГЕНЕРАЦИЯ ДАННЫХ 

/**
 *  Генерировать случайный массив цветков заданного размера.
 *  n   Количество элементов
 *  seed Зерно генератора (для воспроизводимости)
 * return std::vector<Flower>
 */
std::vector<Flower> generateData(int n, unsigned seed = 42) {
    static const std::vector<std::string> names   = {"Роза","Тюльпан","Лилия","Ромашка","Орхидея","Гвоздика","Пион","Нарцисс","Хризантема","Ирис","Незабудка","Лаванда","Мак","Васильки","Фиалка"};
    static const std::vector<std::string> colors  = {"Красный","Белый","Жёлтый","Розовый","Синий","Фиолетовый","Оранжевый","Бордовый"};
    static const std::vector<std::string> aromas  = {"сильный","умеренный","слабый"};
    static const std::vector<std::string> regions = {"Европа","Азия","Африка","Америка","Австралия","Средняя полоса","Субтропики","Тропики"};

    std::mt19937 rng(seed);
    auto pick = [&](const std::vector<std::string>& v) -> const std::string& {
        return v[std::uniform_int_distribution<int>(0, (int)v.size()-1)(rng)];
    };

    std::vector<Flower> flowers(n);
    for (auto& f : flowers) {
        f.name    = pick(names);
        f.color   = pick(colors);
        f.aroma   = pick(aromas);
        f.regions = pick(regions);
    }
    return flowers;
}

//  ЗАМЕР ВРЕМЕНИ 

/**
 *  Замерить время выполнения алгоритма сортировки.
 *  arr      Исходный массив (копируется внутри)
 *  sortFunc Функция сортировки
 * return Время выполнения в миллисекундах
 */
double measureTime(const std::vector<Flower>& arr,
                   std::function<void(std::vector<Flower>&)> sortFunc) {
    std::vector<Flower> copy = arr;
    auto t0 = std::chrono::high_resolution_clock::now();
    sortFunc(copy);
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();//переводим время в миллисекунды
}

//  MAIN 

int main() {
    //  1. Генерация входных данных и сортировка (запись в файлы) 
    std::cout << " Sorting Lab, Variant 4 (Flowers) \n\n";

    // Генерируем базовый набор из 1000 элементов для демонстрации
    auto flowers = generateData(1000);

    // Сохраняем исходный массив
    writeToCSV("flowers_input.csv", flowers);
    std::cout << "[1/5] Input data written to: flowers_input.csv (" << flowers.size() << " records)\n";

    // Сортировка выбором
    auto s1 = flowers; selectionSort(s1);
    writeToCSV("output_selection.csv", s1);
    std::cout << "[2/5] Selection sort done  -> output_selection.csv\n";

    // Пирамидальная сортировка
    auto s2 = flowers; heapSort(s2);
    writeToCSV("output_heap.csv", s2);
    std::cout << "[3/5] Heap sort done       -> output_heap.csv\n";

    // Быстрая сортировка
    auto s3 = flowers; quickSortWrapper(s3);
    writeToCSV("output_quick.csv", s3);
    std::cout << "[4/5] Quick sort done      -> output_quick.csv\n";

    // std::sort
    auto s4 = flowers; std::sort(s4.begin(), s4.end());
    writeToCSV("output_stdsort.csv", s4);
    std::cout << "[5/5] std::sort done       -> output_stdsort.csv\n";

    //  2. Замер времени для разных размеров массива 
    std::cout << "\n Timing Benchmark \n";

    std::vector<int> sizes = {100, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000};

    std::ofstream tf("timing_results.csv");
    tf << "size,selection_ms,heap_ms,quick_ms,stdsort_ms\n";

    std::cout << std::setw(10) << "n"
              << std::setw(18) << "Selection (ms)"
              << std::setw(16) << "Heap (ms)"
              << std::setw(14) << "Quick (ms)"
              << std::setw(14) << "std::sort\n";
    std::cout << std::string(72, '-') << '\n';

    for (int n : sizes) {
        // Каждый размер - новый случайный массив с уникальным seed
        auto data = generateData(n, n);

        double tSel  = measureTime(data, selectionSort);
        double tHeap = measureTime(data, heapSort);
        double tQck  = measureTime(data, quickSortWrapper);
        double tStd  = measureTime(data, [](std::vector<Flower>& a) {
            std::sort(a.begin(), a.end());
        });

        // Вывод в консоль
        std::cout << std::fixed << std::setprecision(3)
                  << std::setw(10) << n
                  << std::setw(18) << tSel
                  << std::setw(16) << tHeap
                  << std::setw(14) << tQck
                  << std::setw(14) << tStd << '\n';

        // Запись в CSV
        tf << n << ',' << tSel << ',' << tHeap << ',' << tQck << ',' << tStd << '\n';
        tf.flush(); // сбросить буфер сразу после каждой строки
    }

    tf.close();
    std::cout << "\nTiming results saved to: timing_results.csv\n";
    std::cout << "Build a chart from this file (Python/Excel).\n";

    return 0;
}
