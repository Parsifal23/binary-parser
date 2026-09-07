#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /** @brief Конструктор */
  explicit MainWindow(QWidget* parent = nullptr);

  /** @brief Деструктор */
  ~MainWindow();

  /** @brief Инициализация компонентов */
  void vInitComponents();

  /** @brief Сохранение последнего пути открытия бинарника в json */
  void vSaveFilePath(const QString& strFilePath);

  /** @brief Загрузка последнего пути открытия бинарника из json */
  bool bLoadFilePath();

private slots:
  /** @brief Выбрать файл */
  void slotOpenFile();

  /** @brief Сохранить в файл csv */
  void slotSaveAs(const QString& strText);

  /** @brief Добавить * в названии окна при изменении ячейки таблицы */
  void slotTitleChanged();

  /** @brief Выбор режима тренировки из Config.json */
  void slotChooseRegime(const QString& strText);

private:
  /** @brief Указатель на форму */
  Ui::MainWindow* ui;

  /** @brief Json с хранением последнего пути к бинарнику */
  const QString _cstrJsonFile = "LastFilePath.json";

  /** @brief Путь, загруженный из json */
  QString _strLastFilePath;

  /** @brief Строка для понимания, какие данные сохранять в файл */
  QString _strDataSaveAs;

  /** @brief Указатель на таблицу-виджет */
  class QTableWidgetExt* _poTableWidgetExt;

  /** @brief Размер бинарника в байтах */
  int _iFileSize;
};
