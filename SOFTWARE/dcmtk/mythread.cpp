#include "mythread.h"

Mythread::Mythread(QObject *parent) : QObject(parent)
{

}

void Mythread::start(bool pausa)
{
 pausa=0;
}

void Mythread::make_progress(int v)
{
 emit progress(v);
}
