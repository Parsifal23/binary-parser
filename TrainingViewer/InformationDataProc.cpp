#include "InformationDataProc.h"
#include <QString>
#include <QDataStream>

//-------------------------------------------------------------------------------------------------
InformationDataProc::InformationDataProc() :
  _iTrainingDaysCount(1)
{
}
//-------------------------------------------------------------------------------------------------
InformationDataProc::~InformationDataProc()
{
}
//-------------------------------------------------------------------------------------------------
void InformationDataProc::vRead(QDataStream& rInStream, int iFileSize)
{
  _mapTrainingValue.clear();

  // Количество записанных тренировочных дней (структур TrainingProgress) в бинарнике
  int iDaysCount = iFileSize / static_cast<int>(sizeof (_oTrainingProgress));

  // Побитово содержит в себе 4 слова (см. _oTrainingProgress)
  unsigned short usProgress = 0;
  // Подтягивания
  short sPullUp = 0;
  // Отжимания от пола
  short sPushUp = 0;
  // Приседания
  short sQuat = 0;

  for (int iDay = 0; iDay < iDaysCount; iDay++) {
    rInStream >> usProgress;
    QVector<short>& rvecRow = _mapTrainingValue[iDay];
    short sProgressPullUp = 0;
    short sProgressPushUp = 0;
    short sProgressQuat = 0;
    short sHarder = 0;
    std::bitset<16> bitProgress(usProgress);
    std::bitset<5> bitProgressPullUp;
    std::bitset<5> bitProgressPushUp;
    std::bitset<5> bitProgressQuat;
    for (unsigned long long i = 0; i < 5; i++) {
      bitProgressPullUp[i] = bitProgress[i];
      bitProgressPushUp[i] = bitProgress[i + 5];
      bitProgressQuat[i] = bitProgress[i + 10];
    }
    sProgressPullUp = static_cast<short>(bitProgressPullUp.to_ulong());
    sProgressPushUp = static_cast<short>(bitProgressPushUp.to_ulong());
    sProgressQuat = static_cast<short>(bitProgressQuat.to_ulong());
    if (bitProgress[4]) { // Проверка старшего бита слова на то, что это отриц. значение
      bitProgressPullUp.flip(); // Инвертирование битов
      sProgressPullUp = -(static_cast<short>(bitProgressPullUp.to_ulong()) + 1); // Преобразование в отриц. значение
    }
    if (bitProgress[9]) {
      bitProgressPushUp.flip();
      sProgressPushUp = -(static_cast<short>(bitProgressPushUp.to_ulong()) + 1);
    }
    if (bitProgress[14]) {
      bitProgressQuat.flip();
      sProgressQuat = -(static_cast<short>(bitProgressQuat.to_ulong()) + 1);
    }
    sHarder = static_cast<short>(bitProgress[15]);
    rvecRow.push_back(sProgressPullUp);
    rvecRow.push_back(sProgressPushUp);
    rvecRow.push_back(sProgressQuat);
    rvecRow.push_back(sHarder);

    rInStream >> sPullUp;
    rvecRow.push_back(sPullUp);

    rInStream >> sPushUp;
    rvecRow.push_back(sPushUp);

    rInStream >> sQuat;
    rvecRow.push_back(sQuat);
  }
}
//-------------------------------------------------------------------------------------------------
void InformationDataProc::vSaveNewVal(QDataStream& rOutStream)
{
  for (int iTrainingDay = 0; iTrainingDay < _iTrainingDaysCount; iTrainingDay++) {
    QVector<short>& rvecRow = _mapTrainingValue[iTrainingDay];
    auto it = rvecRow.begin();
    std::bitset<5> bitProgressPullUp(static_cast<unsigned long long>(*it));
    vTansformToNegBinary(bitProgressPullUp, *it);
    it++;
    std::bitset<5> bitProgressPushUp(static_cast<unsigned long long>(*it));
    vTansformToNegBinary(bitProgressPushUp, *it);
    it++;
    std::bitset<5> bitProgressQuat(static_cast<unsigned long long>(*it));
    vTansformToNegBinary(bitProgressQuat, *it);
    it++;
    std::bitset<16> bitProgress;
    for (unsigned long long i = 0; i < 5; i++) {
      bitProgress[i] = bitProgressPullUp[i];
      bitProgress[i + 5] = bitProgressPushUp[i];
      bitProgress[i + 10] = bitProgressQuat[i];
    }
    bitProgress[15] = static_cast<unsigned long long>(*it);
    rOutStream << static_cast<unsigned short>(bitProgress.to_ulong());

    it++;
    short sProgressPullUp = *it;
    it++;
    short sProgressPushUp = *it;
    it++;
    short sProgressQuat = *it;

    rOutStream << sProgressPullUp;
    rOutStream << sProgressPushUp;
    rOutStream << sProgressQuat;
  }
}
//-------------------------------------------------------------------------------------------------
void InformationDataProc::vTansformToNegBinary(std::bitset<5>& rbitset, short sValue)
{
  if (sValue < 0) { // Если значение отрицательно
    sValue = -sValue;
    sValue--;
    rbitset = static_cast<unsigned long long>(sValue);
    rbitset.flip(); // Инвертирование битов
  }
}
//-------------------------------------------------------------------------------------------------
