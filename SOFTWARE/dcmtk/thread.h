#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>

class thread : public QObject
{
    Q_OBJECT
public:
    explicit thread(QObject *parent = 0);

signals:
    void progress(int);


public slots:
    void make_progress(int);
};

#endif // THREAD_H
