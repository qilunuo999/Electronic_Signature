#include "include/utils/signalcontroller.h"

SignalController::SignalController(int signal, QVector<QImage>** imageList,   int numPages[], QString path, QScrollArea* tempStorage, QListWidget *lWidget, double pictureRate[])
{
    if(model == nullptr)
    {
        model = new BusinessModel();
    }

    switch (signal)
    {
    case OPEN_FILE:model->openFile(path, imageList,tempStorage, numPages, lWidget, pictureRate); break;
    case SEARCH :break;
    case NOTES:break;
    case VIEW_PROPERTIES:break;
    case PRINTING:break;
    //case MAKE_SEAL:model->makeSeal();break;
    case VIEW_TOOLBAR:break;
    case DETAILS:break;
    case VIEW_CERTIFICATE:break;
    case VIEW_TIMESTAMP:break;
    default:break;
    }
}

SignalController::SignalController(int signal, QString path, QString savepath)
{
    if(model == nullptr)
    {
        model = new BusinessModel();
    }

    switch (signal)
    {
    case SAVE:model->saveFile(path); break;
    case SAVE_AS:model->saveAs(path, savepath);break;
    case SOFT_INFO:break;
    case USER_MANUAL:break;
    default:break;
    }
}

SignalController::SignalController(int signal, QVector<QImage>** imageList, int pageNum, double multipleRate, QScrollArea* tempStorage, QString filePath)
{
    if(model == nullptr)
    {
        model = new BusinessModel();
    }

    switch (signal)
    {
    //case JUMP_PAGE:model->jumpPage(pageNum, imageList,tempStorage, filePath);break;
    case ZOOM:model->zoom(imageList, pageNum, multipleRate, tempStorage);break;
    case NEXT_PAGE:model->jumpPage(pageNum, imageList,tempStorage, filePath);break;
    case PREVIOUS_PAGE:model->jumpPage(pageNum, imageList,tempStorage, filePath);break;
    //case ENLARGE:model->zoom(imageList, pageNum, multipleRate, tempStorage);break;
    case NARROW:model->zoom(imageList, pageNum, multipleRate, tempStorage);break;
    case FULL_SCREEN:model->zoom(imageList, pageNum, multipleRate, tempStorage);break;
    case ACTUAL_SIZE:model->zoom(imageList, pageNum, multipleRate, tempStorage);break;
    default:break;
    }
}

SignalController::SignalController(int signal, QVector<QImage>** imageList, QScrollArea* tempStorage,int x, int y, QString pdfFile, QString imagePath, QString passwd, int pageNum)
{
    if(model == nullptr)
    {
        model = new BusinessModel();
    }

    switch (signal)
    {
    case SEAL:model->seal(imageList, tempStorage, x, y, pdfFile, imagePath, passwd, pageNum);break;
    case VERTIFICATION:break;
    case UNSEAL:break;
    }
}

SignalController::~SignalController()
{
}
