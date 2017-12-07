#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include "cydevice.h"
#include "imageprocess.h"
#include <QtWidgets>
//#include <opencv2\highgui\highgui.hpp>
//#include <opencv2\imgproc\imgproc.hpp>
class ImageModel: public QObject
{
	Q_OBJECT

public:
	ImageModel(QWidget *mainWindow, int height, int width);
	~ImageModel();
	
	bool openUSBCamera();
	void closeUSBCamera();
	void readData();

	void setResolution(int height, int width);
	void saveData();

	void whetherPausingUSBCamera(bool flag);

	void changeWidthTo8bitsPerPixel(bool flag);
	void changeImageToColor(bool flag);
	void changeResolution(int width, int height, int req);
	void changeResolution(int width, int height, int req, long sizePerXfer, int xferQueueSize, int timeOut);
	
	void sendSettingCommand(uchar u1, uchar u2, uchar u3, uchar u4); 

	void setSavingPath(QString path);
	void takeImage();
	void initialImageFifo();

	void setHorizontalLine();
	void setVerticalLine();
	void changeResolutionDone();
	void unableImageprocessGetPoint();
	void setResolutionType(int);
	void takeImage(bool darkFlag, int);
	void countPRNU();
	void countDSNU();

signals:
	void resolutionChanged(int width, int height, int req, long sizePerXfer, int xferQueueSize, int timeOut);

public slots:
    void getPointPos(int , int, int, int, int);

private:
	CyDevice m_camera;
	ImageProcess m_imageProcess;
	QWidget *m_mainWindow;
	int m_imageHeight;
	int m_imageWidth;

	QThread m_receiveThread;
	QThread m_imageProcessThread;

	uchar** m_imageDataSavingSpace;
	//bool changeResolutionDoneFlag;

	
};

#endif