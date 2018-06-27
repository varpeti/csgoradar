#include "window.h"

void readCFG(std::vector<float> &v)
{
    std::ifstream f("settings.cfg");
    std::string line;
    std::string map;
    std::getline(f, map);
    while (std::getline(f, line))
    {
        if (map==line) break;
    }
    while (std::getline(f, line))
    {
        std::istringstream iss(line);
        float a;
        if (!(iss >> a)) { break; } // error
        v.push_back(a);
    }
    f.close();
}

int execQApp(Hack *hack)
{
    char *argv[] = {"window", NULL};
    int argc = sizeof(argv) / sizeof(char*) - 1;
    QApplication app(argc,argv);
    QWindow mainWindow;
    readCFG(mainWindow.params);

    char buff[30];
    sprintf(buff,"radarimages/%.0f.png",mainWindow.params[4]);
    mainWindow.pixmap.load(buff);
    mainWindow.hack = hack;

    QFont font ("Courier", 30, 100);
    mainWindow.showFullScreen();
    return app.exec();
}

QWindow::QWindow(QWidget *parent) : QWidget(parent)
{ 
    QSize size = qApp->screens()[0]->size();
    setGeometry(0, 0, size.width(), size.height() );
    setAutoFillBackground(false);
    //setWindowFlags(Qt::X11BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowTransparentForInput | Qt::X11BypassWindowManagerHint);
    QTimer::singleShot(1, this, SLOT(callback()));
}

void QWindow::callback()
{
    if (hack->isOK() and hack->isConnected) 
    {
        hack->readEntities(entitiesToScreen);
    }
    update();
    QTimer::singleShot(16, this, SLOT(callback()));
}

void QWindow::paintEvent(QPaintEvent *)
{   
    if (!hack->isOK() or !hack->isConnected) 
    {
        return;
    }
    
    const QColor red(255, 050, 05);
    const QColor green(050, 255, 05);
    const QColor blue(05, 050, 255);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(105, 225, 225, 0));

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    pen.setColor(QColor(255, 255, 255, 255));
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    QFont font("DroidSansBold", 7, QFont::Bold, false);
    painter.setFont(font);

    painter.setOpacity(params[10]);
    painter.drawPixmap( QRect(params[8]+params[5],params[9]+params[6],1024*params[7],1024*params[7]), pixmap);
    painter.setOpacity(1.0);

    for (int i = 0; i < 64; ++i)
    {
        if (entitiesToScreen[i].entity.m_iHealth <= 0) continue;
        pen.setColor( (entitiesToScreen[i].entity.m_iTeamNum == 3) ? (entitiesToScreen[i].isLocalPlayer ? green : blue) : (entitiesToScreen[i].isLocalPlayer ? green : red) );
        painter.setPen(pen);
        float ix = params[8]+(params[0]+(entitiesToScreen[i].entity.m_vecNetworkOrigin.x)/params[2]*params[3]);
        float iy = params[9]+(params[1]-(entitiesToScreen[i].entity.m_vecNetworkOrigin.y)/params[2]*params[3]);
        float ia = entitiesToScreen[i].entity.m_angNetworkAngles.y;
        painter.translate(ix,iy);
        painter.drawText(0,0, std::to_string(entitiesToScreen[i].entity.m_iHealth).c_str() );
        painter.rotate(-ia);
        painter.drawLine(0,0,15,0);
        painter.rotate(ia);
        painter.translate(-ix,-iy);
        
    }
}