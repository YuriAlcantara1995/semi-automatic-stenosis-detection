#ifndef LOADING_H
#define LOADING_H

#include <QWidget>

namespace Ui {
class loading;
}

class loading : public QWidget
{
    Q_OBJECT

public:
    explicit loading(QWidget *parent = 0, int NumberFrames=0);
    ~loading();

public slots:
    void progress(int);


private:
    Ui::loading *ui;
};

#endif // LOADING_H
