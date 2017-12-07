#ifndef CYDEVICE_H
#define CYDEVICE_H

#include <Windows.h>
#include <QtCore>
#include "CyAPI.h"
#include "imagefifo.h"

class CyDevice: public QObject
{
	Q_OBJECT

public:
	CyDevice(int height, int width);
	~CyDevice();

	bool openDevice(HANDLE hnd);
	void closeDevice();
	void setDataSavingSpace(UCHAR **data, int size);
	void enableReceving();
	void disableReceving();
	
	bool sendControlCode(int code);
	bool sendRequestCode(int code, uchar *buf, LONG bufLen);

	void setWidth(int width);
	void setHeight(int height);

	//void setPauseFlag(bool flag);

	void changeWidthTo8bitsPerPixel();
	void changeWidthTo16bitsPerPixel();

	void changeResolution(int width, int height, int req);

	void receiveData(LONG sizePerXfer, int xferQueueSize, int timeOut);

	bool isReceving();
	void waitChangeDone();

signals:
	//void completeFrameTransmission(unsigned char *data);
	void completeFrameTransmission();
	//void changeResolutionDone();

public slots:
	void receiveData();
	void changeResolution(int width, int height, int req, long sizePerXfer, int xferQueueSize, int timeOut);

private:
	int m_whichBuffer;
	int m_height;
	int m_width;
	LONG m_size;
	UCHAR **m_dataBuffer;
	ImageData *m_imageBuffer;
	int m_bufferSize;
	volatile bool m_recevingFlag;   //子线程会不断改写该状态，主线程每次判断需要直接从内存中读取，而不是优化读取
	//bool m_pauseFlag;
	int m_bitsPerPixel;
	bool m_widthPerPixelChanged;

	QMutex m_mutex;

	CCyUSBDevice *m_device;
	CCyUSBEndPoint *m_dataInEndPoint;
	CCyControlEndPoint *m_controlEndPoint;

	



};


#endif