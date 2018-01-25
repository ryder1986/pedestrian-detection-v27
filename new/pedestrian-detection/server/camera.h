#ifndef CAMERA_H
#define CAMERA_H
#include <QObject>
#include "config.h"
#include "pd.h"
#include "videosrc.h"
#include "videoprocessor.h"
#include "processeddatasender.h"
#include "camera.h"
//#include "server.h"
//class ProcessedDataSender;
#include <thread>
using namespace std;
class Camera:public QObject
{
    Q_OBJECT
    //   friend class  ProcessedDataSender;
    typedef struct data{
        VideoSrc *p_src;
        VideoProcessor *p_worker;
        bool quit;
        QString url;
        //ProcessedDataSender *sender;
        Camera *ts;
        QByteArray rst;
        mutex lock;
        //   bool output;
        //   int index;
    }data_t;
    data_t d;
    std::thread *p_thread;
    typedef CameraConfiguration::camera_config_t camera_config;
    QTimer tmr;

    //  VideoProcessor *_prc;
public:
    QString get_url()
    {
        return d.url;
    }

    static void fun(data_t *p_data)
    {

        Mat mt;
        bool ret;
        p_data->p_src=new VideoSrc(p_data->url.toStdString().data());
        p_data->p_worker=new VideoProcessor;
        //  p_data->sender=  new ProcessedDataSender;
        QByteArray rst;
        string str;
        while(!p_data->quit)
        {
            // p_data->sender=  ProcessedDataSender::get_instance();

            ret=p_data->p_src->fetch_frame(mt);
            if(ret){
                rst.clear();
                //   p_data->p_worker->work_inside(mt,rst,p_data->url);
                //  p_data->p_worker->work_inside(mt,str);

                //   this_thread::sleep_for(chrono::milliseconds(100));
                //  rst.clear();rst.append(p_data->index);
                //                if(p_data->output&&rst.length()){
                //                    p_data->sender->send(rst);
                //                    rst.clear();
                //


                //#if IS_UNIX
#if 1
                p_data->p_worker->work(mt,str);
                QByteArray ss(str.data());
                rst=ss;
#else
                p_data->p_worker->work_inside(mt,rst,p_data->url);
#endif
                if(rst.length()>0){
                    p_data->lock.lock();
                    p_data->rst=rst;
                    p_data->lock.unlock();
                }}
            else{
                this_thread::sleep_for(chrono::milliseconds(30));
            }
        }
        prt(info,"camera %s exit",p_data->url.toStdString().data());
        //   delete   p_data->sender;
        delete p_data->p_src;
        delete p_data->p_worker;
    }
    //    void set_output(bool op,int index)
    //    {
    //        d.output=op;
    //        d.index=index;
    //    }

    Camera(camera_config cfg)
    {
        //d.output=false;
        //d.index=-1;
        //     d.output=true;
        //        d.sender=  ProcessedDataSender::get_instance();
        d.ts=this;
        d.url=cfg.ip;
        d.quit=false;
        p_thread=new std::thread(fun,&d);
        connect(&tmr,SIGNAL(timeout()),this,SLOT(check_rst()));
        tmr.start(10);
        //     d.sender=ProcessedDataSender::get_instance();
    }
    ~Camera()
    {

        d.quit=true;
        p_thread->join();
        delete p_thread;
        // delete d.sender;
    }
//    QByteArray get_rst()
//    {
//        QByteArray ba;
//        ba.clear();
//        d.lock.lock();
//        ba=d.rst;
//        d.rst.clear();
//        d.lock.unlock();
//        return ba;
//    }
public slots:
    void check_rst()
    {
      //  QByteArray ba;
      //  ba.clear();
        d.lock.lock();
      //  ba=d.rst;
        if(d.rst.length()>0)
            emit send_rst(d.rst);
        d.rst.clear();
        d.lock.unlock();
    }

signals:
 //   void work_done(QByteArray process_rst);
    void send_rst(QByteArray process_rst);
   };

class CameraManager{
    friend class ClientSession;
    int test_int;
    mutex cfg_lock;
public:
    CameraManager(CameraManager &m)
    {
        prt(fatal,"copy is not supported now");
        exit(1);
#ifdef DISPLAY_VIDEO
#ifdef IS_UNIX
        XInitThreads();
#endif
#endif

    }
#if 1
    static CameraManager &GetInstance()
    {
        static CameraManager m;

        return m;
    }
private:
#endif
    CameraManager()
    {
#ifdef DISPLAY_VIDEO
#ifdef IS_UNIX
        XInitThreads();
#endif
#endif
        p_cfg=new CameraConfiguration("config.json");
        start_all();
    }

public:
    int cam_num()
    {
        return cameras.size();
    }
    ~CameraManager()
    {
        //        stop_all();
        //        delete p_cfg;
    }

    void start_all()
    {
        foreach (CameraConfiguration::camera_config_t tmp, p_cfg->cfg.camera) {
            Camera *c=new Camera(tmp);
            cameras.push_back(c);
        }
    }

    void stop_all()
    {
        foreach (Camera *tmp, cameras) {
            delete tmp;
            cameras.removeOne(tmp);
        }
    }

    void add_camera(const char *cfg_buf)
    {
        cfg_lock.lock();
        p_cfg->set_config(cfg_buf);
        Camera *c=new Camera(p_cfg->cfg.camera[p_cfg->cfg.camera_amount-1]);
        //    cameras.push_back(c);
        cameras.append(c);
        cfg_lock.unlock();
    }
    void del_camera(const char *cfg_buf,const int index)
    {
        cfg_lock.lock();
        p_cfg->set_config(cfg_buf);
        delete cameras[index-1];
        cameras.removeAt(index-1);
        cfg_lock.unlock();
    }
    void del_camera(const int index)
    {
        cfg_lock.lock();
        p_cfg->del_camera(index);
        Camera *cm=cameras[index-1];
        //   prt(info,"delete %s",cm->d.p_src->get_url());
        delete cm;//////////////////////////TODO
        cameras.removeAt(index-1);
        cfg_lock.unlock();
        //   delete cm;
    }
    void mod_camera(const char *cfg_buf,const int index)
    {
        cfg_lock.lock();
        p_cfg->set_config(cfg_buf);
        //prt(info,"modify cam  %d",index);
        QString s;
        if(cameras[index-1]->get_url().compare(p_cfg->cfg.camera[index-1].ip))
        {
            prt(info, "url modifyed to %s",p_cfg->cfg.camera[index-1].ip.toStdString().data());
            delete cameras[index-1];
            cameras[index-1]=new Camera(p_cfg->cfg.camera[index-1]);

        }else{
            prt(info, "alg modifyed (%s)",p_cfg->cfg.camera[index-1].ip.toStdString().data());
        }

        cfg_lock.unlock();
    }
    CameraConfiguration *p_cfg;
private:

private:
    QList<Camera *> cameras;

};

#endif // CAMERA_H
