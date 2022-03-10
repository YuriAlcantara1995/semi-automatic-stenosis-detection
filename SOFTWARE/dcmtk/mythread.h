#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QThread>

class Mythread : public QObject
{
    Q_OBJECT
public:
    explicit Mythread(QObject *parent = 0);
    void start(bool);

signals:
    void progress(int);


public slots:
    void make_progress(int);
};

#endif // MYTHREAD_H
