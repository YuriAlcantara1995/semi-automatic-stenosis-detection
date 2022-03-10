#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QIcon>
#include <QImageReader>
#include <complex>
#include <cmath>

const double EPS=1e-8;
const double oo=1e12;
const double PI=3.141592653589793;
#define X real()
#define Y imag()
typedef pair<int,int>par;
par Puntos[5];
par PSegmentacion[5005];
int CantidadPuntos;
double CatheterDiameterMM;

struct point
{
    double x, y; // only used if more precision is needed
    point()
    {
        x = y = 0.0;    // default constructor
    }
    point(double _x, double _y) : x(_x), y(_y) {}
    bool operator < (point other) const   // override less than operator
    {
        if (fabs(x - other.x) > EPS) // useful for sorting
            return x < other.x; // first criteria , by x-coordinate
        return y < other.y;
    }
    bool operator == (point other) const
    {
        return (fabs(x - other.x) < EPS && (fabs(y - other.y) < EPS));
    }
};

struct vec
{
    double x, y; // name: ‘vec’ is different from STL vector
    vec(double _x, double _y) : x(_x), y(_y) {}
};

vec toVec(point a, point b)   // convert 2 points to vector a->b
{
    return vec(b.x - a.x, b.y - a.y);
}

double dot(vec a, vec b)
{
    return (a.x * b.x + a.y * b.y);
}

double norm_sq(vec v)
{
    return v.x * v.x + v.y * v.y;
}


queue<par>cola;
int mx[]= {0,0,1,-1},
          my[]= {1,-1,0,0};
bool HerramientaActiva=0;//0 si calibrar, 1 si analizar;
int state=0;

typedef pair<double,double>parDouble;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QImageReader reader;
    QPixmap pixmap;
    QImage img;

    ui->setupUi(this);
    imageLabel = new SubQLabel;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    MyVideo = new video(this);
    connect(MyVideo,SIGNAL(SetFrame(int)),this,SLOT(FrameChanged(int)));
    MyVideo->start();


    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setMaximumWidth(960);


    Anterior = new QPushButton;
    Anterior->setMaximumWidth(30);
    Anterior->setMaximumHeight(30);
    reader.setFileName(":/rc/Prev.jpg");
    img=reader.read();
    pixmap=QPixmap::fromImage(img);
    QIcon Prev(pixmap);
    Anterior->setIcon(Prev);
    Anterior->setIconSize(pixmap.rect().size());

    Siguiente= new QPushButton;
    Siguiente->setMaximumWidth(30);
    Siguiente->setMaximumHeight(30);
    reader.setFileName(":/rc/Next.jpg");
    img=reader.read();
    pixmap=QPixmap::fromImage(img);
    QIcon Next(pixmap);
    Siguiente->setIcon(Next);
    Siguiente->setIconSize(pixmap.rect().size());

    PlayPause= new QPushButton;
    PlayPause->setMaximumWidth(30);
    PlayPause->setMaximumHeight(30);
    reader.setFileName(":/rc/Play.jpg");
    img=reader.read();
    pixmap=QPixmap::fromImage(img);
    QIcon Play(pixmap);
    PlayPause->setIcon(Play);
    PlayPause->setIconSize(pixmap.rect().size());

    MasSpeed = new QPushButton;
    MasSpeed->setMaximumWidth(30);
    MasSpeed->setMaximumHeight(30);
    reader.setFileName(":/rc/Faster.jpg");
    img=reader.read();
    pixmap=QPixmap::fromImage(img);
    QIcon Faster(pixmap);
    MasSpeed->setIcon(Faster);
    MasSpeed->setIconSize(pixmap.rect().size());

    MenosSpeed = new QPushButton;
    MenosSpeed->setMaximumWidth(30);
    MenosSpeed->setMaximumHeight(30);
    reader.setFileName(":/rc/Slower.jpg");
    img=reader.read();
    pixmap=QPixmap::fromImage(img);
    QIcon Slower(pixmap);
    MenosSpeed->setIcon(Slower);
    MenosSpeed->setIconSize(pixmap.rect().size());


    Anterior->setEnabled(false);
    Siguiente->setEnabled(false);
    PlayPause->setEnabled(false);
    MasSpeed->setEnabled(false);
    MenosSpeed->setEnabled(false);
    PlayPause->setShortcut(tr("Space"));
    Anterior->setShortcut(tr("Left"));
    Siguiente->setShortcut(tr("Right"));

    Frames=new QSlider(Qt::Horizontal);
    Frames->setEnabled(false);
    Frame=new QLabel("Frame: ");
    FrameRate=new QLabel("Frame Rate: ");
    FrameActual=new QLineEdit;
    FrameRateActual=new QLineEdit;
    FrameActual->setFixedWidth(40);
    FrameActual->setReadOnly(true);
    FrameRateActual->setFixedWidth(50);
    FrameRateActual->setReadOnly(true);


    FrameNumber = new QGridLayout;
    FrameNumber->addWidget(Frame,0,0);
    FrameNumber->addWidget(FrameActual,0,1);
    FrameNumber->addWidget(FrameRate,0,2);
    FrameNumber->addWidget(FrameRateActual,0,3);
    FrameNumber->setAlignment(FrameActual,Qt::AlignRight);
    FrameNumber->setAlignment(Frame,Qt::AlignLeft);
    FrameNumber->setAlignment(FrameRateActual,Qt::AlignLeft);
    FrameNumber->setAlignment(FrameRate,Qt::AlignRight);
    FrameNumber->setHorizontalSpacing(15);

    VideoControl = new QGridLayout;
    VideoControl->addWidget(Anterior,0,1);
    VideoControl->addWidget(PlayPause,0,2);
    VideoControl->addWidget(Siguiente,0,3);
    VideoControl->addWidget(MenosSpeed,0,0);
    VideoControl->addWidget(MasSpeed,0,4);
    VideoControl->addWidget(Frames,1,0,1,5);

    AceptarCalibracion = new QPushButton("Aceptar");
    AceptarCalibracion->setMinimumHeight(30);
    Cateter = new QComboBox;
    CateterD = new QLabel("Diametro del Cateter");
    SpacingX = new QLineEdit;
    SpacingY = new QLineEdit;
    SpacingX->setFixedWidth(50);
    SpacingX->setReadOnly(true);
    SpacingY->setFixedWidth(50);
    SpacingY->setReadOnly(true);

    CateterD->adjustSize();
    CatheterDiameterMM=2.0;
    char *txt=new char [20];

    for(int i=1; i<=8; i++)
    {
        sprintf(txt,"%d",i);
        QString aux(txt);
        Cateter->addItem(aux +" F");
    }
    CalibrarBox = new QGroupBox;
    CalibrarLayout = new QGridLayout;
    CalibrarLayout->addWidget(Cateter,1,0,1,4);
    CalibrarLayout->addWidget(CateterD,0,0,1,2);
    CalibrarLayout->addWidget(SpacingX,2,0,1,1);
    CalibrarLayout->addWidget(SpacingY,2,1,1,1);
    CalibrarLayout->addWidget(AceptarCalibracion,3,0,1,2);
    CalibrarLayout->setVerticalSpacing(10);
    CalibrarBox->setLayout(CalibrarLayout);

    AceptarAnalisis = new QPushButton("Aceptar");
    AceptarAnalisis->setMinimumHeight(30);
    CancelarAnalisis = new QPushButton("Cancelar");
    CancelarAnalisis->setMinimumHeight(30);
    estenosis = new QRadioButton("Estenosis");
    calcium = new QRadioButton("Calcificacion");
    RadioButtonsLayout = new QGridLayout;
    estenosis->setChecked(true);
    RadioButtonsLayout->addWidget(estenosis,0,0);
    RadioButtonsLayout->addWidget(calcium,1,0);
    RadioButtonsLayout->addWidget(AceptarAnalisis,2,0,1,1);
    RadioButtonsLayout->addWidget(CancelarAnalisis,2,1,1,1);

    RadioButtons = new QGroupBox;
    RadioButtons->setLayout(RadioButtonsLayout);

    Tab = new QTabWidget;
    Tab->addTab(RadioButtons,"Analizar");
    Tab->addTab(CalibrarBox,"Calibrar");
    Tab->adjustSize();
    Tab->setMaximumHeight(200);
    Tab->setCurrentIndex(1);

    Resultados = new QGroupBox("Resultados del Analisis");
    ResultsLayout = new QGridLayout;
    DRef = new QLineEdit;
    Estenosis = new QLineEdit;
    ResultsLayout->addWidget(DRef,0,0);
    ResultsLayout->addWidget(Estenosis,1,0);
    Resultados->setLayout(ResultsLayout);

    mainLayout = new QGridLayout;
    mainLayout->addWidget(scrollArea, 0,0,14,1);
    mainLayout->addWidget(Tab, 2,1,1,7);
    mainLayout->addWidget(Resultados,3,1,11,7);
    mainLayout->addLayout(VideoControl,0,1,1,7);
    mainLayout->addLayout(FrameNumber,1,1,1,1);
    mainLayout->setVerticalSpacing(10);





    ui->centralWidget->setLayout(mainLayout);
    int MW=maximumWidth();
    int MH=maximumHeight();
    resize(MW,MH);
    createActions();
    createMenus();

    setWindowTitle(tr("Imagis"));

}

