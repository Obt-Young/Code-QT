//在任何一个工程中引入这个函数，然后调用即可

void popUpVideoPage(QString url)
{


      VideoPlayer *tmp = new VideoPlayer();
      tmp->resize(560,480);
      tmp->setUrl(url);
      tmp->show();
      tmp->play();
}



