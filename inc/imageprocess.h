#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <QtCore>
//#include "qimage.h"
#include "qpixmap.h"
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class ImageProcess : public QObject
{
	Q_OBJECT

public:
	ImageProcess(int height, int width);
	~ImageProcess();
	void setDataSavingSpace(unsigned char *data);
	void setWidth(int width);
	void setHeight(int height);
	//void imageToPixmap(QImage &image);
	void saveOriginalData(uchar *data);
	void enableSaveData();
	void setImageColorOrBlack(bool isColor);
	
	void disableImageProcess();
	void enableImageProcess();

	void setPauseFlag(bool flag);

	void autoWhiteBalance(cv::Mat &src, cv::Mat &dst);

	void dataToImage(unsigned char *data, int bitsPerPixel, int width, int height);

	void setSavingPath(QString path);
	void takeOriginalImage(const cv::Mat& image,  const QPixmap& pixmap);
	void takeShowingImage(const cv::Mat& image, const QPixmap& pixmap);
	void setTakingImageFlag(bool flag);
	void getPoint(int x_orig, int y_orig);
	void unableGetPoint();
	void setResolutionType(int);
	void takeImage(bool darkFlag, int);
	void calculateImage();
	void countPRNU();
	void countDSNU();

signals:
	void showImage(QPixmap image);
	void sendPointData(int pointPosX, int pointPosY, int pointData);
	void takeContImageDone();	
	void sendPRNU(double);
	void sendDSNU(double);
	void sendPRNUWrongSignal(bool alreadyTakeImageFlag);
	void sendDSNUWrongSignal(bool alreadyTakeImageFlag);
public slots:
	void dataToImage();

private:
	unsigned char *m_imageData;
	
	int m_imageHeight;
	int m_imageWidth;

	QPixmap m_imagePixmap;
	QPixmap m_imageShow;
	QImage m_image;
	QString m_path;
	QString str;//记录时间
	QImage qImage;

	bool m_saveOriginalDataFlag;
	bool m_isColor;
	bool m_isTakingImage;
	bool m_imageProcessingFlag;
	bool m_pauseFlag;
	int pointPosX;
	int pointPosY;
	bool getPointFlag;
	int resolutionType_p;
	QPixmap m_imageShowScaled;
	bool darkFlag;
	int imageNum;
	bool takeContImageFlag;//拍摄连续几张图像的标志位

	cv::Mat image[30];
	int i;
	cv::Mat imageDarkAveraged_temp;
	cv::Mat imageDarkAveraged;
	cv::Mat DarkAveraged[2];
	cv::Mat imageLightAveraged;
	cv::Mat imageLightAveraged_temp;
	cv::Mat LightAveraged[2];
	double meanDark;
	double meanLight;
	double s2Dark;
	double s2Light;
	int cols;
	int rows;
	cv::Mat timeVarianceD;
	cv::Mat timeVarianceD_temp;
	cv::Mat timeVarianceD_array[2];
	cv::Mat timeVarianceL;
	cv::Mat timeVarianceL_temp;
	cv::Mat timeVarianceL_array[2];
	double stackTimeVarianceD;
	double stackTimeVarianceL;
	double s2DarkCorrected;
	double s2LightCorrected;
	bool darkImageCountedFlag;
	bool lightImageCountedFlag;
	double PRNU;
	double DSNU;
	cv::Mat imageTake[10];
	int imageTakeCount;
	cv::Mat imageTakeAveraged;
};

#endif