#include "texteditwindow.h"
#include "ui_texteditwindow.h"
#include <QMessageBox>

TextEditWindow::TextEditWindow(QWidget *parent, QString name, QString* str) :
    QMainWindow(parent),
    ui(new Ui::TextEditWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(name);
    this->setWindowIcon(QIcon(":/images/editor.ico"));

    this->content = str;
    this->text = *str;

    ui->textEdit->insertPlainText(text);
    ui->textEdit->document()->setModified(false); // 设为没被修改
}

TextEditWindow::~TextEditWindow()
{
    delete ui;
}

void TextEditWindow::closeEvent(QCloseEvent *event)
{
    //当文档内容被修改时.
    if (ui->textEdit->document()->isModified())
    {
        //跳出信息框，你是否要关闭.
        auto temp = QMessageBox::information(this, "File changed", QString::fromLocal8Bit("Save this file?"), QMessageBox::Yes | QMessageBox::No);
        if (temp == QMessageBox::Yes) // 确认保存
            *content = ui->textEdit->toPlainText();
    }
    emit QuitEditor();
    event->accept();
    return;
}
