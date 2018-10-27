#include "sidewidget.h"
#include<qdebug.h>
#include<QCoreApplication>
#include<QApplication>
#include<QDesktopServices>
#include<QThread>
extern bool sideshowed;
extern QString name;
extern bool sidefixed;
extern bool anime;
extern int savetime;
QString textstr;
extern QSettings *m_IniFile;
extern int sideOpacity;
sidewidget::sidewidget(QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet("background-color:green");
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(400,GetSystemMetrics(SM_CYFULLSCREEN)+30);
    this->setWindowOpacity(0.9);

    timer=new QTimer(this);
    hideBtn=new QPushButton(tr("隐藏"));
    fixedBtn =new QPushButton(tr("固定"));
    pictureBtn=new QPushButton(tr("加入图片"));
    updatecomboxBtn=new QPushButton(tr("更新目录"));
    tEdit=new QPlainTextEdit;
    combox=new QComboBox;
    pictureLabel1=new QLabel;
    mainlayout=new QGridLayout(this);

    hideBtn->setStyleSheet("background-color:white");
    fixedBtn->setStyleSheet("background-color:white");
    pictureBtn->setStyleSheet("background-color:white");
    updatecomboxBtn->setStyleSheet("background-color:white");
    tEdit->setStyleSheet("background-image:url(:/new/prefix1/syoujyo.png)");
    combox->setStyleSheet("background-color:yellow");

    mainlayout->addWidget(fixedBtn,0,0);
    mainlayout->addWidget(hideBtn,0,1);
    mainlayout->addWidget(tEdit,1,0,1,2);
    mainlayout->addWidget(pictureBtn,2,0);
    mainlayout->addWidget(updatecomboxBtn,2,1);
    mainlayout->addWidget(combox,3,0,1,2);
    mainlayout->addWidget(pictureLabel1,4,0,1,2);
    mainlayout->setSpacing(5);

    path=QCoreApplication::applicationDirPath();
    path+="/AppDate/";
    dir.setPath(path);
    if (!dir.exists(path))
    dir.mkpath(path);
    file_list = dir.entryInfoList(QDir::Files | QDir::Hidden);
        for(int i=0;i<file_list.count();i++)
        {
            filenamelist.append(file_list.at(i).fileName());
            combox->addItem(file_list.at(i).fileName());
        }
        m_IniFile->setValue("picture/count",file_list.count());
        qDebug()<<file_list;
        for(int m=0;m<file_list.count();m++)
        {
            if(path+file_list.at(m).fileName()==m_IniFile->value("picture/current").toString())
               combox->setCurrentIndex(m);
        }
    tEdit->setFixedSize(370,500);
    tEdit->setFont(QFont(tr("宋体"), 12,63));
    textstr=m_IniFile->value("text/text").toString();


    cursor = tEdit->textCursor();
    cursor.mergeCharFormat(fmt);
    tEdit->mergeCurrentCharFormat(fmt);
    tEdit->setPlainText(textstr);
    tEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    createMenuandAction();
    firstrun=true;

    QPixmap *img= new QPixmap(m_IniFile->value("picture/current").toString());
    QPixmap fitpixmap =img->scaled(370, 490, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pictureLabel1->setPixmap(fitpixmap);

    pictureLabel1->installEventFilter(this);


    SetWindowLong((HWND)winId(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);

    connect(fixedBtn,SIGNAL(clicked(bool)),this,SLOT(fixedpro()));
    connect(hideBtn,SIGNAL(clicked(bool)),this,SLOT(hidepro()));
    connect(pictureBtn,SIGNAL(clicked(bool)),this,SLOT(picturepro()));
    connect(updatecomboxBtn,SIGNAL(clicked(bool)),this,SLOT(updatepro()));
    connect(tEdit,SIGNAL(textChanged()),this,SLOT(textchangepro()));
    connect(timer,SIGNAL(timeout()),this,SLOT(timepro()));
    connect(combox,SIGNAL(activated(int)),this,SLOT(comboxpro(int)));
    connect(tEdit,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showMenu(QPoint)));
}
void sidewidget::hidepro()
{
    int i;
    if(anime)
    {
    for(i=0;i>-400;i-=4)
    {
        QThread::msleep(1);
        move(i,0);
    }
    i=0;
    }
    m_IniFile->setValue("text/text",textstr);
    this->hide();
    this->move(0,0);
    sideshowed=false;
    timer->stop();
}
void sidewidget::fixedpro()
{
    if(sidefixed==false)
    {
        timer->stop();
        fixedBtn->setText(tr("取消固定"));
        sidefixed=true;
        return;
    }
    if(sidefixed==true)
    {
        timer->start(100);
        fixedBtn->setText(tr("固定"));
        sidefixed=false;
        return;
    }

}
void sidewidget::textchangepro()
{
    cursor.mergeCharFormat(fmt);
    tEdit->mergeCurrentCharFormat(fmt);
    textstr=tEdit->toPlainText();
}
void sidewidget::timepro()
{
    POINT p;
    GetCursorPos(&p);
    if(p.x>450)
        hidepro();
}
void sidewidget::picturepro()
{
            timer->stop();
            QFileDialog *fileDialog = new QFileDialog(this);
            fileDialog->setWindowTitle(tr("打开图片"));
            fileDialog->setDirectory(".");
            fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)"));
            fileDialog->setFileMode(QFileDialog::ExistingFiles);
            fileDialog->setViewMode(QFileDialog::Detail);
            QStringList fileNames;
            if(fileDialog->exec())
            {
                fileNames = fileDialog->selectedFiles();
                QString temp=fileNames[0];
                QStringList namelist = temp.split("/");//分割后的名字
                QFile::copy(temp, path+namelist.at(namelist.count()-1));
                if(filenamelist.count()==0)
                {
                    combox->blockSignals(true);
                    combox->clear();
                    combox->addItem(namelist[namelist.count()-1]);
                    combox->blockSignals(false);
                    filenamelist.append(namelist.at(namelist.count()-1));
                    combox->setCurrentIndex(m_IniFile->value("picture/count").toInt());
                    comboxpro(combox->currentIndex());
                    m_IniFile->setValue("picture/count",m_IniFile->value("picture/count").toInt()+1);

                }
                else
                {
                    combox->addItem(namelist[namelist.count()-1]);
                    filenamelist.append(namelist.at(namelist.count()-1));
                    combox->setCurrentIndex(m_IniFile->value("picture/count").toInt());
                    comboxpro(combox->currentIndex());
                    m_IniFile->setValue("picture/count",m_IniFile->value("picture/count").toInt()+1);
                }

            }
            timer->start(100);
}
void sidewidget::updatepro()
{
    combox->blockSignals(true);
    combox->clear();

    //调用初始化方法
    filenamelist.clear();
    file_list = dir.entryInfoList(QDir::Files | QDir::Hidden);
    if(file_list.count()!=0)//循环寻找并加入下拉框
    {
        for(int i=0;i<file_list.count();i++)
        {
            filenamelist.append(file_list.at(i).fileName());
            combox->addItem(file_list.at(i).fileName());
        }
        m_IniFile->setValue("picture/count",file_list.count());
        combox->blockSignals(false);
        for(int m=0;m<file_list.count();m++)
        {
            if(path+file_list.at(m).fileName()==m_IniFile->value("picture/current").toString())
               combox->setCurrentIndex(m);
        }
    }

    tEdit->setFont(QFont(tr("宋体"), 12,63));


    cursor = tEdit->textCursor();
    cursor.mergeCharFormat(fmt);
    tEdit->mergeCurrentCharFormat(fmt);
    tEdit->setPlainText(textstr);

}
void sidewidget::comboxpro(int index)
{
    qDebug()<<index;
    QPixmap *img= new QPixmap(path+"/"+filenamelist.at(index));
    QPixmap fitpixmap =img->scaled(370, 490, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    pictureLabel1->setPixmap(fitpixmap);
    m_IniFile->setValue("picture/current",path+filenamelist.at(index));
}
bool sidewidget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj==pictureLabel1)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QString str=m_IniFile->value("picture/current").toString();
            qDebug()<<str;
            str.replace("/","\\");
            ShellExecute(NULL, L"open", L"explorer.exe",str.toStdWString().c_str(), NULL, SW_SHOWNORMAL);
        }
    }
}
void sidewidget::savepro()
{
    m_IniFile->setValue("text/text",textstr);
}
void sidewidget::showMenu(QPoint p)
{
    if(tEdit->textCursor().selectedText()=="")
        {
        copyAction->setDisabled(true);
        cutAction->setDisabled(true);
        URLAction->setDisabled(true);
        }
    else
    {
        copyAction->setDisabled(false);
        cutAction->setDisabled(false);
        URLAction->setDisabled(false);
    }
    selectallAction->setDisabled(true);
    textMenu->move(p);
    textMenu->show();
}
void sidewidget::createMenuandAction()
{
    textMenu=new QMenu(this);
    cutAction=new QAction(tr("剪切"),this);
    copyAction=new QAction(tr("复制"),this);
    selectallAction=new QAction(tr("全选(有问题)"),this);
    URLAction=new QAction(tr("打开URL"),this);
    pasteAction=new QAction(tr("粘贴"),this);
    textMenu->addAction(copyAction);
    textMenu->addAction(cutAction);
    textMenu->addAction(selectallAction);
    textMenu->addAction(pasteAction);
    textMenu->addSeparator();
    textMenu->addAction(URLAction);

    connect(copyAction,SIGNAL(triggered(bool)),SLOT(copypro()));
    connect(cutAction,SIGNAL(triggered(bool)),SLOT(cutpro()));
    connect(selectallAction,SIGNAL(triggered(bool)),SLOT(selectallypro()));
    connect(URLAction,SIGNAL(triggered(bool)),SLOT(URLpro()));
    connect(pasteAction,SIGNAL(triggered(bool)),SLOT(pastepro()));
}
void sidewidget::copypro()
{

    QTextCursor cur=tEdit->textCursor();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(cur.selectedText());
}
void sidewidget::cutpro()
{
    QTextCursor cur=tEdit->textCursor();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(cur.selectedText());
    cur.removeSelectedText();
}
void sidewidget::pastepro()
{
    QTextCursor cur=tEdit->textCursor();
    QClipboard *clipboard = QApplication::clipboard();
    if(cur.selectedText()!="")
    {
        cur.removeSelectedText();
        cur.insertText(clipboard->text());
    }
    else
    cur.insertText(clipboard->text());
}
void sidewidget::selectallypro()
{
    QTextCursor cur=tEdit->textCursor();
    cur.setPosition(1,QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::NoMove,QTextCursor::KeepAnchor,100);
    cur.select(QTextCursor::WordUnderCursor);
    tEdit->setTextCursor(cur);
}
void sidewidget::URLpro()
{
    QString temp=tEdit->textCursor().selectedText();
    QDesktopServices::openUrl(QUrl(temp));
}
