#include "videoplayer.h"
#include <QtWidgets>
#include <qvideowidget.h>
#include <qvideosurfaceformat.h>
#include "trace.h"

static VideoPlayerManager* myVideoPlayerManager = NULL;
static VideoPlayer* commonPlayer = NULL;


VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
    , mediaPlayer(0, QMediaPlayer::VideoSurface)
    , playButton(0)
    , currentTime(0)
    , positionSlider(0)
    , totalTime(0)
{
    /*======================窗口外观======================*/
    setWindowFlags(windowFlags()|Qt::Tool);            //隐藏任务栏图标
    setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);  //始终置顶
    setWindowModality(Qt::ApplicationModal);              //抢占顶层，截获所有动作

    /*======================组件======================*/
    QVideoWidget *videoWidget = new QVideoWidget;   /****mediaPlayer的容器****/

    /****关闭窗口按键(暂不使用,后期优化需要把外边框去掉时,再开放)****/
    //closeButton = new QPushButton(this);
    //closeButton->setFlat(true);
    //QString closeButtonStyleSheet = "QPushButton{background-color:transparent; background-clip: padding;height:24px; width:24px; background-position:center; background-image:url(:/images/popup/popup_exit_n.png);}"
    //                                "QPushButton:hover:{background-image:url(:/images/popup/popup_exit_h.png);}";
    //closeButton->setStyleSheet(closeButtonStyleSheet);
    //connect(closeButton,&QPushButton::clicked,this,&VideoPlayer::close);
    /****播放按钮****/
    playButton = new QPushButton(this);
    playButton->setEnabled(false);
    playButton->setFlat(true);
    playButton->resize(playButton->width(),50);
    connect(playButton, &QPushButton::clicked,this, &VideoPlayer::play);
    /****当前进度(时间)****/
    currentTime = new QLabel(this);
    currentTime->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    /****进度条****/
    positionSlider = new QSlider(Qt::Horizontal,this);
    positionSlider->setRange(0, mediaPlayer.duration()/1000);
    QString sliderStyleSheet = "QSlider{height:8px;}"
                               "QSlider::handle:horizontal{height:8px; width:8px; border-radius:4px; margin:-2px 0; background:\"#57ABEB\";}"
                               "QSlider::groove:horizontal{height:4px; border-radius:1px; position:absolute; left:4px; right:4px;}"
                               "QSlider::add-page:horizontal{height:4px; border-raidus:1px; background:white;}"
                               "QSlider::sub-page:horizontal{height:4px; border-raidus:1px; background:\"#57ABEB\";}";
    positionSlider->setStyleSheet(sliderStyleSheet);
    connect(positionSlider, &QAbstractSlider::sliderMoved,this, &VideoPlayer::setPosition); //仅限拖拽动作的信号,自动更新不会触发这个信号
    connect(positionSlider, &QAbstractSlider::valueChanged,this,&VideoPlayer::updatecurrentTime);   //自动更新信号
    /****总进度(时间)****/
    totalTime = new QLabel(this);
    totalTime->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    gettotalTime();

    /*======================布局管理======================*/
//    QBoxLayout *ly_title = new QHBoxLayout;
//    ly_title->addStretch();
//    ly_title->addWidget(closeButton);
    QBoxLayout *ly_video = new QHBoxLayout;
    ly_video->addWidget(videoWidget);
    QBoxLayout *ly_controllers = new QHBoxLayout;
    ly_controllers->addWidget(playButton);
    ly_controllers->addWidget(currentTime);
    ly_controllers->addWidget(positionSlider);
    ly_controllers->addWidget(totalTime);
    QBoxLayout *ly_main = new QVBoxLayout(this);
//    ly_main->addLayout(ly_title);
    ly_main->addLayout(ly_video);
    ly_main->addLayout(ly_controllers);
    setLayout(ly_main);

    mediaPlayer.setVideoOutput(videoWidget);
    connect(&mediaPlayer, &QMediaPlayer::stateChanged,this, &VideoPlayer::mediaStateChanged);
    connect(&mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::positionChanged);
    connect(&mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayer::durationChanged);

}

VideoPlayer::~VideoPlayer()
{
    LOG(INFO)<<"[~VideoPlayer]close one VideoPlayer";
}

void VideoPlayer::setUrl(const QUrl &url)
{
    mediaPlayer.setMedia(url);
    playButton->setEnabled(true);
}

void VideoPlayer::play()
{
    switch(mediaPlayer.state()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer.pause();
        break;
    default:
        mediaPlayer.play();
        break;
    }
}

void VideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    QString button_style="";
    switch(state) {
    case QMediaPlayer::PlayingState:
        button_style="QPushButton{background-color:transparent; background-position:center;background-image:url(:/images/history/history_pause.png);}"
                     "QPushButton:hover{background-image:url(:/images/history/history_pause_h.png)}";
        break;
    default:
        button_style="QPushButton{background-color:transparent; background-position:center;background-image:url(:/images/history/history_play.png);}"
                     "QPushButton:hover{background-image:url(:/images/history/history_play_h.png)}";
        break;
    }
    playButton->setStyleSheet(button_style);
}

void VideoPlayer::positionChanged(qint64 position)
{
    positionSlider->setValue(position);
}

void VideoPlayer::durationChanged(qint64 duration)
{
    positionSlider->setRange(0, duration);
    gettotalTime();
}

void VideoPlayer::setPosition(int position)
{
    mediaPlayer.setPosition(position);
    updatecurrentTime(positionSlider->value());
}

void VideoPlayer::updatecurrentTime(qint64 position)
{
    QString tStr;
    qint64 tmpSec = position/1000;
    if (tmpSec) {
        QTime tmpcurrentTime((tmpSec/3600)%60, (tmpSec/60)%60, tmpSec%60, (tmpSec*1000)%1000);
        QString format = "mm:ss";
        if (tmpSec > 3600)
            format = "hh:mm:ss";
        tStr = tmpcurrentTime.toString(format);
    }else{
        tStr = "00:00";
    }
    currentTime->setText(tStr);
}

void VideoPlayer::gettotalTime()
{
    QString tStr;
    qint64 tmpduration = mediaPlayer.duration()/1000;
    if(tmpduration) {
        QTime tmptotalTime((tmpduration/3600)%60, (tmpduration/60)%60, tmpduration%60, (tmpduration*1000)%1000);
        QString format = "mm:ss";
        if (tmpduration > 3600)
            format = "hh:mm:ss";
        tStr = tmptotalTime.toString(format);
    }else{
        tStr = "00:00";
    }
    totalTime->setText(tStr);
}

/*============ 隐藏外边框场景下,重新实现拖动动作 ============*/
//void VideoPlayer::mousePressEvent(QMouseEvent *event){
//    LOG(INFO)<<"[mousePressEvent] ENTER";
//    if(event->button() == Qt::LeftButton){
//        lastlocation = event->globalPos();
//    }
//    LOG(INFO)<<"[mousePressEvent] EXIT";
//}

//void VideoPlayer::mouseMoveEvent(QMouseEvent *event){
//    LOG(INFO)<<"[mouseMoveEvent] ENTER";
//    if(event->button()==Qt::LeftButton){
//        int dx = event->globalX()-lastlocation.x();
//        int dy = event->globalY()-lastlocation.y();
//        lastlocation = event->globalPos();
//        move(x()+dx,y()+dy);
//    }
//    LOG(INFO)<<"[mouseMoveEvent] EXIT";
//}

//void VideoPlayer::mouseReleaseEvent(QMouseEvent *event)
//{
//    int dx = event->globalX() - lastlocation.x();
//    int dy = event->globalY() - lastlocation.y();
//    move(x()+dx, y()+dy);
//}
/*============ 隐藏外边框场景下,重新实现拖动动作 ============*/


VideoPlayerManager::VideoPlayerManager(){
    LOG(INFO)<<"[VideoPlayerManager] init parameters";
    playerList.clear();
}

VideoPlayerManager::~VideoPlayerManager(){
    LOG(INFO)<<"[~VideoPlayerManager] release parameters";
    if(false == playerList.isEmpty())
    {
        QList<VideoPlayer *>::iterator it = playerList.begin();
        if(it!=playerList.end()){
            delete *it;
            it++;
        }
    }
}

VideoPlayerManager* VideoPlayerManager::getVideoPlayerManager(){
    if(myVideoPlayerManager==NULL){
        LOG(INFO)<<"myVideoPlayerManager is NULL";
        myVideoPlayerManager = new VideoPlayerManager();
        return myVideoPlayerManager;
    }
    else{
        LOG(INFO)<<"myVideoPlayerManager is NOT NULL";
        return myVideoPlayerManager;
    }
}

VideoPlayer* VideoPlayerManager::getCommonPlayer(){
    if(commonPlayer!=NULL){
        LOG(INFO)<<"commonPlayer is ALREADY in use . Replace url and play";
        return commonPlayer;
    }
    else{
        LOG(INFO)<<"commonPlayer is NOT in use . New a player and play";
        commonPlayer = getNewPlayer();
        return commonPlayer;
    }
}

VideoPlayer* VideoPlayerManager::getNewPlayer(){
    VideoPlayer* tmp = new VideoPlayer();
    if(tmp==NULL){
        LOG(INFO)<<"[getNewPlayer] new VideoPlayer ERROR.";
        return NULL;
    }

    playerList.append(tmp);
    return tmp;
}

