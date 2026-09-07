#include "QTableWidgetExt.h"
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QLabel>
#include "QEditDlg.h"
#include "InformationDataProc.h"

//-------------------------------------------------------------------------------------------------
QTableWidgetExt::QTableWidgetExt():
  _bTableIsChanged(false),
  _pMenu(new QMenu),
  _poTableItem(nullptr),
  _poInfoDataProc(new InformationDataProc()),
  _poEditDlg(nullptr),
  _iValue(0),
  _bLimitRange(false)
{
  _poActionEditWord = new QAction("Редактировать");
  _pMenu->addAction(_poActionEditWord);
  connect(_poActionEditWord, &QAction::triggered, this, &QTableWidgetExt::slotEditDlgShow);
}
//-------------------------------------------------------------------------------------------------
QTableWidgetExt::~QTableWidgetExt()
{
  delete _poActionEditWord;
  delete _pMenu;
  delete _poInfoDataProc;
}
//-------------------------------------------------------------------------------------------------
void QTableWidgetExt::vCreateTable()
{
  clear();
  const QMap<int, QVector<short>>& crmapData = _poInfoDataProc->crmapGetValue();
  int iRowCount = crmapData.size();
  int iColCount = crmapData[0].size();
  setRowCount(iRowCount);
  setColumnCount(iColCount);
  showGrid();
  horizontalHeader()->setVisible(true);
  horizontalHeader()->setStretchLastSection(true);
  verticalHeader()->setVisible(true);
  const QStringList clistColNames {"Прогресс подтягиваний\n от -16 до 15",
                                   "Прогресс отжиманий\n от -16 до 15",
                                   "Прогресс приседаний\n от -16 до 15",
                                   "Тяжелее предыдущего раза?\n(1 - Да/ 0 - Нет)",
                                   "Кол-во подтягиваний", "Кол-во отжиманий", "Кол-во приседаний"};
  setHorizontalHeaderLabels(clistColNames);

  // Ширина столбца
  int iColWidth = 180;
  int iRowNum = 0;
  foreach (auto itData, crmapData) {
    int iColNum = 0;
    // Вектор значений данных
    QVector<short>& rvecData = itData;
    for (int iVarNum = 0; iVarNum < rvecData.size(); iVarNum++) {
      // Создание итема для записи
      QTableWidgetItem* poTableWidgetItem = new QTableWidgetItem();
      // Расположение текста по центру
      poTableWidgetItem->setTextAlignment(Qt::AlignCenter);
      poTableWidgetItem->setText(QString::number(rvecData.at(iVarNum)));
      // Запись в нужное место таблицы
      setItem(iRowNum, iColNum, poTableWidgetItem);
      setColumnWidth(iColNum, iColWidth);
      iColNum++;
    }
    // Создание итема для вертикального заголовка
    QTableWidgetItem* poVerticalHeaderItem = new QTableWidgetItem();
    poVerticalHeaderItem->setText(_strlistDays.at(iRowNum));
    setVerticalHeaderItem(iRowNum, poVerticalHeaderItem);
    iRowNum++;
  }

  // Количество пустых столбцов для заполнения
  int iEmptyColCount = (width() - iColCount * iColWidth) / iColWidth;
  setColumnCount(iColCount + iEmptyColCount);

  // Номер, с которого начинаются пустые столбцы
  int iColEmptyNum = iColCount;
  // Заполнение горизонтального заголовка пустыми item
  for (int iColNum = 0; iColNum < iEmptyColCount; iColNum++) {
    setColumnWidth(iColEmptyNum, iColWidth);
    QTableWidgetItem* poHorizontalHeaderItem = new QTableWidgetItem();
    poHorizontalHeaderItem->setText("");
    setHorizontalHeaderItem(iColEmptyNum, poHorizontalHeaderItem);
    QTableWidgetItem* poTableWidgetItem = new QTableWidgetItem();
    poHorizontalHeaderItem->setText("");
    setItem(0, iColEmptyNum, poTableWidgetItem);
    iColEmptyNum++;
  }

  // Количество дней
  int iDaysCount = iRowCount;
  // Высота первой строки с данными
  int iRowHeight = rowHeight(0);
  // Высота таблицы
  int iTableHeight = height();
  // Количество пустых строк для заполнения
  int iEmptyRowCount = (iTableHeight - iDaysCount * iRowHeight) / iRowHeight;
  setRowCount(iRowCount + iEmptyRowCount);

  // Номер, с которого начинаются пустые строки
  int iRowEmptyNum = iRowCount;
  // Заполнение вертикального заголовка пустыми item
  for (int iRowNum = 0; iRowNum < iEmptyRowCount; iRowNum++) {
    QTableWidgetItem* poVerticalHeaderItem = new QTableWidgetItem();
    poVerticalHeaderItem->setText("");
    setVerticalHeaderItem(iRowEmptyNum, poVerticalHeaderItem);
    iRowEmptyNum++;
  }
}
//-------------------------------------------------------------------------------------------------
void QTableWidgetExt::contextMenuEvent(QContextMenuEvent* event)
{
  // Ячейка с данными, в которой вызвано контекстное меню
  _poTableItem = itemAt(viewport()->mapFromGlobal(event->globalPos()));

  if (_poTableItem != nullptr) {
    // Данные ячейки
    QString strData = _poTableItem->text();
    // Номер строки
    int iRowNum = _poTableItem->row();
    // Номер столбца
    int iColNum = _poTableItem->column();

    if (strData != "") {
      _iValue = _poInfoDataProc->iGetValue(iRowNum, iColNum);
      // В этих столбцах слова по 5 бит, являющиеся частями составного слова в 2 байта
      if (iColNum < 3) {
        _bLimitRange = true;
      }
      _pMenu->exec(event->globalPos());
    }
  }
  QTableWidget::contextMenuEvent(event);
}
//-------------------------------------------------------------------------------------------------
void QTableWidgetExt::vSetDays(const QStringList& strlistDays) {
  _strlistDays = strlistDays;
  _poInfoDataProc->vSetCountDays(strlistDays.size());
}
//-------------------------------------------------------------------------------------------------
void QTableWidgetExt::slotEditDlgShow()
{
  _poEditDlg = new QEditDlg(_iValue, _bLimitRange, this);
  connect(_poEditDlg, &QEditDlg::sigSendNewVal, this, &QTableWidgetExt::slotSetNewVal);
  _poEditDlg->exec();
  _bLimitRange = false;
  delete _poEditDlg;
  _poEditDlg = nullptr;
}
//-------------------------------------------------------------------------------------------------
void QTableWidgetExt::slotSetNewVal(short sNewVal)
{
  QString strNewVal = QString::number(sNewVal);
  _poTableItem->setText(strNewVal);
  _poInfoDataProc->vSaveVal(_poTableItem->row(), _poTableItem->column(), sNewVal);
  if (!_bTableIsChanged) {
    emit sigTableChanged();
    _bTableIsChanged = true;
  }
}
//-------------------------------------------------------------------------------------------------
