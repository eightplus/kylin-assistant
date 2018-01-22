/*
 * Copyright (C) 2013 ~ 2018 National University of Defense Technology(NUDT) & Tianjin Kylin Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntukylin.com/kobe24_lixiang@126.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "networkflow.h"
#include "smoothcurvegenerator.h"
#include "util.h"

#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QButtonGroup>
#include <QSvgRenderer>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>

inline QString formatNetworkBrandWidth(guint64 size, bool isTotal)
{
    enum {
        K_INDEX,
        M_INDEX,
        G_INDEX,
        T_INDEX
    };

    QList<guint64> factorList;
    factorList.append(G_GUINT64_CONSTANT(1) << 10);//KiB
    factorList.append(G_GUINT64_CONSTANT(1) << 20);//MiB
    factorList.append(G_GUINT64_CONSTANT(1) << 30);//GiB
    factorList.append(G_GUINT64_CONSTANT(1) << 40);//TiB

    if (size < factorList.at(K_INDEX)) {
        if ((guint) size > 1) {
            if (isTotal)
                return QString("%1 %2").arg((guint) size).arg(QObject::tr("bits"));
            else
                return QString("%1 %2").arg((guint) size).arg(QObject::tr("bits/s"));
        }
        else {
            if (isTotal)
                return QString("%1 %2").arg((guint) size).arg(QObject::tr("bit"));
            else
                return QString("%1 %2").arg((guint) size).arg(QObject::tr("bit/s"));
        }
    } else {
        guint64 factor;
        QString format;
        if (size < factorList.at(M_INDEX)) {
            factor = factorList.at(K_INDEX);
            if (isTotal)
                format = QObject::tr("KiB");
            else
                format = QObject::tr("KiB/s");
        }else if (size < factorList.at(G_INDEX)) {
            factor = factorList.at(M_INDEX);
            if (isTotal)
                format = QObject::tr("MiB");
            else
                format = QObject::tr("MiB/s");
        } else if (size < factorList.at(T_INDEX)) {
            factor = factorList.at(G_INDEX);
            if (isTotal)
                format = QObject::tr("GiB");
            else
                format = QObject::tr("GiB/s");
        } else {
            factor = factorList.at(T_INDEX);
            if (isTotal)
                format = QObject::tr("TiB");
            else
                format = QObject::tr("TiB/s");
        }
        std::string formatted_result(make_string(g_strdup_printf("%.1f", size / (double)factor)));
        return QString::fromStdString(formatted_result) + format;
    }
}

inline QString formatNetwork(guint64 rate)
{
    return formatNetworkBrandWidth(rate, true);
}

inline QString formatNetworkRate(guint64 rate)
{
    return formatNetworkBrandWidth(rate, false);
}

NetworkFlow::NetworkFlow(QWidget *parent) : QWidget(parent)
{
    setFixedSize(302, 200);

    m_netMaxHeight = 60;
    m_pointSpace = 10;

//    math1_radio = new QRadioButton();
//    math2_radio = new QRadioButton();
//    math1_radio->setFocusPolicy(Qt::NoFocus);
//    math1_radio->setObjectName("math1");
//    math2_radio->setFocusPolicy(Qt::NoFocus);
//    math2_radio->setObjectName("math2");
//    math1_radio->setChecked(false);
//    math2_radio->setChecked(true);
//    math1_radio->move(10,10);
//    math2_radio->move(100,10);
//    connect(math1_radio, SIGNAL(clicked()), this, SLOT(setRadioButtonRowStatus()));
//    connect(math2_radio, SIGNAL(clicked()), this, SLOT(setRadioButtonRowStatus()));

    m_pointsCount = int((this->width() -2) / m_pointSpace);
    m_downloadSpeedList = new QList<long>();
    for (int i = 0; i < m_pointsCount; i++) {
        m_downloadSpeedList->append(0);
    }

    m_uploadSpeedList = new QList<long>();
    for (int i = 0; i < m_pointsCount; i++) {
        m_uploadSpeedList->append(0);
    }

    m_gridY = new QList<int>();

    /*connect(ui->generateCurveButton, SIGNAL(clicked(bool)), this, SLOT(generateCurves()));
    connect(ui->showKnotsCheckBox, SIGNAL(clicked(bool)), this, SLOT(update()));
    connect(ui->showSmoothCurveCheckBox, SIGNAL(clicked(bool)), this, SLOT(update()));
    connect(ui->smoothCurveGeneratorComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    ui->generateCurveButton->click();*/
}

NetworkFlow::~NetworkFlow()
{
    delete m_downloadSpeedList;
    delete m_uploadSpeedList;
    delete m_gridY;
}

