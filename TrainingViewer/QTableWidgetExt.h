#pragma once

#include <QTableWidget>

/** @brief Класс доступа к контекстному меню для ячеек таблицы */
class QTableWidgetExt: public QTableWidget
{
  Q_OBJECT
public:
  /** @brief Конструктор */
  QTableWidgetExt();

  /** @brief Деструктор */
  ~QTableWidgetExt();

  /** @brief Заполнение таблицы данными */
  void vCreateTable();

  /** @brief Получить указатель на меню */
  const QTableWidgetItem* cpoGetTableItem() {
    return _poTableItem;
  }

  /** @brief Перегруженный вызов контекстного меню */
  void contextMenuEvent(QContextMenuEvent* event);

  /** @brief Обработка данных из бинарника и заполнение таблицы */
  class InformationDataProc* cpGetInfoDataProc() { return _poInfoDataProc; }

  /** @brief Сохраняет список Days из Config.json */
  void vSetDays(const QStringList& strlistDays);

  /** @brief Получение списка Days */
  const QStringList& crstrlistGetDays() { return _strlistDays; }

  /** @brief Флаг на то, что в названии главного окна уже добавлена "*" */
  bool _bTableIsChanged;

signals:
  /** @brief Сигнал для добавления * в названии главного окна при изменении ячейки таблицы */
  void sigTableChanged();

private slots:
  /** @brief Слот открытия окна редактирования слова */
  void slotEditDlgShow();

  /** @brief Слот изменения содержимого ячейки, когда было отредактировано значение */
  void slotSetNewVal(short sNewVal);

private:
  /** @brief Указатель на меню */
  QMenu* _pMenu;

  /** @brief Указатель на ячейку с данными, в которой вызвано контекстное меню */
  QTableWidgetItem* _poTableItem;

  /** @brief Указатель на пункт контекстного меню */
  QAction* _poActionEditWord;

  /** @brief Указатель на класс работы с прочитанными данными из файла */
  class InformationDataProc* _poInfoDataProc;

  /** @brief Указатель на диалоговое окно редактирования значения */
  class QEditDlg* _poEditDlg;

  /** @brief Код поправки выбранной ячейки*/
  int _iValue;

  /** @brief Флаг, указывающий на необходимость установки специального диапазона значений в поле
   * изменения значения в диалоговом окне */
  bool _bLimitRange;

  /** @brief Список дней */
  QStringList _strlistDays;
};