QImage imagenActual,imagenOriginal,imageState3;
int posActual=0,N;
bool pausa=true;

void SubQLabel::setFactor(const double &factor)
{
    scaleFactor = factor;
}

int value(QRgb rgb)
{
    double GrayValue = double(0.299*qRed(rgb)) + double(0.587*qGreen(rgb)) + double(0.114*qBlue(rgb));
    return int(GrayValue);
}




void MainWindow::open()
{
//    QString fileName = QFileDialog::getOpenFileName(this,
//                                    tr("Abrir Archivo"), QDir::currentPath());
//    if (!fileName.isEmpty()) {
//        QImage image(fileName);
//        if (image.isNull()) {
//            QMessageBox::information(this, tr("Imagis"),
//                                     tr("No se pudo cargar %1.").arg(fileName));
//            return;
//        }
//        image.convertToFormat(QImage::Format_Grayscale8);


    QString strDir= QFileDialog::getOpenFileName(this,tr("Abrir Archivo"), QDir::currentPath());

    if(!strDir.isEmpty())
    {

        DcmFileFormat format;
        OFCondition status = format.loadFile(strDir.toStdString().c_str());
        DcmDataset *DS;
        DS=format.getDataset();

        if(status.good())
        {
            OFString ts;
            int height;
            if(DS->findAndGetOFString(DCM_Rows,ts).good())
                sscanf(ts.c_str(),"%d",&height);
            else
                printf("NO SE PUDO ACCEDER A LA ALTURA DE LA IMAGEN\n");

            int width;
            if(DS->findAndGetOFString(DCM_Rows,ts).good())
                sscanf(ts.c_str(),"%d",&width);
            else
                printf("NO SE PUDO ACCEDER AL ANCHO DE LA IMAGEN\n");

            int BitsStored;
            if(DS->findAndGetOFString(DCM_BitsStored,ts).good())
            {
                sscanf(ts.c_str(),"%d",&BitsStored);
            }
            else
                printf("NO SE PUDO ACCEDER A LA CANTIDAD DE BYTES POR PIXEL\n");

            DS->findAndGetOFString(DCM_SamplesPerPixel,ts);
            int SPP;
            sscanf(ts.c_str(),"%d",&SPP);

            DS->findAndGetOFString(DCM_FrameTime,ts);
            double framerate;
            sscanf(ts.c_str(),"%lf",&framerate);

            int numberFrames;
            if(DS->findAndGetOFString(DCM_NumberOfFrames,ts).good())
            {
                sscanf(ts.c_str(),"%d",&numberFrames);
            }
            else
                numberFrames=1;

            int BytePerPixel;

            switch(BitsStored*SPP)
            {
            case 8:
                BytePerPixel=1;
                break;
            case 12:
            case 16:
                BytePerPixel=2;
                break;
            case 24:
                BytePerPixel=3;
                break;
            case 32:
                BytePerPixel=4;
            }

            DicomImage img(&format,DS->getOriginalXfer());


            img.removeAllOverlays();

            //unsigned char *buff;
            //buff = new unsigned char[height*width*BytePerPixel+5];
            //img.getOutputData(buff,height*width*BytePerPixel,0,20);
            //QImage image=QImage(buff,width,height,QImage::Format_RGB16);

            int **aux;
            aux=new int*[height+1];
            for(int i=0; i<height; i++)
                aux[i]=new int[width];

            Anterior->setEnabled(true);
            Siguiente->setEnabled(true);
            PlayPause->setEnabled(true);
            Frames->setEnabled(true);
            MasSpeed->setEnabled(true);
            MenosSpeed->setEnabled(true);

            imagenes=new QImage[numberFrames+1];

            Loading= new loading(this,numberFrames);
            Loading->show();
            Loading->setFocus();
            Loading->setWindowTitle(tr("Espere por favor"));
            Loading->setGeometry(300,300,450,150);
            Loading->setAutoFillBackground(true);

            for(int p=0; p<numberFrames; p++)
            {
                Loading->progress(p);
                img.writeBMP("auxiliar.bmp",32,p);
                imagenes[p] = QImage("auxiliar.bmp");

//                if(!imagenes[p].allGray())
//                {
//                    for(int i=0; i<width; i++)
//                        for(int j=0; j<height; j++)
//                        {
//                            int gray=value(imagenes[p].pixel(i,j));
//                            imagenes[p].setPixel(i,j,qRgb(gray,gray,gray));
//                        }
//                }

                for(int i=0; i<width; i++)
                    for(int j=0; j<height; j++)
                        aux[i][j]=value(imagenes[p].pixel(i,j));        ;

                for(int i=1; i<width-1; i++)
                    for(int j=1; j<height-1; j++)
                    {
                        int newValue=( 4*aux[i][j]
                                       +2*aux[i-1][j]
                                       +2*aux[i][j-1]
                                       +2*aux[i+1][j]
                                       +2*aux[i][j+1]
                                       +aux[i-1][j-1]
                                       +aux[i+1][j+1]
                                       +aux[i-1][j+1]
                                       +aux[i+1][j-1] )/16;

                        imagenes[p].setPixel(i,j,qRgb(newValue,newValue,newValue));
                    }
            }



            Loading->close();
            posActual=0;
            N=numberFrames;
            MyVideo->NumberFrames=numberFrames;
            MyVideo->pause=true;
            MyVideo->FrameActual=0;
            MyVideo->speed=1.0;
            Frames->setMaximum(N-1);
            Frames->setValue(0);
            Frames->setTracking(1);
            Frames->setTickPosition(QSlider::TicksBelow);
            double delay=100*MyVideo->speed;
            int fps=int(1000/delay);
            char *txt=new char [20];
            sprintf(txt,"%d",fps);
            int l=(int)strlen(txt);
            txt[l]=' ';
            txt[l+1]='f';
            txt[l+2]='p';
            txt[l+3]='s';
            txt[l+4]='\0';
            FrameRateActual->setText(txt);
            update();
            scaleFactor = 1.0;

            updateActions();
            normalSize();
            emit scaleFactorChanged(scaleFactor);
        }
        else
            QMessageBox::information(this, tr("Imagis"),tr("No se pudo cargar %1.").arg(strDir));
    }
    else
    {
        QMessageBox::information(this, tr("Imagis"),tr("No se pudo cargar %1.").arg(strDir));
    }
}

