#include "imagemodel.h"
#include "imagefifo.h"
#define FIFO_SIZE 2   //该数值必须大于等于2

ImageModel::ImageModel(QWidget *mainWindow, int height, int width)
	:m_mainWindow(mainWindow),
	m_imageHeight(height),
	m_imageWidth(width),
	m_camera(height, width),
	m_imageProcess(height, width)
{
	initialImageFifo();

	m_camera.setDataSavingSpace(m_imageDataSavingSpace, FIFO_SIZE);

	m_camera.moveToThread(&m_receiveThread);

	m_imageProcess.moveToThread(&m_imageProcessThread);

	connect(&m_receiveThread, SIGNAL(started()), &m_camera, SLOT(receiveData()));
	connect(&m_imageProcessThread, SIGNAL(started()), &m_imageProcess, SLOT(dataToImage()));

	connect(&m_imageProcess, SIGNAL(showImage(QPixmap)), m_mainWindow, SLOT(updateImage(QPixmap)));
	//connect(&m_imageProcess, SIGNAL(showImage(QImage)), m_mainWindow, SLOT(updateImage(QImage)));
	connect(&m_camera, SIGNAL(completeFrameTransmission()), m_mainWindow, SLOT(countReceiveFrames()));
	connect(this, &ImageModel::resolutionChanged, &m_camera, static_cast<void (CyDevice::*)(int, int, int, LONG, int ,int)>(&CyDevice::changeResolution));
	//connect(&m_camera, SIGNAL(completeFrameTransmission(unsigned char *, bool)), &m_imageProcess, SLOT(dataToImage(unsigned char *, bool)), Qt::DirectConnection);
	//connect(&m_camera, SIGNAL(completeFrameTransmission(unsigned char *, bool)), &m_imageProcess, SLOT(dataToImage(unsigned char *, bool)));
	connect(&m_imageProcess, SIGNAL(sendPointData(int, int, int)), m_mainWindow, SLOT(getPointData(int, int, int)));
	connect(&m_imageProcess, SIGNAL(takeContImageDone()), m_mainWindow, SLOT(setNonuniformityEnable()));
	connect(&m_imageProcess, SIGNAL(sendPRNU(double)), m_mainWindow, SLOT(showPRNU(double)));
	connect(&m_imageProcess, SIGNAL(sendPRNUWrongSignal(bool)), m_mainWindow, SLOT(wrongTipsOfPRNU(bool)));
	connect(&m_imageProcess, SIGNAL(sendDSNU(double)), m_mainWindow, SLOT(showDSNU(double)));
	connect(&m_imageProcess, SIGNAL(sendDSNUWrongSignal(bool)), m_mainWindow, SLOT(wrongTipsOfDSNU(bool)));
}

ImageModel::~ImageModel()
{
	if (m_receiveThread.isRunning())
	{

		m_camera.disableReceving();
		m_receiveThread.quit();
		m_receiveThread.wait();

		m_camera.closeDevice();
	}
	if (m_imageProcessThread.isRunning())
	{
		m_imageProcess.disableImageProcess();

		m_imageProcessThread.quit();
		m_imageProcessThread.wait();
	}
	QThreadPool::globalInstance()->waitForDone();

	for (int i = 0; i < FIFO_SIZE; ++i)
	{
		delete[] m_imageDataSavingSpace[i];
	}

	delete[] m_imageDataSavingSpace;
}

bool ImageModel::openUSBCamera()
{
	if (m_camera.openDevice((HANDLE)(m_mainWindow->winId())))
	{
		
		return true;
	}
	else
	{
		return false;
	}
}

void ImageModel::closeUSBCamera()
{
	if (m_receiveThread.isRunning())
	{
		m_camera.disableReceving();
		m_imageProcess.disableImageProcess();

		m_receiveThread.quit();
		m_receiveThread.wait();

		m_camera.closeDevice();

		m_imageProcessThread.quit();
		m_imageProcessThread.wait();
	}
}

void ImageModel::readData()
{
	if (!m_imageProcessThread.isRunning())
	{
		m_imageProcess.enableImageProcess();
		m_imageProcessThread.start();
	}
	m_camera.enableReceving();
	m_receiveThread.start();
	
}

void ImageModel::setResolution(int height, int width)
{
	m_camera.setHeight(height);
	m_camera.setWidth(width);
	m_imageProcess.setHeight(height);
	m_imageProcess.setWidth(width);
}

void ImageModel::saveData()
{
	m_imageProcess.enableSaveData();
}

void ImageModel::whetherPausingUSBCamera(bool trueOrFalse)
{
	//m_camera.setPauseFlag(trueOrFalse);
	m_imageProcess.setPauseFlag(trueOrFalse);
}

void ImageModel::changeWidthTo8bitsPerPixel(bool flag)
{
	if (flag)
	{
		m_camera.changeWidthTo8bitsPerPixel();
	}
	else
	{
		m_camera.changeWidthTo16bitsPerPixel();
	}
}

void ImageModel::changeImageToColor(bool flag)
{
	m_imageProcess.setImageColorOrBlack(flag);
}

void ImageModel::changeResolution(int width, int height, int req, long sizePerXfer, int xferQueueSize, int timeOut)
{
	//m_camera.changeResolution(width, height, req);
	//可以对切换无法进行的在界面处进行处理
	if (m_camera.isReceving())
	{
		m_camera.disableReceving();
		emit resolutionChanged(width, height, req, sizePerXfer, xferQueueSize, timeOut);
	}
}
//分配内存空间
void ImageModel::initialImageFifo()
{
	int size = m_imageHeight * m_imageWidth * 2;
	m_imageDataSavingSpace = new uchar*[FIFO_SIZE];
	ImageFifo::setFifoSize(FIFO_SIZE);
	for (int i = 0; i < FIFO_SIZE; ++i)
	{
		m_imageDataSavingSpace[i] = new uchar[size];
	}

}

void ImageModel::sendSettingCommand(uchar u1, uchar u2, uchar u3, uchar u4)
{
	uchar buf[4] = { u1, u2, u3, u4 };

	m_camera.sendRequestCode(0xb3, buf, 4);
}



void ImageModel::setSavingPath(QString path)
{
	m_imageProcess.setSavingPath(path);
}

void ImageModel::takeImage()
{
	m_imageProcess.setTakingImageFlag(true);		
}

void ImageModel::changeResolutionDone()
{
	//while (!m_camera.m_recevingFlag);
	m_camera.waitChangeDone();
}

void ImageModel::getPointPos(int x_orig, int y_orig, int y_fake, int x_move, int y_move)
{
	m_imageProcess.getPoint(x_orig, y_orig);
}

void ImageModel::unableImageprocessGetPoint()
{
	m_imageProcess.unableGetPoint();
}
void ImageModel::setResolutionType(int type)
{
	m_imageProcess.setResolutionType(type);
}

void ImageModel::takeImage(bool darkFlag, int num)
{
	m_imageProcess.takeImage(darkFlag, num);
}

void ImageModel::countPRNU()
{
	m_imageProcess.countPRNU();
}

void ImageModel::countDSNU()
{
	m_imageProcess.countDSNU();
}