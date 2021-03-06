// THIS IS A SHORT SAMPLE OF USING FFMPEG WITH QT
// IT DISPLAYS RANDOM FRAME FROM THE .MP4 FILES IN
// THE CURRENT DIRECTORY
//
// ALL THE RELEVANT CODE IS IN THIS FILE

#include "mainwindow.h"
#include "ui_mainwindow.h"


// SHOW A RANDOM FRAME FROM THE VIDEOS IN THE CURRENT DIRECTORY
void MainWindow::showRandomFrame() {
    AVFormatContext *pFormatCtx = nullptr;
    AVFrame *pFrameRGB = nullptr;
    AVFrame *pFrame = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    AVCodec *pCodec = nullptr;
    uint8_t *buffer = nullptr;

    try {
        // SELECT AN .MP4 FILES AT RANDOM FROM THE CURRENT DIRECTORY
        std::vector <std::string> v;
        boost::filesystem::directory_iterator end_itr; // Default ctor yields past-the-end
        for( boost::filesystem::directory_iterator i(boost::filesystem::current_path()); i != end_itr; ++i )
        {
            if (i->status().type() == boost::filesystem::file_type::regular_file && i->path().extension() == ".mp4")
                v.push_back(i->path().native());
        }
        if (v.size() < 1) return;
        boost::random::uniform_int_distribution<> v_dist(0, v.size() - 1);
        const std::string &path = v[v_dist(gen)];

        // OPEN VIDEO USING FFMPEG
        av_register_all();
        if(avformat_open_input(&pFormatCtx, path.c_str(), NULL, NULL)!=0)
            throw std::logic_error("Couldn't open file");
        if(avformat_find_stream_info(pFormatCtx, NULL)<0)
            throw std::logic_error("Couldn't find stream information");

        // Find the first video stream
        int videoStream=-1;
        for(int i=0; i<pFormatCtx->nb_streams; i++)
          if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
            videoStream=i;
            break;
          }
        if(videoStream==-1)
            throw std::logic_error("Didn't find a video stream");

        pCodecCtx=pFormatCtx->streams[videoStream]->codec;
        pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
        if(pCodec==NULL) {
            throw std::logic_error("Codec not found");
        }
        if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
            throw std::logic_error("Could not open codec");

        pFrame=av_frame_alloc();
        pFrameRGB=av_frame_alloc();
        if(pFrameRGB==NULL)
            throw std::logic_error("Could not alloc RGB frame");
        int numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
                        pCodecCtx->height);
        buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

        avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
               pCodecCtx->width, pCodecCtx->height);

        // PICK A FRAME AT RANDOM USING BOOST::RANDOM
        boost::random::uniform_int_distribution<> dist(0, pFormatCtx->duration);

        av_seek_frame(pFormatCtx, -1, dist(gen), 0);
        avcodec_flush_buffers(pCodecCtx);

        AVPacket packet;
        while(av_read_frame(pFormatCtx, &packet)>=0) {
          int frameFinished = 0;
          if(packet.stream_index==videoStream) {
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
          }
          av_free_packet(&packet);
          if(frameFinished && pFrame->key_frame)
              break;
        }



        // CONVERT FRAME TO RGB
        SwsContext *swsContext = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
        sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
        sws_freeContext(swsContext);

        // DISPLAY IN QT LABEL
        QPixmap bmp = QPixmap::fromImage(QImage((uint8_t*)buffer, pCodecCtx->width, pCodecCtx->height, QImage::Format_RGB888));
        ui->label->setPixmap(bmp);
    } catch(std::logic_error &e) {
        std::cerr << "Error displaying frame " << e.what() << std::endl;
    }

    av_free(buffer);
    av_free(pFrameRGB);
    av_free(pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gen(std::time(0))
{
    ui->setupUi(this);

    // SHOW RANDOM VIDEO FRAME IN THIS WINDOW EVERY SECOND
    showRandomFrame();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showRandomFrame()));
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}