void MainWindow::GoToFrame()
{
    QImage img(":/rc/Play.jpg");
    QPixmap pixmap=QPixmap::fromImage(img);
    QIcon Play(pixmap);
    PlayPause->setIcon(Play);
    PlayPause->setIconSize(pixmap.rect().size());

    MyVideo->pause=true;
    posActual=Frames->value();
    update();
}

void MainWindow::MenosVelocidad()
{
    if(MyVideo->speed<3)
        MyVideo->speed*=1.10;
    double delay=100*MyVideo->speed;
    int fps=int(1000/delay);
    char *txt=new char [20];
    sprintf(txt,"%d",fps);
    int l=(int)strlen(txt);
    txt[l]=' ';
    txt[l+1]='f';
    txt[l+2]='p';
    txt[l+3]='s';
    txt[l+4]='\0';
    FrameRateActual->setText(txt);
}

void MainWindow::MasVelocidad()
{
    if(MyVideo->speed>0.3)
        MyVideo->speed*=0.90;
    double delay=100*MyVideo->speed;
    int fps=int(1000/delay);
    char *txt=new char [20];
    sprintf(txt,"%d",fps);
    int l=(int)strlen(txt);
    txt[l]=' ';
    txt[l+1]='f';
    txt[l+2]='p';
    txt[l+3]='s';
    txt[l+4]='\0';
    FrameRateActual->setText(txt);
}


void MainWindow::FrameChanged(int F)
{
    imageLabel->setPixmap(QPixmap::fromImage(imagenes[F]));
    char *txt=new char [20];
    sprintf(txt,"%d",F);
    FrameActual->setText(txt);
}

void MainWindow::update()
{
    imagenOriginal=imagenes[posActual];
    imagenActual=imagenOriginal;
    imageLabel->setPixmap(QPixmap::fromImage(imagenActual));
    char *txt=new char [20];
    sprintf(txt,"%d",posActual);
    FrameActual->setText(txt);
    Frames->setValue(posActual);
    state=0;
}

