#include <QCoreApplication>
#include <QFile>
#include <QDataStream>
#include <iostream>
#include <QMessageBox>
#ifdef Q_OS_WIN
  #include <windows.h>
#endif

/** @brief Структра из 2 байтовых слов, содержащих тренировочный план дня */
struct TrainingDay {
  /* Побитово содержит в себе 4 слова "Прогресс подтягиваний"(Насколько меньше или больше подтягиваний стало) 5 бит,
   * "Прогресс отжиманий"(Насколько меньше или больше отжиманий стало) 5 бит,
   * "Прогресс удержания планки"(Насколько меньше или больше минут удалось удержать планку) 5 бит,
   * "Тяжелее предыдущей тренировки?" (Да/Нет) 1 бит */
  unsigned short usProgress = 0;
  // Подтягивания
  short sPullUp = 0;
  // Отжимания от пола
  short sPushUp = 0;
  // Планка
  short sPlank = 0;
};

int main()
{
#ifdef Q_OS_WIN
  // для отображения кириллицы
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);
#endif
  // Количество тренировочных дней
  short sCountDays = 1;

  std::cout << "Введите количество тренировочных дней от 1 до 7 включительно: " << std::flush;
  std::cin >> sCountDays;
  // Вектор тренировочных дней
  QVector<TrainingDay> vecTrainingWeek;
  // Резервирует память под количество тренировочных дней
  vecTrainingWeek.resize(sCountDays);

  // Создаем файл
  QFile file("TrainingWeek1.dat");
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(nullptr,
                         "Ошибка!",
                         "Не удалось создать файл",
                         QMessageBox::Ok);
    return 1;
  }
  // Создаем поток вывода в файл
  QDataStream stream(&file);
  // Определяем порядок байт с младшим разрядом в конце
  stream.setByteOrder(QDataStream::LittleEndian);
  // Записываем в файл данные
  for (const auto& caTrainingDay: vecTrainingWeek) {
    stream << caTrainingDay.usProgress;
    stream << caTrainingDay.sPullUp;
    stream << caTrainingDay.sPushUp;
    stream << caTrainingDay.sPlank;
  }

  file.close();
  return 0;
}
