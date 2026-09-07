#pragma once
#include <QMap>
#include <QVector>
#include <bitset>

/** @brief Класс работы с прочитанными данными из файла */
class InformationDataProc
{
public:
  /** @brief Конструктор */
  InformationDataProc();

  /** @brief Деструктор */
  ~InformationDataProc();

  /** @brief Чтение данных из буфера */
  void vRead(QDataStream& InStream, int iFileSize);

  /** @brief Получение данных Тренировок */
  const QMap<int, QVector<short>>& crmapGetValue() { return _mapTrainingValue; }

  /** @brief Получение данных из позиции в таблице */
  int iGetValue(int iRowNum, int iColNum) { return _mapTrainingValue[iRowNum][iColNum]; }

  /** @brief Сохранение в контейнер нового значения */
  void vSaveVal(int iRowNum, int iColNum, short sNewVal)
                   { _mapTrainingValue[iRowNum][iColNum] = sNewVal; }

  /** @brief Сохранение в массив-буфер нового значения из таблицы */
  void vSaveNewVal(QDataStream& rOutStream);

  /** @brief Преобразование десятичного отрицательного числа в битовое представление */
  void vTansformToNegBinary(std::bitset<5>& rbitset, short sCode);

  /** @brief Преобразование десятичного отрицательного числа в битовое представление */
  void vSetCountDays(int iTrainingDaysCount) { _iTrainingDaysCount = iTrainingDaysCount; }

private:
  /** @brief Карта значений тренировок (ключ - день) */
  QMap<int, QVector<short>> _mapTrainingValue;

  /** @brief Структура данных (прогресса тренировок) в бинарнике */
  struct TrainingProgress {
    /* Побитово содержит в себе 4 слова "Прогресс подтягиваний"(Насколько меньше или больше подтягиваний стало) 5 бит,
     * "Прогресс отжиманий"(Насколько меньше или больше отжиманий стало) 5 бит,
     * "Прогресс приседаний"(Насколько меньше или больше приседаний стало) 5 бит,
     * "Тяжелее предыдущей тренировки?" (Да/Нет) 1 бит */
    unsigned short usProgress = 0;
    // Подтягивания
    short sPullUp = 0;
    // Отжимания от пола
    short sPushUp = 0;
    // Приседания
    short sQuat = 0;
  } _oTrainingProgress;

  /** @brief Выбранное количество тренировочных дней */
  int _iTrainingDaysCount;
};