void NetworkFlow::setRadioButtonRowStatus()
{
    QObject *obj = sender(); //返回发出信号的对象，用QObject类型接收
    QRadioButton* pbtn = qobject_cast<QRadioButton*>(obj);
    QString obj_name = pbtn->objectName();
    if(obj_name == "math1")
    {

    }
    else if(obj_name == "math2")
    {

    }
}

//http://www.qtdebug.com/qtbook-paint-smooth-curve/
void NetworkFlow::onUpdateNetworkStatus(long recvTotalBytes, long sentTotalBytes, long recvRateBytes, long sentRateBytes)
{
    m_recvTotalBytes = recvTotalBytes;
    m_sentTotalBytes = sentTotalBytes;
    m_recvRateBytes = recvRateBytes;
    m_sentRateBytes = sentRateBytes;

    //download
    QList<QPointF> downloadPoints;
    m_downloadSpeedList->append(m_recvRateBytes);
    if (m_downloadSpeedList->size() > m_pointsCount) {
        m_downloadSpeedList->pop_front();
    }
    //计算出下载速度中最大的值
    long downloadMaxHeight = 0;
    for (int i = 0; i < m_downloadSpeedList->size(); i++) {
        if (m_downloadSpeedList->at(i) > downloadMaxHeight) {
            downloadMaxHeight = m_downloadSpeedList->at(i);
        }
    }
    for (int i = 0; i < m_downloadSpeedList->size(); i++) {
        if (downloadMaxHeight < m_netMaxHeight) {
            downloadPoints.append(QPointF(i * m_pointSpace, m_downloadSpeedList->at(i)));
        }
        else {
            downloadPoints.append(QPointF(i * m_pointSpace, m_downloadSpeedList->at(i) * m_netMaxHeight / downloadMaxHeight));
        }
    }
    m_downloadPath = SmoothCurveGenerator::generateSmoothCurve(downloadPoints);


    /*// 根据曲线上的点创建平滑曲线
    smoothCurve1 = SmoothCurveGenerator1::generateSmoothCurve(downloadPoints);
    smoothCurve2 = SmoothCurveGenerator2::generateSmoothCurve(downloadPoints);
    // 连接点创建非平滑曲线曲线
    nonSmoothCurve = QPainterPath();
    nonSmoothCurve.moveTo(downloadPoints[0]);
    for (int i = 1; i < downloadPoints.size(); ++i) {
        nonSmoothCurve.lineTo(downloadPoints[i]);
    }*/


    //upload
    QList<QPointF> uploadPoints;
    m_uploadSpeedList->append(m_sentRateBytes);
    if (m_uploadSpeedList->size() > m_pointsCount) {
        m_uploadSpeedList->pop_front();
    }
    //计算出上传速度中最大的值
    long uploadMaxHeight = 0;
    for (int i = 0; i < m_uploadSpeedList->size(); i++) {
        if (m_uploadSpeedList->at(i) > uploadMaxHeight) {
            uploadMaxHeight = m_uploadSpeedList->at(i);
        }
    }

    for (int i = 0; i < m_uploadSpeedList->size(); i++) {
        if (uploadMaxHeight < m_netMaxHeight) {
            uploadPoints.append(QPointF(i * m_pointSpace, m_uploadSpeedList->at(i)));
        }
        else {
            uploadPoints.append(QPointF(i * m_pointSpace, m_uploadSpeedList->at(i) * m_netMaxHeight / uploadMaxHeight));
        }
    }
    m_uploadPath = SmoothCurveGenerator::generateSmoothCurve(uploadPoints);

    repaint();
}


void NetworkFlow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);//反走样,绘制出来的线条会出现锯齿

    QFont font = painter.font();
    font.setPointSize(22);
    font.setWeight(QFont::Light);
    painter.setFont(font);
    painter.setPen(QPen(QColor("#303030")));
    painter.drawText(QRect(rect().x() + 2, rect().y(), rect().width() - 4, rect().height()), Qt::AlignLeft | Qt::AlignTop, tr("Network"));//绘制文本

    setFontSize(painter, 9);
    QFontMetrics fm = painter.fontMetrics();
    QString downloadTitle = QString("%1 %2").arg(tr("正在接收")).arg(formatNetworkRate(m_recvRateBytes));
    QString downloadContent = QString("%1 %2").arg(tr("总计已接收")).arg(formatNetwork(m_recvTotalBytes));//接收
    QString uploadTitle = QString("%1 %2").arg(tr("正在发送")).arg(formatNetworkRate(m_sentRateBytes));
    QString uploadContent = QString("%1 %2").arg(tr("总计已发送")).arg(formatNetwork(m_sentTotalBytes));//发送

    int rateW = std::max(fm.width(downloadTitle), fm.width(uploadTitle));

    //download
    painter.setOpacity(1);
    QSvgRenderer uploadRender(QString("://res/download.svg"));
    QImage uploadimage(20, 20, QImage::Format_ARGB32);
    uploadimage.fill(QColor("#1E90FF"));
    QPainter uploadPainter(&uploadimage);
    uploadPainter.setRenderHint(QPainter::Antialiasing, true);
