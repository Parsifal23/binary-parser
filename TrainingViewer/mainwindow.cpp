#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QHeaderView>

#include "QTableWidgetExt.h"
#include "InformationDataProc.h"

static const char* sccpSaveAs = "Сохранить как";
static const char* sccpCSV = ".csv";
static const char* sccpDat = ".dat";
static const char* sccpChooseRegime = "Выбрать режим";
const int ciTrainingSize = 8; // 8 байт - один план тренировки в 1 дне (TrainingProgress в InformationDataProc)

//-------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  _poTableWidgetExt(new QTableWidgetExt()),
  _iFileSize(0)
{
  ui->setupUi(this);
  vInitComponents();
  this->showMaximized();
}
//-------------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
  delete _poTableWidgetExt;
  delete ui;
}
//-------------------------------------------------------------------------------------------------
void MainWindow::vInitComponents()
{
  ui->verticalLayout->addWidget(_poTableWidgetExt);
  _poTableWidgetExt->setFrameShape(QFrame::StyledPanel);
  _poTableWidgetExt->setFrameShadow(QFrame::Sunken);
  _poTableWidgetExt->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
  _poTableWidgetExt->setEditTriggers(QAbstractItemView::NoEditTriggers);
  _poTableWidgetExt->setSelectionMode(QAbstractItemView::NoSelection);
  _poTableWidgetExt->setAlternatingRowColors(true);
  _poTableWidgetExt->setCornerButtonEnabled(false);

  // Отключение редактирования ячеек
  _poTableWidgetExt->setEditTriggers(QAbstractItemView::NoEditTriggers);
  // Установка растягивания последнего столбца заголовка до границы таблицы
  _poTableWidgetExt->verticalHeader()->setStretchLastSection(true);
  // Отключение работы кнопки Сохранить как
  ui->poBoxSaveAs->setEnabled(false);
  ui->poBoxOpenFile->setEnabled(false);
  ui->polblCurRegime->setHidden(true);
  // Размер шрифта
  QFont oFontBtn;
  oFontBtn.setPointSize(10);
  ui->poBoxSaveAs->setFont(oFontBtn);
  ui->poBoxOpenFile->setFont(oFontBtn);
  ui->poBoxRegime->setFont(oFontBtn);
  ui->polblCurRegime->setFont(oFontBtn);

  ui->poBoxSaveAs->addItem(sccpSaveAs);
  ui->poBoxSaveAs->addItem(sccpCSV);
  ui->poBoxSaveAs->addItem(sccpDat);
  ui->poBoxSaveAs->setCurrentText(sccpSaveAs);

  ui->poBoxRegime->addItem(sccpChooseRegime);

  // Читаем из конфигурационного файла варианты режимов тренировок для выбора количества дней
  QFile oFile(":/Config.json");
  if (oFile.exists() && oFile.open(QIODevice::ReadOnly)) {
    QJsonArray ojarrSets;
    QByteArray FileData = oFile.readAll();
    QJsonDocument oJsonDoc(QJsonDocument::fromJson(FileData));
    ojarrSets = oJsonDoc.array();
    for (auto it(ojarrSets.begin()); it != ojarrSets.end(); it++) {
      QJsonObject ojSet = (*it).toObject();
      QString strRegime = ojSet.value("Режим").toString();
      ui->poBoxRegime->addItem(strRegime);
    }
    oFile.close();
  }

  ui->poBoxRegime->setCurrentText(sccpChooseRegime);

  connect(ui->poBoxOpenFile, &QToolButton::clicked,
          this,              &MainWindow::slotOpenFile);
  connect(ui->poBoxSaveAs, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
          this,            &MainWindow::slotSaveAs);
  connect(ui->poBoxRegime, QOverload<const QString&>::of(&QComboBox::currentIndexChanged),
          this,            &MainWindow::slotChooseRegime);
  connect(_poTableWidgetExt, &QTableWidgetExt::sigTableChanged, this, &MainWindow::slotTitleChanged);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::slotOpenFile()
{
  // Путь к последней открытой папке
  const QString cstrResentDirPath = bLoadFilePath() ? _strLastFilePath : QString("\\");
  // Путь к бинарному файлу
  const QString cstrFilePath = QFileDialog::getOpenFileName(this, "Открыть бинарный файл",
                                                            cstrResentDirPath, "*.dat");
  // Если нажата кнопка Отмена
  if (cstrFilePath != "") {
    vSaveFilePath(cstrFilePath);

    QFile oFile(cstrFilePath);
    // Размер файла в байтах
    _iFileSize = static_cast<int>(oFile.size());
    if (_iFileSize) {
      if (oFile.open(QIODevice::ReadOnly)) {
        _poTableWidgetExt->_bTableIsChanged = false;
        QDataStream InStream(&oFile);

        if ((_iFileSize/ciTrainingSize) > _poTableWidgetExt->crstrlistGetDays().size()) {
          QMessageBox::warning(this,
                               "Ошибка!",
                               "Количество дней из Config.json\nменьше количества оных в бинарнике.",
                               QMessageBox::Ok);
        }
        else {
          InformationDataProc* poInfoDataProc = _poTableWidgetExt->cpGetInfoDataProc();
          poInfoDataProc->vRead(InStream, _iFileSize);
          _poTableWidgetExt->vCreateTable();
          ui->poBoxSaveAs->setEnabled(true);
        }
        oFile.close();
        ui->poBoxSaveAs->setEnabled(true);
        setWindowTitle("TrainingViewer " + cstrFilePath);
      }
      else {
        QMessageBox::warning(this,
                             "Ошибка!",
                             "Невозможно открыть файл.",
                             QMessageBox::Ok);
      }
    }
    else {
      QMessageBox::warning(this,
                           "Ошибка!",
                           "Файл пуст.",
                           QMessageBox::Ok);
    }
  }
}
//-------------------------------------------------------------------------------------------------
bool MainWindow::bLoadFilePath()
{
  bool bIsJsonOpen = false;
  // Путь к json, хранящему последний открытый путь
  QString strFilePath = QApplication::applicationDirPath() + "/" + _cstrJsonFile;
  QFile oFile(strFilePath);
  if (oFile.exists() && oFile.open(QIODevice::ReadOnly)) {
    // Считанный файл записей
    QJsonDocument oJsonDoc(QJsonDocument::fromJson(oFile.readAll()));
    // Ссылка на массив записей
    const QJsonArray& croJsonArray = oJsonDoc.array();
    if (!croJsonArray.empty()) { // Есть ли элемент в массиве, т.е. путь к бинарнику
      _strLastFilePath = croJsonArray[0].toString();
      bIsJsonOpen = true;
    }
    oFile.close();
  }
  return bIsJsonOpen;
}
//-------------------------------------------------------------------------------------------------
void MainWindow::vSaveFilePath(const QString& strFilePath)
{
  _strLastFilePath = strFilePath;
  // Путь к файлу
  QString strJsonPath = QApplication::applicationDirPath() + "/" + _cstrJsonFile;
  QFile oFile(strJsonPath);
  if (oFile.open(QIODevice::WriteOnly)) {
    // Массив записей
    QJsonArray oJsonArray;
    oJsonArray.push_back(strFilePath);
    // Файл записей
    QJsonDocument JsonDoc;
    JsonDoc.setArray(oJsonArray);
    oFile.write(JsonDoc.toJson());
    oFile.close();
  }
}
//-------------------------------------------------------------------------------------------------
void MainWindow::slotSaveAs(const QString& strText)
{
  if (strText == sccpCSV) {
    // Фактический путь к сохранённому файлу
    const QString cstrFilePath = QFileDialog::getSaveFileName(this, "Сохранить в .csv",
                                                              QString("\\"), "*.csv");

    QFile oFile(cstrFilePath);
    if (oFile.open(QIODevice::WriteOnly)) {
      // Поток для записи данных
      QTextStream out(&oFile);

      // Буфер текста для записи в csv
      QStringList strListData;
      // Модель, через которую легко получить доступ к данным таблицы
      QAbstractItemModel* poItemModel = _poTableWidgetExt->model();
      // Количество строк
      int iRowCount = poItemModel->rowCount();
      // Количество столбцов
      int iColCount = poItemModel->columnCount();
      strListData << "\"\"";
      for (int iColNum = 0; iColNum < iColCount; iColNum++) {
        // Получение и запись заголовка каждого столбца
        strListData << "\"" + poItemModel->headerData(iColNum, Qt::Horizontal).toString() + "\"";
      }
      // Запись в csv
      out << strListData.join(";") + "\n";
      strListData.clear();
      for (int iRowNum = 0; iRowNum < iRowCount; iRowNum++) {
        strListData.clear();
        // Получение и запись заголовка строки
        strListData << "\"" + poItemModel->headerData(iRowNum, Qt::Vertical).toString() + "\"";
        for (int iColNum = 0; iColNum < iColCount; iColNum++) {
          strListData << "\"" + poItemModel->data(poItemModel->index(iRowNum, iColNum),
                                                  Qt::DisplayRole).toString() + "\"";
        }
        out << strListData.join(";") + "\n";
      }
      QMessageBox::information(_poTableWidgetExt,
                               "",
                               "Успешно записано!",
                               QMessageBox::Ok);
      oFile.close();
    }
  }
  else {
    // Фактический путь к сохранённому файлу
    const QString cstrFilePath = QFileDialog::getSaveFileName(this, "Сохранить в .dat",
                                                              QString("\\"), "*.dat");
    QFile oFile(cstrFilePath);
    if (oFile.open(QIODevice::WriteOnly)) {
      QDataStream OutStream(&oFile);

      InformationDataProc* poInfoDataProc = _poTableWidgetExt->cpGetInfoDataProc();
      poInfoDataProc->vSaveNewVal(OutStream);
      QMessageBox::information(_poTableWidgetExt,
                               "",
                               "Успешно записано!",
                               QMessageBox::Ok);
      oFile.close();
    }
  }
  ui->poBoxSaveAs->blockSignals(true);
  ui->poBoxSaveAs->setCurrentText(sccpSaveAs);
  ui->poBoxSaveAs->blockSignals(false);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::slotTitleChanged()
{
  QString strTitle = windowTitle();
  strTitle.append("*");
  setWindowTitle(strTitle);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::slotChooseRegime(const QString& strText)
{
  QFile oFile(":/Config.json");
  if (oFile.exists() && oFile.open(QIODevice::ReadOnly)) {
    QJsonArray ojarrSets;
    QByteArray FileData = oFile.readAll();
    QJsonDocument oJsonDoc(QJsonDocument::fromJson(FileData));
    ojarrSets = oJsonDoc.array();
    for (auto it(ojarrSets.begin()); it != ojarrSets.end(); it++) {
      QJsonObject ojSet = (*it).toObject();
      QString strRegime = ojSet.value("Режим").toString();
      if (strRegime == strText) {
        ui->polblCurRegime->setText("Режим " + strText);
        ui->polblCurRegime->setHidden(false);
        QJsonArray ojarrDays = ojSet.value("Дни").toArray();
        QStringList strlistDays;
        for (auto it(ojarrDays.begin()); it != ojarrDays.end(); it++) {
          strlistDays.push_back((*it).toString());
        }
        _poTableWidgetExt->vSetDays(strlistDays);
        ui->poBoxOpenFile->setEnabled(true);
        break;
      }
    }
    oFile.close();
  }
  ui->poBoxRegime->blockSignals(true);
  ui->poBoxRegime->setCurrentText(sccpChooseRegime);
  ui->poBoxRegime->blockSignals(false);
}
//-------------------------------------------------------------------------------------------------
