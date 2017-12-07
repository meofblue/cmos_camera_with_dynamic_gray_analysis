#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "imagemodel.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	bool initCameraConfig();
	void setGain();
	int getGainData(int gain);

public slots:
	void openCamera();
	void closeCamera();
	void pauseCamera();

	void updateImage(QPixmap image);

	void showFrameRate();

	void saveData();

	void countReceiveFrames();

	void changeWidthTo8bitsPerPixel(bool flag);
	
	void changeImageToColor(bool flag);

	//void switchResolution(int index);
	void switchResolution_1280(bool);
	void switchResolution_640(bool);
	void switchResolution_320(bool);
	void switchResolution_1920(bool);
	void switchResolution_2048(bool);
	void switchResolution_2592(bool);
	void setAnalogGain_1(bool);
	void setAnalogGain_2(bool);
	void setAnalogGain_4(bool);
	void setAnalogGain_8(bool);
	void setAnalogGain_16(bool);

	//void setAnalogGain(int index);
	
	//void setRedGain(int gain);
	//void setGreenGain(int gain);
	//void setBlueGain(int gain);
	//void setGlobalGain(int gain);
	void setRedGain_1(bool);
	void setRedGain_2(bool);
	void setRedGain_4(bool);
	void setRedGain_8(bool);
	void setRedGain_16(bool);
	void setGreenGain_1(bool);
	void setGreenGain_2(bool);
	void setGreenGain_4(bool);
	void setGreenGain_8(bool);
	void setGreenGain_16(bool);
	void setBlueGain_1(bool);
	void setBlueGain_2(bool);
	void setBlueGain_4(bool);
	void setBlueGain_8(bool);
	void setBlueGain_16(bool);
	void setGlobalGain_1(bool);
	void setGlobalGain_2(bool);
	void setGlobalGain_4(bool);
	void setGlobalGain_8(bool);
	void setGlobalGain_16(bool);

	void setExposureMode(bool isAuto);
	void setExposureValue(int value);
	void chooseSavingPath();
	void takeImage();
	void showCurve();
	void openImage();
	void setHorizontalLine();
	void setVerticalLine();
	void setInputDataLine();
	//void configMyGraphPlot();
	//void showAnalysePoint();
	void getPointData(int, int, int);
	void clearPointData();
	void startCountTime(int, int, int, int, int);
	void setPointChoosed_o();
	void setPointChoosed_a();
	void setPointChoosed_b();
	void setPointChoosed_c();
	void setPointChoosed_d();
	void setPointChoosed_e();
	void getPointInput();
	void countPRNU();
	void countDSNU();
	void takeDarkImage();
	void takeLightImage();
	void setNonuniformityEnable();
	void showPRNU(double);
	void wrongTipsOfPRNU(bool);
	void showDSNU(double);
	void wrongTipsOfDSNU(bool);

private:
	Ui::MainWindowClass ui;

	ImageModel m_imageModel;
	
	QTimer m_timer;

	QLabel *m_frameRateLabel;

	QString path;

	int m_frameCount;

	int m_receiveFramesCount;

	int m_imageWidth;
	int m_imageHeight;

	bool m_isClosed;
	int showImageWidth ;
	int showImageHeight;
	int openImageWidth;
	int openImageHeight;
//	bool openImageFlag;
	int resolutionType;
	QPixmap image_show;
	bool bits_8_flag; 
	int analogGain;
	int redGain;
	int greenGain;
	int blueGain;
	bool getPoint;
	int updataGraphFlag;
	bool openCameraFlag;
	QTime startTime;
	int gray_avg;
	int y_orig_fake_m;
	int gray_avg_show;
	int updateDataFlag;
//	QMessageBox messageBox;
//	bool takingImageFlag;
};


#endif // MAINWINDOW_H