//    uploadPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);//默认模式,源的alpha将目标顶部的像素混合
    uploadRender.render(&uploadPainter);
    painter.drawImage(rect().x() + 2, rect().y() + 40, uploadimage);
//    uploadPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);//输出是源，其中alpha被目标的值减少   设置画刷的组合模式CompositionMode_SourceIn这个模式为目标图像在下
//    uploadPainter.end();

    setFontSize(painter, 9);
    painter.setPen(QPen(QColor("#1E90FF")));
    painter.drawText(QRect(rect().x() + 20 + 5, rect().y() + 40, fm.width(downloadTitle), rect().height()), Qt::AlignLeft | Qt::AlignTop, downloadTitle);
    painter.drawText(QRect(rect().x() + 20 + rateW + 14, rect().y() + 40, fm.width(downloadContent), rect().height()), Qt::AlignLeft | Qt::AlignTop, downloadContent);

    //upload
    QSvgRenderer downloadRender(QString("://res/upload.svg"));
    QImage downloadimage(20, 20, QImage::Format_ARGB32);
    downloadimage.fill(QColor("#FF0000"));
    QPainter downloadPainter(&downloadimage);
    downloadPainter.setRenderHint(QPainter::Antialiasing, true);
//    downloadPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);//默认模式,源的alpha将目标顶部的像素混合
    downloadRender.render(&downloadPainter);
    painter.drawImage(rect().x() + 2, rect().y() + 70, downloadimage);
//    downloadPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);//输出是源，其中alpha被目标的值减少   设置画刷的组合模式CompositionMode_SourceIn这个模式为目标图像在下
//    downloadPainter.end();

    setFontSize(painter, 9);
    painter.setPen(QPen(QColor("#FF0000")));
    painter.drawText(QRect(rect().x() + 20 + 5, rect().y() + 70, fm.width(uploadTitle), rect().height()), Qt::AlignLeft | Qt::AlignTop, uploadTitle);
    painter.drawText(QRect(rect().x() + 20 + rateW + 14, rect().y() + 70, fm.width(uploadContent), rect().height()), Qt::AlignLeft | Qt::AlignTop, uploadContent);

    //网格背景
    painter.setBrush(QBrush(QColor("#AFEEEE")));
    painter.setRenderHint(QPainter::Antialiasing, false);
    QPen framePen;
    painter.setOpacity(0.1);
    framePen.setColor(QColor("#303030"));
    framePen.setWidth(0.5);
    painter.setPen(framePen);

    int penSize = 1;
    int gridX = rect().x() + penSize;
    int gridY = rect().y() + 95;

    int gridWidth = rect().width() - penSize * 2;
    int gridHeight = 100;

    QPainterPath framePath;
    framePath.addRect(QRect(gridX, gridY, gridWidth, gridHeight));
    painter.drawPath(framePath);

    //网格
    QPen gridPen;
    QVector<qreal> dashes;
    qreal space = 4;
    dashes << 1 << space << 3 << space << 9 << space << 27 << space << 9 << space;
    painter.setOpacity(0.05);
    gridPen.setColor(QColor("#303030"));
    gridPen.setWidth(0.5);
    gridPen.setDashPattern(dashes);
    painter.setPen(gridPen);
    int gridLineX = gridX;
    while (gridLineX < gridX + gridWidth - 100) {
        gridLineX += 100;
        painter.drawLine(gridLineX, gridY + 1, gridLineX, gridY + gridHeight - 1);
    }
    painter.setPen(gridPen);

    m_gridY->clear();
    int gridLineY = gridY;
    while (gridLineY < gridY + gridHeight - 20) {
        gridLineY += 20;
        painter.drawLine(gridX + 1, gridLineY, gridX + gridWidth - 1, gridLineY);
        m_gridY->append(gridLineY);
    }

    painter.setOpacity(1);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //利用translate()函数进行平移变换
    painter.translate((rect().width() - m_pointsCount * m_pointSpace - 2) / 2 + 6, 175);//将坐标第原点移动到该点
    //利用scale()函数进行比例变换，实现缩放效果
    painter.scale(1, -1);//将横坐标扩大1倍,将纵坐标缩小1倍
    //使用QPainterPath画贝塞尔曲线
    painter.setPen(QPen(QColor("#1E90FF"), 1));
    painter.setBrush(QBrush());
    painter.drawPath(m_downloadPath);//绘制前面创建的path:m_downloadPath

    painter.translate(0, -8);//将点（0，-8）设为原点
//    painter.translate(0, 2);
//    painter.scale(1, -1);
    painter.setPen(QPen(QColor("#FF0000"), 1));
    painter.setBrush(QBrush());
    painter.drawPath(m_uploadPath);
}
