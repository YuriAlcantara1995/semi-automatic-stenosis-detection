#ifndef VIDEO_H
#define VIDEO_H
#include <QThread>


class video : public QThread
{
  Q_OBJECT

public:
   explicit   video(QObject *);
   bool pause;
   void run();
   int FrameActual;
   int NumberFrames;
   double speed;

signals:
 void SetFrame(int);

public slots:

};

#endif // VIDEO_H