void MainWindow::Play_Pause()
{
    if(MyVideo->pause)
    {
        QImage img(":/rc/Pause.jpg");
        QPixmap pixmap=QPixmap::fromImage(img);
        QIcon Pause(pixmap);
        PlayPause->setIcon(Pause);
        PlayPause->setIconSize(pixmap.rect().size());


        MyVideo->pause=false;
        MyVideo->FrameActual=posActual;
        MyVideo->start();
    }
    else
    {
        QImage img(":/rc/Play.jpg");
        QPixmap pixmap=QPixmap::fromImage(img);
        QIcon Play(pixmap);
        PlayPause->setIcon(Play);
        PlayPause->setIconSize(pixmap.rect().size());

        MyVideo->pause=true;
        posActual=MyVideo->FrameActual;
        state=0;
        update();
    }
}


void MainWindow::ImagenAnterior()
{
    if(MyVideo->pause==false)
    {
        QImage img(":/rc/Play.jpg");
        QPixmap pixmap=QPixmap::fromImage(img);
        QIcon Play(pixmap);
        PlayPause->setIcon(Play);
        PlayPause->setIconSize(pixmap.rect().size());

        MyVideo->pause=true;
        posActual=MyVideo->FrameActual;
    }
    posActual=(((posActual-1)%N)+N)%N;
    update();
}

void MainWindow::ImagenSiguiente()
{
    if(MyVideo->pause==false)
    {
        QImage img(":/rc/Play.jpg");
        QPixmap pixmap=QPixmap::fromImage(img);
        QIcon Play(pixmap);
        PlayPause->setIcon(Play);
        PlayPause->setIconSize(pixmap.rect().size());

        MyVideo->pause=true;
        posActual=MyVideo->FrameActual;
    }
    posActual=(posActual+1)%N;
    update();
}

void MainWindow::zoomIn()
{
    scaleImage(1.10);
}

void MainWindow::zoomOut()
{
    scaleImage(0.9);
}

void MainWindow::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}



void MainWindow::CalibrarActivo()
{
    HerramientaActiva=0;
    imageLabel->setPixmap(QPixmap::fromImage(imagenOriginal));
    imagenActual=imagenOriginal;
}

void MainWindow::AnalizarActivo()
{
    HerramientaActiva=1;
    state=0;
    imageLabel->setPixmap(QPixmap::fromImage(imagenOriginal));
    imagenActual=imagenOriginal;
}
void SubQLabel::setScaleFactor(double factor)
{
    scaleFactor = factor;
}


void MainWindow::createActions()
{
    openAct = new QAction(tr("&Abrir..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    exitAct = new QAction(tr("Salir"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom &In (10%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (10%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Tamano Normal"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));


    connect(Anterior,SIGNAL(clicked()),this,SLOT(ImagenAnterior()));
    connect(Siguiente,SIGNAL(clicked()),this,SLOT(ImagenSiguiente()));
    connect(PlayPause,SIGNAL(clicked()),this,SLOT(Play_Pause()));
    connect(Frames,SIGNAL(valueChanged(int)),this,SLOT(GoToFrame()));
    connect(MenosSpeed,SIGNAL(clicked()),this,SLOT(MenosVelocidad()));
    connect(MasSpeed,SIGNAL(clicked()),this,SLOT(MasVelocidad()));
    connect(imageLabel,SIGNAL(PixelSpacingReady(double,double)),this,SLOT(setPixelSpacing(double,double)));
    connect(imageLabel,SIGNAL(DRefReady(double)),this,SLOT(setDRef(double)));
    connect(imageLabel,SIGNAL(EstenosisReady(double)),this,SLOT(setEstenosis(double)));

    aboutQtAct = new QAction(tr("Acerca de &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(Tab,SIGNAL(currentChanged(int)),this,SLOT(TabChanged(int)));
    connect(CancelarAnalisis,SIGNAL(clicked()),this,SLOT(AnalizarActivo()));
    connect(
        this,
        SIGNAL(scaleFactorChanged(const double&)),
        imageLabel,
        SLOT(setFactor(const double&)));

}

void MainWindow::setEstenosis(double est)
{
    char *txt=new char [20];
    sprintf(txt,"%.3lf",est);
    Estenosis->setText(txt);
}

void MainWindow::setDRef(double dref)
{
 char *txt=new char [20];
 sprintf(txt,"%.3lf",dref);
 DRef->setText(txt);

}

void MainWindow::setPixelSpacing(double x,double y)
{
 char *txt=new char [20];
 sprintf(txt,"%.3lf",x);
 SpacingX->setText(txt);
 sprintf(txt,"%.3lf",y);
 SpacingY->setText(txt);
}

void MainWindow::createMenus()
{
    fileMenu = new QMenu(tr("&Archivo"), this);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&Vista"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();

    helpMenu = new QMenu(tr("&Ayuda"), this);
    helpMenu->addAction(aboutQtAct);


    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);

}

void MainWindow::TabChanged(int current)
{
    if(current==0)
        AnalizarActivo();
    else
        CalibrarActivo();
}

void MainWindow::updateActions()
{
    zoomInAct->setEnabled(1);
    zoomOutAct->setEnabled(1);
    normalSizeAct->setEnabled(1);
}

void drawPixel(par p)
{
    int x=p.first,y=p.second;
    imagenActual.setPixelColor(x,y,Qt::blue);
    imagenActual.setPixelColor(x+1,y,Qt::blue);
    imagenActual.setPixelColor(x-1,y,Qt::blue);
    imagenActual.setPixelColor(x,y+1,Qt::blue);
    imagenActual.setPixelColor(x,y-1,Qt::blue);
    imagenActual.setPixelColor(x+1,y+1,Qt::blue);
    imagenActual.setPixelColor(x-1,y-1,Qt::blue);
    imagenActual.setPixelColor(x+1,y-1,Qt::blue);
    imagenActual.setPixelColor(x-1,y+1,Qt::blue);
}

void drawLine(par p1,par p2)
{
    int x1=p1.first,x2=p2.first;
    int y1=p1.second,y2=p2.second;
    int dx=abs(x1-x2);
    int dy=abs(y1-y2);
    CantidadPuntos=0;
// imagenActual=imagenOriginal;
    if(p1.first==p2.first)
    {
        for(int i=min(p1.second,p2.second); i<=max(p1.second,p2.second); i++)
            drawPixel(par(p1.first,i)),
            PSegmentacion[++CantidadPuntos]=par(p1.first,i);
    }
    else
    {
        double m=double(y2-y1)/double(x2-x1);
        double n=double(y2)-double(m*x2);
        if(dx>dy)
            for(int i=min(x1,x2); i<=max(x1,x2); i++)
                drawPixel(par(i,int(m*i+n))),
                PSegmentacion[++CantidadPuntos]=par(i,int(m*i+n));
        else
            for(int i=min(y1,y2); i<=max(y1,y2); i++)
                drawPixel(par(int((i-n)/m),i)),
                PSegmentacion[++CantidadPuntos]=par(int((i-n)/m),i);

    }
}


void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);

    emit scaleFactorChanged( scaleFactor );
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

