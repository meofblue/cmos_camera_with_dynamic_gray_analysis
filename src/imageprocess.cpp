#include "imageprocess.h"

#include "qdebug.h"
#include <Windows.h> 
#include <QtConcurrent>
#include "imagefifo.h"

//#include "qtextstream.h"

ImageProcess::ImageProcess(int height, int width)
	:m_imageHeight(height),
	m_imageWidth(width),
	m_isColor(false),
	m_imageProcessingFlag(true),
	m_pauseFlag(false),
	m_path("."),
	getPointFlag(false),
	m_isTakingImage(false),
	takeContImageFlag (false),
	darkImageCountedFlag(false),
	lightImageCountedFlag(false)
{
	m_imageData = new uchar[m_imageHeight * m_imageWidth];

}

ImageProcess::~ImageProcess()
{
	delete[] m_imageData;

}

void ImageProcess::setDataSavingSpace(unsigned char *data)
{
	m_imageData = data;
}

void ImageProcess::dataToImage(unsigned char *data, int bitsPerPixel, int width, int height)
{
	switch (bitsPerPixel)
	{
	case 8:
	{
	unsigned short* data16bits = (unsigned short *)data;
		cv::Mat image8bits = cv::Mat(height, width, CV_8UC1, data);
		if (takeContImageFlag)
		{
			if (i == imageNum)
			{
				takeContImageFlag = false;
				calculateImage();
			}
			else
			{
				//image[i] = cv::Mat(height, width, CV_16UC1, cv::Scalar(0));
				image8bits.convertTo(image[i], CV_16U);//从8位数据转为16位数据保存
			 	i++;
			}
		}
		if (getPointFlag)
		{ 		
		int pointData = image8bits.at<uchar>(pointPosY, pointPosX);
		emit sendPointData( pointPosX, pointPosY,  pointData);
		}
		if (m_isColor)
		{			
			cv::Mat image;
			//cv::cvtColor(image8bits, image, CV_BayerRG2RGB);
			cv::cvtColor(image8bits, image, CV_BayerGB2RGB);

			autoWhiteBalance(image, image);
		    qImage = QImage(image.data, width, height, image.step, QImage::Format_RGB888);
			if (qImage.isNull()) return;
			m_imagePixmap = QPixmap::fromImage(qImage);
				m_imageShow = m_imagePixmap;
		}
		else
		{
			qImage = QImage(data, width, height, QImage::Format_Grayscale8);
			if (qImage.isNull()) return;
			m_imagePixmap = QPixmap::fromImage(qImage);
			m_imageShow = m_imagePixmap;
		}
		emit showImage(m_imageShow);
		if (m_isTakingImage)
		{
		//takeShowingImage(cv::Mat(height, width, CV_16UC1, data16bits), m_imagePixmap);
			if (imageTakeCount == 10)
			{ 
				m_isTakingImage = false;
				imageTakeAveraged = cv::Mat(height, width, CV_16U, cv::Scalar(0));
				for (int j = 0; j < 10; j++)
				{
					imageTakeAveraged += imageTake[j];//有问题
				}
				imageTakeAveraged = imageTakeAveraged / 10;
				imageTakeAveraged.convertTo(imageTakeAveraged, CV_8U);
				QImage ImageTake_qImage;
				if (m_isColor)
					ImageTake_qImage = QImage(imageTakeAveraged.data, width, height, imageTakeAveraged.step, QImage::Format_RGB888);
				else
				    ImageTake_qImage = QImage(imageTakeAveraged.data, width, height, QImage::Format_Grayscale8);
				QPixmap ImageTake_pixmap = QPixmap::fromImage(ImageTake_qImage);
				takeShowingImage(imageTakeAveraged, ImageTake_pixmap);
			}
			else
			{
				image8bits.convertTo(imageTake[imageTakeCount], CV_16U);//从8位数据转为16位数据保存
				imageTakeCount++;
			}
		}
		break;

	}
	case 12:
	{
		unsigned short* data16bits = (unsigned short *)data;
		cv::Mat image16bits = cv::Mat(height, width, CV_16UC1, data16bits);
		if (takeContImageFlag)
		{
			if (i == imageNum)
			{
				takeContImageFlag = false;
				calculateImage();
			}
			else
			{
				image16bits.convertTo(image[i], CV_16UC1);
				i++;
			}
		}
		if (getPointFlag)
		{
			int pointData = image16bits.at<ushort>(pointPosY, pointPosX);
			emit sendPointData(pointPosX, pointPosY, pointData);
		}
		if (m_isColor)
		{

			int size = height * width;
			//unsigned short temp;
			//DWORD start = GetTickCount();
			for (int i = 0; i < size; ++i)
			{
				//m_imageData[i] = pow((data[2 * i + 1] << 8) + data[2 * i], 0.66);
				//m_imageData[i] = pow(data16bits[i], 0.66);
				m_imageData[i] = data16bits[i] >> 4;
//				m_imageData[i] = data16bits[i];

			}
			cv::Mat image8bits = cv::Mat(height, width, CV_8UC1, m_imageData);
			cv::Mat image;
			//image16bits.convertTo(image, CV_8UC1);
			//cv::cvtColor(image8bits, image, CV_BayerRG2RGB);
			cv::cvtColor(image8bits, image, CV_BayerGB2RGB);

			autoWhiteBalance(image, image);

		    qImage = QImage(image.data, width, height, image.step, QImage::Format_RGB888);
			//DWORD end = GetTickCount();
			//qDebug() << end - start;
			//cv::Mat image16bits = cv::Mat(m_imageHeight, m_imageWidth, CV_16UC1, data);
			//image16bits.convertTo(image16bits, CV_8UC1, 0.0625);
			//cv::Mat image;
			//cv::cvtColor(image16bits, image, CV_BayerRG2RGB);
			if (qImage.isNull()) return;
			m_imagePixmap = QPixmap::fromImage(qImage);
			m_imageShow = m_imagePixmap;
		}
		else
		{
			int size = height * width;
			//unsignd short temp;
			//DWORD start = GetTickCount();
			for (int i = 0; i < size; ++i)
			{
				//m_imageData[i] = pow((data[2 * i + 1] << 8) + data[2 * i], 0.66);
				//m_imageData[i] = pow(data16bits[i], 0.66);
				m_imageData[i] = data16bits[i]>>4 ;
				//m_imageData[i] = data16bits[i];
			}
			//DWORD end = GetTickCount();
			//qDebug() << end - start;
			qImage = QImage(m_imageData, width, height, QImage::Format_Grayscale8);
			if (qImage.isNull()) return;
			m_imagePixmap = QPixmap::fromImage(qImage);
			m_imageShow = m_imagePixmap;
		}
		emit showImage(m_imageShow);
		if (m_isTakingImage)
		{
			if (imageTakeCount == 10)
			{
				m_isTakingImage = false;
				imageTakeAveraged = cv::Mat(height, width, CV_16U, cv::Scalar(0));//8位的数据需要扩展到12位，12位数据扩展到16位
				for (int j = 0; j < 10; j++)
				{
					imageTakeAveraged += imageTake[j];
				}
				imageTakeAveraged = imageTakeAveraged / 10;
				cv::Mat imageTakeAveraged_8bit;
				imageTakeAveraged_8bit = imageTakeAveraged;
				for (int m = 0; m < height; m++)
				{
					ushort* data = imageTakeAveraged_8bit.ptr<ushort>(m);
					for (int n = 0; n < width; n++)
					{
						data[n] = data[n] >> 4;
					}
				}
				imageTakeAveraged_8bit.convertTo(imageTakeAveraged_8bit, CV_8U);
				QImage ImageTake_qImage;
				if (m_isColor)
					ImageTake_qImage = QImage(imageTakeAveraged_8bit.data, width, height, imageTakeAveraged_8bit.step, QImage::Format_RGB888);
				else
					ImageTake_qImage = QImage(imageTakeAveraged_8bit.data, width, height, QImage::Format_Grayscale8);
				QPixmap ImageTake_pixmap = QPixmap::fromImage(ImageTake_qImage);
				takeOriginalImage(imageTakeAveraged, ImageTake_pixmap);
			}
			else
			{
				imageTake[imageTakeCount] = image16bits;
				imageTakeCount++;
			}	
		}
		break;
	}
	default:
		break;
	
	}
	/*********数据接收较慢时使用*******************/
	/*m_image = QImage(m_imageData, m_imageWidth, m_imageHeight, QImage::Format_Grayscale8);
	QtConcurrent::run(this, &ImageProcess::imageToPixmap, m_image);*/

}

