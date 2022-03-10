#include "video.h"
#include <QtCore>

video::video(QObject *parent=Q_NULLPTR):QThread(parent)
{
 NumberFrames=1;
 FrameActual=0;
 pause=true;
 speed=1.0;
}

void video::run()
{
 while(1)
 {
  QMutex mutex;
  mutex.lock();
  if(this->pause)break;
  mutex.unlock();

  FrameActual=(FrameActual+1)%NumberFrames;
  emit SetFrame(FrameActual);
  this->msleep(100*speed);
 }
}

