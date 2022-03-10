#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QThread>
#include <QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <iostream>
#include <QScrollArea>
#include <QScrollBar>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QMouseEvent>
#include <QGridLayout>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>
#include <QSlider>
#include <queue>
#include <algorithm>
#include <QColor>
#include <QImage>
#include <QWidget>
#include <QActionGroup>
#include <QLineEdit>
#include <QLayout>
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmimage/dicoimg.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include "loading.h"
#include "video.h"
#include <QTabWidget>
#include <QGroupBox>
#include <QRadioButton>
using namespace std;

typedef pair<int,int>par;

namespace Ui {
class MainWindow;
}

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class SubQLabel;
class BrushInterface;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void open();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void CalibrarActivo();
    void AnalizarActivo();
    void ImagenSiguiente();
    void ImagenAnterior();
    void Play_Pause();
    void GoToFrame();
    void FrameChanged(int);
    void TabChanged(int);
    void MasVelocidad();
    void MenosVelocidad();
    void CalibracionAceptada();
    void setPixelSpacing(double,double);
    void setDRef(double);
    void setEstenosis(double);

signals:
     void scaleFactorChanged(const double& );
     void SignalSetFrameActual(int);
     void SignalSetPause(bool);
private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    void updateActions();
    void update();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    SubQLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;
    QGridLayout *mainLayout, *VideoControl , *CalibrarLayout, *AnalizarLayout, *RadioButtonsLayout;
    QGridLayout *FrameNumber, *ResultsLayout;
    QSlider *Frames;
    QPushButton *Anterior, *Siguiente, *PlayPause, *AceptarAnalisis, *CancelarAnalisis, *AceptarCalibracion,
                *MasSpeed, *MenosSpeed;

    QLabel *Frame, *CateterD, *FrameRate;
    QLineEdit *FrameActual, *FrameRateActual,*SpacingX,*SpacingY,*DRef,*Estenosis;
    QImage *imagenes;
    loading *Loading;
    QAction *openAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *aboutQtAct;
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
    video *MyVideo;
    QTabWidget *Tab;
    QGroupBox *Resultados, *CalibrarBox, *AnalizarBox, *RadioButtons;
    QComboBox *Cateter;
    QRadioButton *estenosis,*calcium;
};

#endif // MAINWINDOW_H

class SubQLabel : public QLabel
{
   Q_OBJECT

public:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void setScaleFactor(double factor);
    double PixelSpacingX=0;
    double PixelSpacingY=0;


signals:
   void PixelSpacingReady(double,double);
   void DRefReady(double);
   void EstenosisReady(double);

public slots:
   void setFactor(const double &factor);
   double Medicion1(par,int,int *,int *,int,int,int);

private:
     double scaleFactor;
     par LastPoint;

//public:
    //SubQPixmap();

};