bool Fisher(long long CantPixeles,long long Sgray,long long graynew)
{
    double Media1=double(Sgray/CantPixeles);
    double T;

    T=double(Media1-graynew);
    T*=T;

    return T<60.;
}



double angle(point a, point o, point b)   // returns angle aob in rad
{
    vec oa = toVec(o, a), ob = toVec(o, b);
    return acos(dot(oa, ob) / sqrt(norm_sq(oa) * norm_sq(ob)));
}

parDouble distance(int pos,int limit,int minimo,int maximo, int *EdgeMin,int *EdgeMax)
{
 double PSX,PSY;
 if(limit==1)
 {
     double dist1=1e9;
     int x1,x2,y1,y2,x3,y3;
     for(int i=minimo;i<=maximo;i++)
     {
      if(dist1>hypot(EdgeMax[i]-EdgeMin[pos],pos-i))
      {
        dist1=hypot(EdgeMax[i]-EdgeMin[pos],pos-i);
        x1=EdgeMax[i],y1=i;
        x2=EdgeMin[pos],y2=pos;
        x3=x2,y3=y1;
       }
      }
     double angulo=angle(point(x2,y2),point(x1,y1),point(x3,y3));
     PSY=CatheterDiameterMM*sin(angulo);
     if(y2!=y3)PSY/=abs(y2-y3);
     else PSY=0;
     PSX=CatheterDiameterMM*cos(angulo);
     PSX/=abs(x3-x1);
  }
 else
 {
     double dist1=1e9;
     int x1,x2,y1,y2,x3,y3;
     for(int i=minimo;i<=maximo;i++)
     {
      if(dist1>hypot(pos-i,EdgeMax[i]-EdgeMin[pos]))
      {
        dist1=hypot(pos-i,EdgeMax[i]-EdgeMin[pos]);
        x1=pos,y1=EdgeMin[pos];
        x2=i,y2=EdgeMax[i];
        x3=x2,y3=y1;
        if(i==pos)
          y3=y2;
       }
      }
     double angulo=angle(point(x2,y2),point(x1,y1),point(x3,y3));
     PSY=CatheterDiameterMM*sin(angulo);
     if(angulo==0)
      PSY=CatheterDiameterMM;
     PSY/=abs(y2-y1);
     PSX=CatheterDiameterMM*cos(angulo);
     if(x1!=x3)PSX/=abs(x3-x1);
     else PSX=0;
 }
 return parDouble(PSX,PSY);
 }

void MainWindow::CalibracionAceptada()
{

}

double square(double x)
{
 return x*x;
}

double SubQLabel::Medicion1(par Punto,int limit,int *EdgeMin,int *EdgeMax, int NN, int minimo, int maximo)
{
 int x,y,x1=0,y1=0,x2=-1,y2=-1;
 double pendiente=0;
 double intersect=0;
 if(limit==1)
 {
  //izquierda a derecha
  x=EdgeMin[Punto.second],y=Punto.second;
  for(int i=x-2;i<=x+2;i++)
      for(int j=y;j<=y+2;j++)
      {
       if(i==x && y==j)continue;
       if(EdgeMin[j]==i)
        x1=i,y1=j;
      }
  if(x==x1)
  {
   x2=EdgeMax[y1],y2=y1;
  }
  else
  {
   pendiente=double(y-y1)/double(x-x1);

   if(pendiente!=0)
   {
    pendiente=-1./pendiente;
    intersect=double(y1-double(pendiente*x1));
    for(int i=x1+1;i<=NN;i++)
    {
     int yy=int(pendiente*i+intersect);
     if(yy>=0 && EdgeMax[yy]!=-1 && abs(EdgeMax[yy]-i)<2)
     {
       x2=i,y2=yy;
       break;
     }
    }
   }
   }
   //derecha a izquierda
  if(x2==-1)
  {
      x=EdgeMax[Punto.second],y=Punto.second;
      for(int i=x-2;i<=x+2;i++)
          for(int j=y;j<=y+2;j++)
          {
           if(i==x && y==j)continue;
           if(EdgeMax[j]==i)
            x1=i,y1=j;
          }
      if(x==x1)
      {
       x2=EdgeMin[y1],y2=y1;
      }
      else
      {
       pendiente=double(y-y1)/double(x-x1);

       if(pendiente!=0)
       {
        pendiente=-1./pendiente;
        intersect=double(y1-double(pendiente*x1));
        for(int i=x1-1;i>=1;i--)
        {
         int yy=int(pendiente*i+intersect);
         if(yy>=0 && EdgeMin[yy]!=NN && abs(EdgeMin[yy]-i)<2)
         {
           x2=i,y2=yy;
           break;
          }
         }
       }
       }

  }
  }
  else
  {
     //abajo a arriba
     x=Punto.first,y=EdgeMax[Punto.first];
      for(int i=x;i<=x+2;i++)
          for(int j=y-2;j<=y+2;j++)
          {
           if(i==x && y==j)continue;
           if(EdgeMax[i]==j)
            x1=i,y1=j;
          }
      if(y==y1)
      {
         x2=x1,y2=EdgeMin[x1];
      }
      else
      {
       pendiente=double(y-y1)/double(x-x1);

       pendiente=-1./pendiente;
       intersect=double(y1-double(pendiente*x1));



       for(int i=y1-1;i>=1;i--)
       {
        int xx=int((i-intersect)/pendiente);
        if(xx>=0 && EdgeMin[xx]!=NN && abs(EdgeMin[xx]-i)<2)
        {
         x2=xx,y2=i;
         break;
        }
       }
      }
     // arriba a abajo
      if(x2==-1)
      {
          x=Punto.first,y=EdgeMin[Punto.first];
           for(int i=x;i<=x+2;i++)
               for(int j=y-2;j<=y+2;j++)
               {
                if(i==x && y==j)continue;
                if(EdgeMin[i]==j)
                 x1=i,y1=j;
               }
           if(y==y1)
           {
              x2=x1,y2=EdgeMax[x1];
           }
           else
           {
            pendiente=double(y-y1)/double(x-x1);

            pendiente=-1./pendiente;
            intersect=double(y1-double(pendiente*x1));



            for(int i=y1+1;i<=NN;i++)
            {
             int xx=int((i-intersect)/pendiente);
             if(xx>=0 && EdgeMax[xx]!= -1 && abs(EdgeMax[xx]-i)<2)
             {
              x2=xx,y2=i;
              break;
             }
            }
           }
      }
  }
// qDebug() << NN;
// qDebug() << pendiente << ' ' << intersect;
// qDebug() << minimo << ' ' << maximo;
// qDebug() << x << ' ' << y;
// qDebug() << x1 << ' ' << y1;
// qDebug() << x2 << ' ' << y2;

   return sqrt(square((x2-x1)*PixelSpacingX)+square((y2-y1)*PixelSpacingY));
}

