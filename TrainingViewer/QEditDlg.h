#pragma once

#include <QDialog>

namespace Ui {
  class QEditDlg;
}
/** @brief Класс диалогового окна редактирования значений */
class QEditDlg : public QDialog
{
  Q_OBJECT

public:
  /** @brief Конструктор с передачей значений в поля диалогового окна */
  explicit QEditDlg(int iValue, bool bLimitRange, QWidget* parent = nullptr);

  /** @brief Деструктор */
  ~QEditDlg();

signals:
  /** @brief Отправка в TableWidget нового значения для сохранения в таблице */
  void sigSendNewVal(short sNewValue);

private slots:
  /** @brief Слот сохранения изменения значения */
  void slotSaveChanges();

private:
  /** @brief Указатель на форму */
  Ui::QEditDlg* ui;
};
