#include "loading.h"
#include "ui_loading.h"

loading::loading(QWidget *parent,int NumberFrames) :
    QWidget(parent),
    ui(new Ui::loading)
{
    ui->setupUi(this);
    ui->progressBar->setMaximum(NumberFrames);


}

loading::~loading()
{
    delete ui;
}

void loading::progress(int v)
{
   ui->progressBar->setValue(v);
}