void ImageProcess::setWidth(int width)
{
	m_imageWidth = width;
}

void ImageProcess::setHeight(int height)
{
	m_imageHeight = height;
}
/*void ImageProcess::imageToPixmap(QImage &image)
{
	//DWORD start = GetTickCount();
	QPixmap pixmap = QPixmap::fromImage(image);
	//DWORD end = GetTickCount();
	//qDebug() << end - start << "this is thread " << QThread::currentThreadId();
	emit showImage(pixmap);
}
*/

void ImageProcess::saveOriginalData(unsigned char *data)
{
	int size = m_imageHeight * m_imageWidth;
	unsigned short *tempData = new unsigned short[size];
	for (int i = 0; i < size; ++i)
	{
		tempData[i] = data[i];
	}
	QString path = m_path + "\\" + str + ".txt";
	QFile file(path);

	if (!file.open(QIODevice::WriteOnly))
		return;
	
	QTextStream out(&file);
	for (int i = 0; i < m_imageHeight; ++i)
	{
		int k = i * m_imageWidth;
		for (int j = 0; j < m_imageWidth; ++j)
		{
			out << tempData[k + j] << " ";//保存每一个像素点
		}
		out << "\r\n";
	}
	delete tempData;
	tempData = nullptr;

}


void ImageProcess::enableSaveData()
{
	m_saveOriginalDataFlag = true;
}

