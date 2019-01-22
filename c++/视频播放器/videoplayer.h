#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <qmediaplayer.h>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE
class QPushButton;
class QSlider;
class QLabel;
class QUrl;
QT_END_NAMESPACE

/********************播放器********************/
class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    friend class VideoPlayerManager;
    ~VideoPlayer();

private:
    VideoPlayer(QWidget *parent = 0);   //由VideoPlayerManager统一分配，不对外开放

public slots:
    void play();
    void setUrl(const QUrl &url);

private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition(int position);
    void updatecurrentTime(qint64 tmpSec);
    void gettotalTime();

private://基本组件
    QMediaPlayer mediaPlayer;       //媒体资源管理类
    QPushButton *playButton;        //控制按键
    QLabel *currentTime;            //当前播放进度
    QSlider *positionSlider;        //进度条
    QLabel *totalTime;              //总时间
    QPushButton *closeButton;       //关闭窗口的按键

//protected:
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void mouseReleaseEvent(QMouseEvent *event);
//    QPoint lastlocation;

};

/********************播放器管理类********************/
class VideoPlayerManager:public QObject
{
    Q_OBJECT
public:
    static VideoPlayerManager* getVideoPlayerManager();
    ~VideoPlayerManager();

private:
    VideoPlayerManager();

public:
    VideoPlayer*  getCommonPlayer();
    VideoPlayer*  getNewPlayer();

private:
    QList<VideoPlayer*> playerList;

};

#endif
