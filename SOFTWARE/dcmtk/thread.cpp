#include "thread.h"

thread::thread(QObject *parent) : QObject(parent)
{

}

void thread::make_progress(int v)
{
 connect(this,SIGNAL(progress(int)),)
 emit progress(v);

}