void ImageProcess::setImageColorOrBlack(bool isColor)
{
	m_isColor = isColor;
}

void ImageProcess::dataToImage()
{
	ImageData* imageData;
	while (m_imageProcessingFlag)
	{
		imageData = ImageFifo::getFifoHead();
		if (imageData == nullptr) continue;
		if (!m_pauseFlag)
		{
			dataToImage((*imageData).m_data, (*imageData).m_bitsPerPixel, (*imageData).m_imageWidth, (*imageData).m_imageHeight);
		}
		ImageFifo::outFifo();
	}

	ImageFifo::emptyFifo();
	
}

void ImageProcess::disableImageProcess()
{
	m_imageProcessingFlag = false;
}

void ImageProcess::enableImageProcess()
{
	m_imageProcessingFlag = true;
}

void ImageProcess::setPauseFlag(bool flag)
{
	m_pauseFlag = flag;
}

void ImageProcess::setSavingPath(QString path)
{
	m_path = path;
}

void ImageProcess::takeOriginalImage(const cv::Mat& image, const QPixmap& pixmap)
{
	QTime time = QTime::currentTime();
	str = time.toString("hhmmsszzz");
	QString path = m_path + "\\" + str + ".png";
	cv::imwrite(path.toLocal8Bit().toStdString(), image);//保存12位图像
	pixmap.save(m_path + "\\" + str + ".bmp");//保存8位图像
//	m_isTakingImage = false;
}

void ImageProcess::takeShowingImage(const cv::Mat& image, const QPixmap& pixmap)
{
	QTime time = QTime::currentTime();
	str = time.toString("hhmmsszzz");
//	QString path = m_path + "\\" + str + ".png";
//	cv::imwrite(path.toLocal8Bit().toStdString(), image);
	pixmap.save(m_path + "\\" + str + ".bmp");	
	//m_isTakingImage = false;
}

void ImageProcess::setTakingImageFlag(bool flag)
{
	if (m_isTakingImage)
		return;
	m_isTakingImage = flag;
	imageTakeCount = 0;
}

void ImageProcess::getPoint(int x_orig, int y_orig)
{
	pointPosX = x_orig;
	pointPosY = y_orig;
	getPointFlag = true;
}
void ImageProcess::unableGetPoint()
{
	getPointFlag = false;
}

void ImageProcess::setResolutionType(int type)
{
	resolutionType_p = type;
}

void ImageProcess::autoWhiteBalance(cv::Mat &src, cv::Mat &dst)
{
	/*********原始的灰度世界算法**************/
	cv::Scalar averagePerChannel = cv::mean(src);
	double ratio = (averagePerChannel[0] + averagePerChannel[1] + averagePerChannel[2]) / 3;
	averagePerChannel[0] = ratio / averagePerChannel[0];
	averagePerChannel[1] = ratio / averagePerChannel[1];
	averagePerChannel[2] = ratio / averagePerChannel[2];
	cv::multiply(src, averagePerChannel, dst);
}

void ImageProcess::takeImage(bool flag, int num)
{
	takeContImageFlag = true;
	darkFlag = flag;
	imageNum = num;
	i = 0;
}