bool EDGE(int x,int y)
{
 //sobel
 QRgb Pixel1=imagenOriginal.pixel(x-1,y-1);
 double z1=value(Pixel1);
 QRgb Pixel2=imagenOriginal.pixel(x-1,y);
 double z2=value(Pixel2);
 QRgb Pixel3=imagenOriginal.pixel(x-1,y+1);
 double z3=value(Pixel3);
 QRgb Pixel4=imagenOriginal.pixel(x,y-1);
 double z4=value(Pixel4);
 QRgb Pixel5=imagenOriginal.pixel(x,y);
 double z5=value(Pixel5);
 QRgb Pixel6=imagenOriginal.pixel(x,y+1);
 double z6=value(Pixel6);
 QRgb Pixel7=imagenOriginal.pixel(x+1,y-1);
 double z7=value(Pixel7);
 QRgb Pixel8=imagenOriginal.pixel(x+1,y);
 double z8=value(Pixel8);
 QRgb Pixel9=imagenOriginal.pixel(x+1,y+1);
 double z9=value(Pixel9);

 double Rx = abs(z3+sqrt(2.)*z6+z9 - (z1+sqrt(2.)*z4+z7));
 double Ry = abs(z7+sqrt(2.)*z8+z9 - (z1+sqrt(2.)*z2+z3));

 double Gradient=Rx+Ry;

 return Gradient>21.;


}


