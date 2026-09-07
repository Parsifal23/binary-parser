#include "QEditDlg.h"
#include "ui_QEditDlg.h"

//-------------------------------------------------------------------------------------------------
QEditDlg::QEditDlg(int iValue, bool bLimitRange, QWidget* parent) :
  QDialog(parent),
  ui(new Ui::QEditDlg)
{
  ui->setupUi(this);
  ui->poSpBoxEditValue->setButtonSymbols(QAbstractSpinBox::NoButtons);
  ui->poLblCurValue->setText(QString::number(iValue));
  if (bLimitRange) {
    ui->poSpBoxEditValue->setRange(-16, 15); // 5 бит
  }
  else {
    ui->poSpBoxEditValue->setRange(-32768, 32767); // short
  }
  ui->poSpBoxEditValue->setValue(iValue);

  connect(ui->poTBtnCancel, &QToolButton::clicked, this, &QEditDlg::reject);
  connect(ui->poTBtnSave, &QToolButton::clicked, this, &QEditDlg::slotSaveChanges);
}
//-------------------------------------------------------------------------------------------------
QEditDlg::~QEditDlg()
{
  delete ui;
}
//-------------------------------------------------------------------------------------------------
void QEditDlg::slotSaveChanges()
{
  if (ui->poLblCurValue->text() != ui->poSpBoxEditValue->text()) {
    emit sigSendNewVal(static_cast<short>(ui->poSpBoxEditValue->value()));
  }
  accept();
}
//-------------------------------------------------------------------------------------------------