//求一个平均图像,和平均灰度值
//会不会阻塞datatoimage？需要另开线程吗？
void ImageProcess::calculateImage()
{
	rows = image[0].rows;
	cols = image[0].cols;
	if (darkFlag)
	{
		//平均图像
		//imageDarkAveraged = image[0];
		imageDarkAveraged = cv::Mat(rows, cols, CV_16U, cv::Scalar(0));//16幅图，8位的数据需要扩展到12位，12位数据扩展到16位
		for (int j = 0; j < imageNum; j++)
		{
			imageDarkAveraged += image[j];
		}
		imageDarkAveraged = imageDarkAveraged / imageNum;//直接16幅图相加，值会超过255，溢出
		//平均灰度值		
		meanDark = 0;
		for (int m = 0; m < rows; m++)
		{
			ushort* data = imageDarkAveraged.ptr<ushort>(m);
			for (int n = 0; n < cols; n++)
			{
				meanDark += data[n];
			}
		}		
		meanDark = meanDark / rows / cols;
		//方差
		s2Dark = 0;			
		for (int m = 0; m < rows; m++)
		{
			ushort* data = imageDarkAveraged.ptr<ushort>(m);
			for (int n = 0; n < cols; n++)
			{
				s2Dark += pow(data[n] - meanDark, 2);
			}
		}	
		s2Dark = s2Dark / (rows*cols - 1);
		//时间残差
		timeVarianceD = cv::Mat(rows, cols, CV_16U, cv::Scalar(0));			
		for (int m = 0; m < rows; m++)
		{
			for (int n = 0; n < cols; n++)
			{
			//	timeVarianceD.ptr<ushort>(m)[n] = 0;
				for (int l = 0; l < imageNum; l++)
				{
					timeVarianceD.ptr<ushort>(m)[n] += pow(image[l].ptr<ushort>(m)[n] - imageDarkAveraged.ptr<ushort>(m)[n], 2);
				}
				timeVarianceD.ptr<ushort>(m)[n] = timeVarianceD.ptr<ushort>(m)[n] / (imageNum - 1);
			}
		}
		stackTimeVarianceD = 0;
		for (int m = 0; m < rows; m++)
		{
			for (int n = 0; n < cols; n++)
			{
				stackTimeVarianceD += timeVarianceD.ptr<ushort>(m)[n];
			}
		}		
		stackTimeVarianceD = stackTimeVarianceD / rows / cols;
		//校正后的方差
		s2DarkCorrected = s2Dark - stackTimeVarianceD / imageNum;
		darkImageCountedFlag = true;
	}
	else//亮图像计算
	{
		//平均图像
		imageLightAveraged = cv::Mat(rows, cols, CV_16U, cv::Scalar(0));
		for (int j = 0; j < imageNum; j++)
		{
			imageLightAveraged += image[j];
		}
		imageLightAveraged = imageLightAveraged / imageNum;
		//平均值
		meanLight = 0;	
		for (int m = 0; m < rows; m++)
		{
			ushort* data = imageLightAveraged.ptr<ushort>(m);
			for (int n = 0; n < cols; n++)
			{
				meanLight += data[n];
			}
		}		
		meanLight = meanLight / rows / cols;
		//测量方差
		s2Light = 0;	
		for (int m = 0; m < rows; m++)
		{
			ushort* data = imageLightAveraged.ptr<ushort>(m);
			for (int n = 0; n < cols; n++)
			{
				s2Light += pow(data[n] - meanLight, 2);
			}
		}
		s2Light = s2Light / (rows*cols - 1);
		//时间残差
		timeVarianceL = cv::Mat(rows, cols, CV_16U, cv::Scalar(0));
		for (int m = 0; m < rows; m++)
		{
			for (int n = 0; n < cols; n++)
			{
				//	timeVarianceL.ptr<ushort>(m)[n] = 0;
				for (int l = 0; l < imageNum; l++)
				{
					timeVarianceL.ptr<ushort>(m)[n] += pow(image[l].ptr<ushort>(m)[n] - imageLightAveraged.ptr<ushort>(m)[n], 2);
				}
				timeVarianceL.ptr<ushort>(m)[n] = timeVarianceL.ptr<ushort>(m)[n] / (imageNum - 1);
			}
		}
		stackTimeVarianceL = 0;
		for (int m = 0; m < rows; m++)
		{
			for (int n = 0; n < cols; n++)
			{
				stackTimeVarianceL += timeVarianceL.ptr<ushort>(m)[n];
			}
		}		
		stackTimeVarianceL = stackTimeVarianceL / rows / cols;
		//校正后的方差
		s2LightCorrected = s2Light - stackTimeVarianceL / imageNum;
		lightImageCountedFlag = true;
	}
	emit takeContImageDone();
}	

void ImageProcess::countPRNU()
{
	if (!(darkImageCountedFlag&&lightImageCountedFlag))//如果没有拍摄过暗图像和亮图像，则返回
	{
		emit sendPRNUWrongSignal(0);
		return;
	}
	PRNU = (sqrt(s2LightCorrected - s2DarkCorrected)) / (meanLight - meanDark) * 100;//因为单位是%所以需要乘以100
	if (PRNU < 0)
	{
		emit sendPRNUWrongSignal(1);
		return;
	}
	emit sendPRNU(PRNU);
}

void ImageProcess::countDSNU()
{
	if (!darkImageCountedFlag)
	{
		emit sendDSNUWrongSignal(0);
		return;
	}
	DSNU = s2DarkCorrected ;
	if (DSNU < 0)
	{
		emit sendDSNUWrongSignal(1);
		return;
	}
	emit sendDSNU(DSNU);	
}