void SubQLabel::mousePressEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    int x_image = x/scaleFactor;
    int y_image = y/scaleFactor;

    if(HerramientaActiva==0)
    {
        if (event->button() == Qt::LeftButton)
        {

            if(par(x_image,y_image)==LastPoint)
            {
                par p1=PSegmentacion[1];
                par p2=PSegmentacion[CantidadPuntos];
                int minx=min(p1.first,p2.first);
                int maxx=max(p1.first,p2.first);
                int miny=min(p1.second,p2.second);
                int maxy=max(p1.second,p2.second);
                int height=imagenOriginal.height();
                int width=imagenOriginal.width();
                int *EdgeMax,*EdgeMin;
                int N=max(height,width);
                EdgeMax=new int[N];
                EdgeMin=new int[N];
                for(int i=0; i<N; i++)
                    EdgeMax[i]=-1,
                               EdgeMin[i]=N;

                int dx=maxx-minx;
                int dy=maxy-miny;
                bool limit=0;// 0 si el limite esta en las x, 1 si esta en las y
                if(dx<dy)
                    limit=1;

                QImage nuevaImagen(imagenOriginal);
                QRgb Pixel;
                int  grayold;
                long long Sgray=0;
                long long CantPixeles=CantidadPuntos;
//                ///* Deteccion de bordes
//                 if(limit==1)
//                 {
//                  for(int i=1;i<=CantidadPuntos;i++)
//                  {
//                   int x=PSegmentacion[i].first;
//                   int y=PSegmentacion[i].second;
//                   for(int j=x+1;j<=N;j++)
//                    if(EDGE(j,y))
//                    {
//                     EdgeMax[y]=j;
//                     break;
//                    }
//                   for(int j=x-1;j>=1;j--)
//                    if(EDGE(j,y))
//                    {
//                     EdgeMin[y]=j;
//                     break;
//                    }
//                  }
//                 }
//                 else
//                 {
//                     for(int i=1;i<=CantidadPuntos;i++)
//                     {
//                      int x=PSegmentacion[i].first;
//                      int y=PSegmentacion[i].second;
//                      for(int j=y+1;j<=N;j++)
//                       if(EDGE(x,j))
//                       {
//                        EdgeMax[x]=j;
//                        break;
//                       }
//                      for(int j=y-1;j>=1;j--)
//                       if(EDGE(x,j))
//                       {
//                        EdgeMin[x]=j;
//                        break;
//                       }
//                     }
//                 }
                //*/
                for(int i=1;i<=CantidadPuntos;i++)
                {
                 Pixel=imagenOriginal.pixel(PSegmentacion[i].first,PSegmentacion[i].second);
                 grayold=value(Pixel);
                 Sgray+=1ll*grayold;
                 cola.push(PSegmentacion[i]);
                 nuevaImagen.setPixel(PSegmentacion[i].first,PSegmentacion[i].second,qRgb(0,0,0));
                }

                for(; !cola.empty(); cola.pop())
                {
                    int x=cola.front().first;
                    int y=cola.front().second;
                    for(int i=0; i<4; i++)
                    {
                        int nx=x+mx[i];
                        int ny=y+my[i];

                        QRgb rgb=nuevaImagen.pixel(nx,ny);
                        int gray=value(rgb);

                        if(limit==1 && (ny<miny || ny>maxy) || gray==0)
                            continue;
                        if(limit==0 && (nx<minx || nx>maxx) )
                            continue;

                        QRgb nuevoPixel=imagenOriginal.pixel(nx,ny);
                        int graynew=value(nuevoPixel);
                        if(Fisher(CantPixeles,Sgray,graynew))
                        {
                            nuevaImagen.setPixel(nx,ny,qRgb(0,0,0));
                            cola.push(par(nx,ny));
                            CantPixeles++;
                            Sgray+=graynew;
                            if(limit)
                            {
                                EdgeMax[ny]=max(EdgeMax[ny],nx);
                                EdgeMin[ny]=min(EdgeMin[ny],nx);
                            }
                            else
                            {
                                EdgeMax[nx]=max(EdgeMax[nx],ny);
                                EdgeMin[nx]=min(EdgeMin[nx],ny);
                            }
                        }
                    }
                }
                //imagenActual=nuevaImagen;
                imagenActual=imagenOriginal;
                if(limit==1)//vertical
                {
                    for(int i=miny; i<=maxy; i++)
                        drawPixel(par(EdgeMax[i],i)),
                        drawPixel(par(EdgeMin[i],i)),
                        imagenActual.setPixelColor(EdgeMax[i],i,Qt::blue),
                                                   imagenActual.setPixelColor(EdgeMin[i],i,Qt::blue);
                 int pos=(miny+maxy)/2;
                 int pos1=pos-1;
                 int pos2=pos+1;

                 parDouble dist1=parDouble(0,0),dist2=parDouble(0,0),dist3=parDouble(0,0);

                 dist1=distance(pos,limit,miny,maxy,EdgeMin,EdgeMax);
                 if(pos1>=miny)
                  dist2=distance(pos1,limit,miny,maxy,EdgeMin,EdgeMax);
                 if(pos2<=maxy)
                  dist3=distance(pos2,limit,miny,maxy,EdgeMin,EdgeMax);
                 int cant1=0;
                 if(dist1.first)cant1++;if(dist2.first)cant1++;if(dist3.first)cant1++;
                 int cant2=0;
                 if(dist1.second)cant2++;if(dist2.second)cant2++;if(dist3.second)cant2++;

                 if(cant1)PixelSpacingX=(dist1.first+dist2.first+dist3.first)/cant1;
                 else PixelSpacingX=0;

                 if(cant2)PixelSpacingY=(dist1.second+dist2.second+dist3.second)/cant2;
                 else PixelSpacingY=0;

                 if(PixelSpacingX==0)PixelSpacingX=PixelSpacingY;
                 if(PixelSpacingY==0)PixelSpacingY=PixelSpacingX;
                }
                else{
                    for(int i=minx; i<=maxx; i++)
                        drawPixel(par(i,EdgeMax[i])),
                        drawPixel(par(i,EdgeMin[i])),
                        imagenActual.setPixelColor(i,EdgeMax[i],Qt::blue),
                                                   imagenActual.setPixelColor(i,EdgeMin[i],Qt::blue);
                    int pos=(minx+maxx)/2;
                    int pos1=pos-1;
                    int pos2=pos+1;

                    parDouble dist1=parDouble(0,0),dist2=parDouble(0,0),dist3=parDouble(0,0);

                    dist1=distance(pos,limit,minx,maxx,EdgeMin,EdgeMax);
                    if(pos1>=miny)
                     dist2=distance(pos1,limit,minx,maxx,EdgeMin,EdgeMax);
                    if(pos2<=maxy)
                     dist3=distance(pos2,limit,minx,maxx,EdgeMin,EdgeMax);
                    int cant1=0;
                    if(dist1.first)cant1++;if(dist2.first)cant1++;if(dist3.first)cant1++;
                    int cant2=0;
                    if(dist1.second)cant2++;if(dist2.second)cant2++;if(dist3.second)cant2++;

                    if(cant1)PixelSpacingX=(dist1.first+dist2.first+dist3.first)/cant1;
                    else PixelSpacingX=0;

                    if(cant2)PixelSpacingY=(dist1.second+dist2.second+dist3.second)/cant2;
                    else PixelSpacingY=0;

                    if(PixelSpacingX==0)PixelSpacingX=PixelSpacingY;
                    if(PixelSpacingY==0)PixelSpacingY=PixelSpacingX;
                }
                emit PixelSpacingReady(PixelSpacingX,PixelSpacingY);
                setPixmap(QPixmap::fromImage(imagenActual));
                LastPoint=par(x_image,y_image);
            }
            else
            {
                //setPixmap(QPixmap::fromImage(imagenActual));
                LastPoint=par(x_image,y_image);
            }

        }
    }
    else
    {
        if (event->button() == Qt::LeftButton)
        {

            if(par(x_image,y_image)==LastPoint && (state==3 || state==4))
            {
                //analizar;
                par p1=Puntos[0];
                par p2;
                if(state==3)
                 p2=Puntos[2];
                else
                 p2=Puntos[3];

                int minx=min(p1.first,p2.first);
                int maxx=max(p1.first,p2.first);
                int miny=min(p1.second,p2.second);
                int maxy=max(p1.second,p2.second);
                int height=imagenOriginal.height();
                int width=imagenOriginal.width();
                int *EdgeMax,*EdgeMin;
                int N=max(height,width);
                EdgeMax=new int[N];
                EdgeMin=new int[N];
                for(int i=0; i<N; i++)
                    EdgeMax[i]=-1,
                               EdgeMin[i]=N;

                int dx=maxx-minx;
                int dy=maxy-miny;
                bool limit=0;// 0 si el limite esta en las x, 1 si esta en las y
                if(dx<dy)
                {
                    limit=1;
                 if(p1.second>p2.second)
                   swap(p1,p2);
                }
                else
                {
                 if(p1.first>p2.first)
                   swap(p1,p2);
                }
                QImage nuevaImagen(imagenOriginal);
                QRgb Pixel=imagenOriginal.pixel(p1.first,p1.second);
                int  grayold1=value(Pixel);
                Pixel=imagenOriginal.pixel(p2.first,p2.second);
                int grayold2=value(Pixel);
                Pixel=imagenOriginal.pixel(Puntos[1].first,Puntos[1].second);
                int grayold3=value(Pixel);

                long long Sgray=1ll*grayold1+1ll*grayold2+1ll*grayold3;
                long long CantPixeles=3;
                cola.push(p1);
                cola.push(p2);
                cola.push(Puntos[1]);
                nuevaImagen.setPixel(Puntos[1].first,Puntos[1].second,qRgb(0,0,0));
                nuevaImagen.setPixel(p1.first,p1.second,qRgb(0,0,0));
                nuevaImagen.setPixel(p2.first,p2.second,qRgb(0,0,0));
                for(; !cola.empty(); cola.pop())
                {
                    int x=cola.front().first;
                    int y=cola.front().second;
                    for(int i=0; i<4; i++)
                    {
                        int nx=x+mx[i];
                        int ny=y+my[i];

                        QRgb rgb=nuevaImagen.pixel(nx,ny);
                        int gray=value(rgb);

                        if(limit==1 && (ny<miny || ny>maxy) || gray==0)
                            continue;
                        if(limit==0 && (nx<minx || nx>maxx) )
                            continue;

                        QRgb nuevoPixel=imagenOriginal.pixel(nx,ny);
                        int graynew=value(nuevoPixel);
                        if(Fisher(CantPixeles,Sgray,graynew))
                        {
                            nuevaImagen.setPixel(nx,ny,qRgb(0,0,0));
                            cola.push(par(nx,ny));
                            CantPixeles++;
                            Sgray+=graynew;
                            if(limit)
                            {
                                EdgeMax[ny]=max(EdgeMax[ny],nx);
                                EdgeMin[ny]=min(EdgeMin[ny],nx);
                            }
                            else
                            {
                                EdgeMax[nx]=max(EdgeMax[nx],ny);
                                EdgeMin[nx]=min(EdgeMin[nx],ny);
                            }
                        }
                    }
                }
                imagenActual=nuevaImagen;
                if(limit==1)//vertical
                {
                  for(int i=miny; i<=maxy; i++)
                    imagenActual.setPixelColor(EdgeMax[i],i,Qt::blue),
                    imagenActual.setPixelColor(EdgeMin[i],i,Qt::blue);

                 emit DRefReady(Medicion1(p1,limit,EdgeMin,EdgeMax,N,miny,maxy));
                 emit EstenosisReady(Medicion1(Puntos[1],limit,EdgeMin,EdgeMax,N,miny,maxy));
                  //Medicion2(p2);

                }
                else
                {
                    for(int i=minx; i<=maxx; i++)
                        imagenActual.setPixelColor(i,EdgeMax[i],Qt::blue),
                                                   imagenActual.setPixelColor(i,EdgeMin[i],Qt::blue);

                    emit DRefReady(Medicion1(p1,limit,EdgeMin,EdgeMax,N,minx,maxx));
                    emit EstenosisReady(Medicion1(Puntos[1],limit,EdgeMin,EdgeMax,N,minx,maxx));

                }
                setPixmap(QPixmap::fromImage(imagenActual));
                LastPoint=par(-1,-1);

                state=0;
            }
            else if(state==0)
            {
                imagenActual=imagenOriginal;
                drawPixel(par(x_image,y_image));
                Puntos[0]=par(x_image,y_image);
                state=1;
                LastPoint=par(x_image,y_image);
            }
            else if(state==1)
            {
                drawPixel(par(x_image,y_image));
                Puntos[1]=par(x_image,y_image);
                state=2;
                LastPoint=par(x_image,y_image);
            }
            else if(state==2)
            {
                drawPixel(par(x_image,y_image));
                Puntos[2]=par(x_image,y_image);
                state=3;
                drawLine(Puntos[0],Puntos[1]);
                drawLine(Puntos[1],Puntos[2]);
                imageState3=imagenActual;
                LastPoint=par(-1,-1);
            }
            else if(state==3 || state==4)
                LastPoint=par(x_image,y_image);

            setPixmap(QPixmap::fromImage(imagenActual));
        }
    }
}

//! [1]
void SubQLabel::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    int x_image = x/scaleFactor;
    int y_image = y/scaleFactor;

    if(HerramientaActiva==0)
    {
        if (event->buttons() & Qt::LeftButton)
        {
            imagenActual=imagenOriginal;
            drawLine(LastPoint,par(x_image,y_image));
            setPixmap(QPixmap::fromImage(imagenActual));
        }
    }
    else
    {
        if (event->buttons() & Qt::LeftButton && state==4)
        {
            drawPixel(par(x_image,y_image));
            setPixmap(QPixmap::fromImage(imagenActual));
            Puntos[3]=par(x_image,y_image);
        }
        else if(event->buttons() & Qt::LeftButton && state==3)
        {
            imagenActual=imageState3;
            setPixmap(QPixmap::fromImage(imagenActual));
            state=4;
            Puntos[3]=par(x_image,y_image);
        }
    }
}
//! [1]

void SubQLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if(HerramientaActiva==1 &&  state==4)
        state=3,Puntos[2]=Puntos[3];
}
MainWindow::~MainWindow()
{
    delete ui;
}

