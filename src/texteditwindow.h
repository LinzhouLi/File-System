#ifndef TEXTEDITWINDOW_H
#define TEXTEDITWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class TextEditWindow;
}

class TextEditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextEditWindow(QWidget *parent, QString name, QString* str);
    ~TextEditWindow();

private:
    QString* content, text;
    Ui::TextEditWindow *ui;

signals:
    void QuitEditor();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // TEXTEDITWINDOW_H
