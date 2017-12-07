#include "mainwindow.h"
#include "qsettings.h"
#include "qdebug.h"


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	m_imageModel(this, 2592, 1952),
	m_frameCount(0),
	m_receiveFramesCount(0),
	updataGraphFlag (0),
	bits_8_flag(true),
	getPoint(false),
	m_isClosed(false)
	//openImageFlag(false)
{
	if (!initCameraConfig())
	{
		m_imageHeight = 972;
		m_imageWidth = 1296;
	}
	m_imageModel.setResolution(m_imageHeight, m_imageWidth);
	resolutionType = 3;
	analogGain=0x0008;
	redGain=0x0000;
	greenGain = 0x0000;
	blueGain = 0x0000;
	
 	ui.setupUi(this);
	//ui.m_showFrame->setFixedSize(QSize(1280, 960));
	ui.m_showFrame->setFixedSize(QSize(m_imageWidth, m_imageHeight));
	ui.m_showLabel->setFixedSize(QSize(m_imageWidth, m_imageHeight));
		
	m_frameRateLabel = new QLabel(ui.statusBar);
	ui.statusBar->addWidget(m_frameRateLabel);
	m_frameRateLabel->setText(QStringLiteral("就绪"));

	ui.m_point_pos->setText(QStringLiteral("0,0"));
	ui.m_point_data->setText(QStringLiteral("0"));

	//Qt4的信号槽使用方式
	connect(ui.m_startButton, SIGNAL(clicked()), this, SLOT(openCamera()));
	connect(ui.m_stopButton, SIGNAL(clicked()), this, SLOT(closeCamera()));
	connect(ui.m_pauseButton, SIGNAL(clicked()), this, SLOT(pauseCamera()));

	connect(ui.m_8bitsChoosed, SIGNAL(toggled(bool)), this, SLOT(changeWidthTo8bitsPerPixel(bool)));
	connect(ui.m_colorImageChoosed, SIGNAL(toggled(bool)), this, SLOT(changeImageToColor(bool)));
	

	//Qt5的重载信号与槽连接的使用方式
	//connect(ui.m_resolutionSwitching, static_cast<void (QComboBox:: *)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::switchResolution);
	connect(ui.m_1280, SIGNAL(toggled(bool)), this, SLOT(switchResolution_1280(bool)));
	connect(ui.m_640, SIGNAL(toggled(bool)), this, SLOT(switchResolution_640(bool)));
	//connect(ui.m_320, SIGNAL(toggled(bool)), this, SLOT(switchResolution_320(bool)));
	connect(ui.m_1920, SIGNAL(toggled(bool)), this, SLOT(switchResolution_1920(bool)));
	connect(ui.m_2048, SIGNAL(toggled(bool)), this, SLOT(switchResolution_2048(bool)));
	connect(ui.m_2592, SIGNAL(toggled(bool)), this, SLOT(switchResolution_2592(bool)));
	//connect(ui.m_analogGainSet, static_cast<void (QComboBox:: *)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::setAnalogGain);
	connect(ui.m_1, SIGNAL(toggled(bool)), this, SLOT(setAnalogGain_1(bool)));
	connect(ui.m_2, SIGNAL(toggled(bool)), this, SLOT(setAnalogGain_2(bool)));
	connect(ui.m_4, SIGNAL(toggled(bool)), this, SLOT(setAnalogGain_4(bool)));
	connect(ui.m_8, SIGNAL(toggled(bool)), this, SLOT(setAnalogGain_8(bool)));
	connect(ui.m_16, SIGNAL(toggled(bool)), this, SLOT(setAnalogGain_16(bool)));
	
//	connect(ui.m_rGainSet, &QSlider::valueChanged, this, &MainWindow::setRedGain);
//	connect(ui.m_gGainSet, &QSlider::valueChanged, this, &MainWindow::setGreenGain);
//	connect(ui.m_bGainSet, &QSlider::valueChanged, this, &MainWindow::setBlueGain);
//	connect(ui.m_globalGainSet, &QSlider::valueChanged, this, &MainWindow::setGlobalGain);
	connect(ui.m_red_1, SIGNAL(toggled(bool)), this, SLOT(setRedGain_1(bool)));
	connect(ui.m_red_2, SIGNAL(toggled(bool)), this, SLOT(setRedGain_2(bool)));
	connect(ui.m_red_4, SIGNAL(toggled(bool)), this, SLOT(setRedGain_4(bool)));
	connect(ui.m_red_8, SIGNAL(toggled(bool)), this, SLOT(setRedGain_8(bool)));
	connect(ui.m_red_16, SIGNAL(toggled(bool)), this, SLOT(setRedGain_16(bool)));
	connect(ui.m_green_1, SIGNAL(toggled(bool)), this, SLOT(setGreenGain_1(bool)));
	connect(ui.m_green_2, SIGNAL(toggled(bool)), this, SLOT(setGreenGain_2(bool)));
	connect(ui.m_green_4, SIGNAL(toggled(bool)), this, SLOT(setGreenGain_4(bool)));
	connect(ui.m_green_8, SIGNAL(toggled(bool)), this, SLOT(setGreenGain_8(bool)));
	connect(ui.m_green_16, SIGNAL(toggled(bool)), this, SLOT(setGreenGain_16(bool)));
	connect(ui.m_blue_1, SIGNAL(toggled(bool)), this, SLOT(setBlueGain_1(bool)));
	connect(ui.m_blue_2, SIGNAL(toggled(bool)), this, SLOT(setBlueGain_2(bool)));
	connect(ui.m_blue_4, SIGNAL(toggled(bool)), this, SLOT(setBlueGain_4(bool)));
	connect(ui.m_blue_8, SIGNAL(toggled(bool)), this, SLOT(setBlueGain_8(bool)));
	connect(ui.m_blue_16, SIGNAL(toggled(bool)), this, SLOT(setBlueGain_16(bool)));
	connect(ui.m_global_1, SIGNAL(toggled(bool)), this, SLOT(setGlobalGain_1(bool)));
	connect(ui.m_global_2, SIGNAL(toggled(bool)), this, SLOT(setGlobalGain_2(bool)));
	connect(ui.m_global_4, SIGNAL(toggled(bool)), this, SLOT(setGlobalGain_4(bool)));
	connect(ui.m_global_8, SIGNAL(toggled(bool)), this, SLOT(setGlobalGain_8(bool)));
	connect(ui.m_global_16, SIGNAL(toggled(bool)), this, SLOT(setGlobalGain_16(bool)));

	connect(ui.m_autoExposure, &QRadioButton::toggled, this, &MainWindow::setExposureMode);
	connect(ui.m_exposureSlider, &QSlider::valueChanged, this, &MainWindow::setExposureValue);
	connect(ui.m_exposureSlider, &QSlider::valueChanged, ui.m_exposureSpinBox, &QSpinBox::setValue);
	connect(ui.m_exposureSpinBox, static_cast<void (QSpinBox:: *)(int)>(&QSpinBox::valueChanged), ui.m_exposureSlider, &QSlider::setValue);
	
	connect(ui.m_pathChoosingButton, &QPushButton::clicked, this, &MainWindow::chooseSavingPath);
	connect(ui.m_imageTakingButton, &QPushButton::clicked, this, &MainWindow::takeImage);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(showFrameRate()));

	connect(ui.m_openImage, &QPushButton::clicked, this, &MainWindow::openImage);
	connect(ui.m_horizontalLine, &QPushButton::clicked, this, &MainWindow::setHorizontalLine);
	connect(ui.m_verticalLine, &QPushButton::clicked, this, &MainWindow::setVerticalLine);
	connect(ui.m_inputDataLine, &QPushButton::clicked, this, &MainWindow::setInputDataLine);

	connect(ui.m_showLabel, SIGNAL(drawLine(int, int, int, int, int)), ui.m_customPlot, SLOT(updateLine(int, int, int, int, int)));
	connect(ui.m_showLabel, SIGNAL(sendPointPos(int, int)), ui.m_customPlot, SLOT(getPointPos(int, int )));
	connect(ui.m_customPlot, SIGNAL(sendPointData(int, int, int)), this, SLOT(getPointData(int, int, int)));
	connect(ui.m_showLabel, SIGNAL(sendPointPos(int, int, int, int, int)), &m_imageModel, SLOT(getPointPos(int, int, int, int, int)));
	connect(ui.m_showLabel, SIGNAL(sendPointPos(int, int, int, int, int)), this, SLOT(startCountTime(int, int, int, int, int)));

	connect(ui.point_o, SIGNAL(clicked()), this, SLOT(setPointChoosed_o()));
	connect(ui.point_a, SIGNAL(clicked()), this, SLOT(setPointChoosed_a()));
	connect(ui.point_b, SIGNAL(clicked()), this, SLOT(setPointChoosed_b()));
	connect(ui.point_c, SIGNAL(clicked()), this, SLOT(setPointChoosed_c()));
	connect(ui.point_d, SIGNAL(clicked()), this, SLOT(setPointChoosed_d()));
	connect(ui.point_e, SIGNAL(clicked()), this, SLOT(setPointChoosed_e()));
	connect(ui.pointComfirm, SIGNAL(clicked()), this, SLOT(getPointInput()));

	connect(ui.m_takedarkimage, &QPushButton::clicked, this, &MainWindow::takeDarkImage);
	connect(ui.m_takelightimage, &QPushButton::clicked, this, &MainWindow::takeLightImage);
	connect(ui.m_prnu, &QPushButton::clicked, this, &MainWindow::countPRNU);
	connect(ui.m_dsnu, &QPushButton::clicked, this, &MainWindow::countDSNU);
}

MainWindow::~MainWindow()
{
	//m_imageModel.closeUSBCamera();
}

void MainWindow::openCamera()
{
	//qDebug() << "i have in ";
	if (m_imageModel.openUSBCamera())
	{
		ui.m_startButton->setEnabled(false);
		ui.m_pauseButton->setEnabled(true);
		ui.m_stopButton->setEnabled(true);
		
		//位宽及色彩
		if (ui.m_12bitsChoosed->isChecked())
		{
			changeWidthTo8bitsPerPixel(false);
		}
		else
		{
			changeWidthTo8bitsPerPixel(true);
		}
		if (ui.m_colorImageChoosed->isChecked())
		{
			changeImageToColor(true);
		}
		else
		{
			changeImageToColor(false);
		}

		m_isClosed = false;
		m_imageModel.readData();
		m_frameCount = 0;
		m_timer.start(1000);
		openCameraFlag = true;
		ui.m_showLabel->setOpenCameraFlag(true);
		//曝光模式
		if (ui.m_manualExposure->isChecked())
		{
			setExposureMode(false);
		//	setExposureValue(ui.m_exposureSlider->value());在分辨率函数中有这个操作
			//分辨率
			if (ui.m_2592->isChecked())
			{
				switchResolution_2592(true);
			}
			else if (ui.m_2048->isChecked())
			{
				switchResolution_2048(true);
			}
			else if (ui.m_1920->isChecked())
			{
				switchResolution_1920(true);
			}
			else if (ui.m_1280->isChecked())
			{
				switchResolution_1280(true);
			}
			else if (ui.m_640->isChecked())
			{
				switchResolution_640(true);
			}
		}
		else
		{
			setExposureMode(true);//这个函数里面已经做了分辨率切换
		}
		
		ui.m_bitsPerPixelChange->setEnabled(true);
//		ui.m_resolutionSwitching->setEnabled(true);
		ui.m_solution->setEnabled(true);
		ui.m_imageTakingButton->setEnabled(true);
//		ui.m_analogGainSet->setEnabled(true);
		ui.m_analogGain->setEnabled(true);
		ui.m_digitalGainSet->setEnabled(true);
		ui.m_exposureMode->setEnabled(true);
		ui.m_spacial_nonuniformity->setEnabled(true);
//		ui.m_colorImageChange->setEnabled(true);
		ui.m_horizontalLine->setEnabled(false);
		ui.m_verticalLine->setEnabled(false);
		ui.m_setLineChange->setEnabled(false);	
		//ui.m_point->setEnabled(false);
		ui.m_showLabel->unableSetLine();
		ui.m_customPlot->clearData();
		ui.m_customPlot->setupRealtimeDataDemo();
	//	openImageFlag = false;
		
	}
	else
	{
		QMessageBox::about(this, QStringLiteral("提示"), QStringLiteral("设备打开失败，请重新连接"));
	}
}

void MainWindow::closeCamera()
{

	ui.m_stopButton->setEnabled(false);
	ui.m_pauseButton->setEnabled(false);
	ui.m_bitsPerPixelChange->setEnabled(false);
	ui.m_solution->setEnabled(false);
//	ui.m_resolutionSwitching->setEnabled(false);
	ui.m_imageTakingButton->setEnabled(false);
//	ui.m_analogGainSet->setEnabled(false);
	ui.m_analogGain->setEnabled(false);
	ui.m_digitalGainSet->setEnabled(false);
	ui.m_exposureMode->setEnabled(false);
	ui.m_spacial_nonuniformity->setEnabled(false);
//	ui.m_colorImageChange->setEnabled(false);
	openCameraFlag = false;
	m_imageModel.closeUSBCamera();
	m_timer.stop();
	clearPointData();

	ui.m_showLabel->update();
	ui.m_showLabel->setOpenCameraFlag(false);
	if (ui.m_pauseButton->text() != "暂停")
	{
		ui.m_pauseButton->setText(QStringLiteral("暂停"));
		m_imageModel.whetherPausingUSBCamera(false);
	}

	m_frameRateLabel->setText(QStringLiteral("就绪"));
	ui.m_startButton->setEnabled(true);
	m_frameCount = 0;
	m_receiveFramesCount = 0;
	m_isClosed = true;
	ui.m_showLabel->clear();
	ui.m_receiveRateLabel->setText(QString::number(0)); 
}

void MainWindow::updateImage(QPixmap image)
{
	if (m_isClosed)
		return;
	//qDebug() << "updateImage";
	//ui.m_showLabel->resize(image.size());
	switch (resolutionType)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		//image = image.scaled(640, 480, Qt::KeepAspectRatio, Qt::FastTransformation);
		break;
	case 4:
		image = image.scaled(1280, 720, Qt::KeepAspectRatio, Qt::SmoothTransformation);//需要返回一个image
		break;
	case 5:
		image = image.scaled(1280, 960, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		break;
	case 6:
		image = image.scaled(1296, 982, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//实际高1952
		break;
	}
//	image_show = QPixmap::fromImage(image);
	ui.m_showLabel->setPixmap(image);
	if (getPoint)//如果点击过一点，那么就重画该点
		ui.m_showLabel->update();
	//DWORD start1 = GetTickCount();
	++m_frameCount;

	/*DWORD end1 = GetTickCount();
	qDebug() << end1 - start1;*/
}

void MainWindow::showFrameRate()
{
	//QString receiveFrameRate = QString::number(m_receiveFramesCount);
	//m_frameRateLabel->setText(receiveFrameRate + "fps");
	ui.m_receiveRateLabel->setText(QString::number(m_frameCount));
	m_frameCount = 0;
	//m_receiveFramesCount = 0;
}


void MainWindow::saveData()
{
	m_imageModel.saveData();
}

void MainWindow::countReceiveFrames()
{
	++m_receiveFramesCount;
}

void MainWindow::pauseCamera()
{
	if (ui.m_pauseButton->text() == QStringLiteral("暂停"))
	{
		ui.m_imageTakingButton->setEnabled(false);
		m_imageModel.whetherPausingUSBCamera(true);
		ui.m_pauseButton->setText(QStringLiteral("取消暂停"));
	}
	else
	{
		m_imageModel.whetherPausingUSBCamera(false);
		ui.m_pauseButton->setText(QStringLiteral("暂停"));
		ui.m_imageTakingButton->setEnabled(true);
	}
}

void MainWindow::changeWidthTo8bitsPerPixel(bool flag)
{
	m_imageModel.changeWidthTo8bitsPerPixel(flag);
	bits_8_flag = flag;
	ui.m_customPlot->get8BitsFlag(flag);
}

void MainWindow::changeImageToColor(bool flag)
{
	m_imageModel.changeImageToColor(flag);
}

/*void MainWindow::switchResolution(int index)
{
	//ui.m_resolutionSwitching->setEnabled(false);
	switch (index)
	{
	case 0:// 320 * 240
		m_imageModel.changeResolution(320, 240, 0xb1, 256 * 150, 2, 50);
		ui.m_showLabel->setFixedSize(320, 240);
		break;
	case 1:// 640 * 480
		m_imageModel.changeResolution(640, 480, 0xa2, 320 * 240, 4, 50);
		ui.m_showLabel->setFixedSize(640, 480);
		break;
	case 2:// 1280 * 960
		m_imageModel.changeResolution(1280, 960, 0xa1, 120 * 1024, 10, 50);
		ui.m_showLabel->setFixedSize(1280, 960);
		break;
	default:
		break;
	}
	//ui.m_resolutionSwitching->setEnabled(true);

}*/

void MainWindow::switchResolution_2592(bool flag)
{
	if (flag == true)
	{
		m_imageModel.unableImageprocessGetPoint();
	//	m_imageModel.changeResolution(2560, 1944, 0xc6, 414720, 12, 1000);
		m_imageModel.changeResolution(2592, 1952, 0xc6, 562176, 9, 2000);
		ui.m_showLabel->setFixedSize(1296, 972);//实际上只显示1932高的图像
		ui.m_showFrame->setFixedSize(1297, 973);
		resolutionType = 6;
		m_imageModel.changeResolutionDone();

		if (ui.m_manualExposure->isChecked())
			setExposureValue(ui.m_exposureSpinBox->value());

		setGain();
		ui.m_showLabel->setResolutionType(6);	
		ui.m_showLabel->setPointChoosed(647, 485, 1295, 0);//中心点在label上的显示位置和实际点坐标
	}
}

void MainWindow::switchResolution_2048(bool flag)
{
	if (flag == true)
	{
		m_imageModel.unableImageprocessGetPoint();
		m_imageModel.changeResolution(2048, 1536, 0xc5, 196608, 12, 1000);//传接32次完成一帧 256*384  262144
		ui.m_showLabel->setFixedSize(1280, 960);
		ui.m_showFrame->setFixedSize(1281, 961);
		resolutionType = 5;
		m_imageModel.changeResolutionDone();
		if (ui.m_manualExposure->isChecked())
			setExposureValue(ui.m_exposureSpinBox->value());
		setGain();
		m_imageModel.setResolutionType(5);
		ui.m_showLabel->setResolutionType(5);
		//clearPointData();
		ui.m_showLabel->setPointChoosed(640, 480, 1024, 768);
	}
}


void MainWindow::switchResolution_1920(bool flag)
{
	if (flag == true)
	{
		m_imageModel.unableImageprocessGetPoint();
		m_imageModel.changeResolution(1920, 1080, 0xc4, 230400, 9, 1000);//传接45次完成一帧  120*384
		ui.m_showLabel->setFixedSize(1280, 720);
		ui.m_showFrame->setFixedSize(1281, 961);
		resolutionType = 4;
		m_imageModel.changeResolutionDone();
		if (ui.m_manualExposure->isChecked())
			setExposureValue(ui.m_exposureSpinBox->value());
		setGain();
		m_imageModel.setResolutionType(4);
		ui.m_showLabel->setResolutionType(4);
	//	clearPointData();
		ui.m_showLabel->setPointChoosed(640, 360, 960, 540);
	}
}

void MainWindow::switchResolution_1280(bool flag)
{
	if (flag == true)
	{
		m_imageModel.unableImageprocessGetPoint();
		m_imageModel.changeResolution(1280, 960, 0xc3, 120 * 1024, 10, 1000);
		ui.m_showLabel->setFixedSize(1280, 960);//frame是1292*972的
		ui.m_showFrame->setFixedSize(1281, 961);
		resolutionType = 3;
		m_imageModel.changeResolutionDone();
		if (ui.m_manualExposure->isChecked())
			setExposureValue(ui.m_exposureSpinBox->value());
		setGain();
		ui.m_showLabel->setResolutionType(3);
		//clearPointData();
		ui.m_showLabel->setPointChoosed(640, 480, 640, 480);
	}
}

void MainWindow::switchResolution_640(bool flag)
{
	if (flag == true)
	{
		m_imageModel.unableImageprocessGetPoint();
		m_imageModel.changeResolution(640, 480, 0xc2, 320 * 240, 4, 1000);
		ui.m_showLabel->setFixedSize(640, 480);
		ui.m_showFrame->setFixedSize(1281, 961);
		resolutionType = 2;
		m_imageModel.changeResolutionDone();
		if (ui.m_manualExposure->isChecked())
			setExposureValue(ui.m_exposureSpinBox->value());
		setGain();
		ui.m_showLabel->setResolutionType(2);
	//	clearPointData();
		ui.m_showLabel->setPointChoosed(320, 240, 320, 240);
	}
}

void MainWindow::switchResolution_320(bool flag)
{
	if (flag == true)
	{
		m_imageModel.changeResolution(320, 240, 0xc1, 256 * 150, 2, 1000);
		ui.m_showLabel->setFixedSize(320, 240);
		resolutionType = 1;
		m_imageModel.changeResolutionDone();
	}
}

bool MainWindow::initCameraConfig()
{
	QSettings settings("camconfig.ini", QSettings::IniFormat);

	m_imageWidth = settings.value("Camera006/width").toInt();
	m_imageHeight = settings.value("Camera006/height").toInt();

	if (m_imageHeight == 0 || m_imageWidth == 0)
		return false;
	else
		return true;
}

/*void MainWindow::setAnalogGain(int index)
{
	switch (index)
	{
	case 0://1倍增益
		m_imageModel.sendSettingCommand(0x3E, 0xE4, 0xD2, 0x08);
		m_imageModel.sendSettingCommand(0x30, 0xB0, 0x00, 0x00);
		break;
	case 1://2倍增益
		m_imageModel.sendSettingCommand(0x3E, 0xE4, 0xD2, 0x08);
		m_imageModel.sendSettingCommand(0x30, 0xB0, 0x00, 0x10);
		break;
	case 2://4倍增益
		m_imageModel.sendSettingCommand(0x3E, 0xE4, 0xD2, 0x08);
		m_imageModel.sendSettingCommand(0x30, 0xB0, 0x00, 0x20);
		break;
	case 3://8倍增益
		m_imageModel.sendSettingCommand(0x3E, 0xE4, 0xD2, 0x08);
		m_imageModel.sendSettingCommand(0x30, 0xB0, 0x00, 0x30);
		break;
	case 4://10倍增益
		m_imageModel.sendSettingCommand(0x3E, 0xE4, 0xD3, 0x08);
		m_imageModel.sendSettingCommand(0x30, 0xB0, 0x00, 0x30);
		break;
	default:
		break;
	}
}*/

void MainWindow::setGain()
{
	//m_imageModel.changeResolutionDone();
	//qDebug() << "analoGain: " << analogGain;
	m_imageModel.sendSettingCommand(0x00, 0x2B, greenGain, analogGain);//前两个寄存器地址，后两个数据
	m_imageModel.sendSettingCommand(0x00, 0x2C, blueGain, analogGain);
	m_imageModel.sendSettingCommand(0x00, 0x2D, redGain, analogGain);
	m_imageModel.sendSettingCommand(0x00, 0x2E, greenGain, analogGain);
}

void  MainWindow::setAnalogGain_1(bool flag)
{
	if (flag == true)
	{
		analogGain = 0x08;
		setGain();
	}
}

void  MainWindow::setAnalogGain_2(bool flag)
{
	if (flag == true)
	{
		analogGain = 0x10;
		setGain();
	}
}

void  MainWindow::setAnalogGain_4(bool flag)
{
	if (flag == true)
	{
		analogGain = 0x20;
		setGain();
		m_imageModel.sendSettingCommand(0x00, 0x3E, 0x00, 0x87);
	}
}
void  MainWindow::setAnalogGain_8(bool flag)
{
	if (flag == true)
	{
		analogGain = 0x60;
		setGain();
		m_imageModel.sendSettingCommand(0x00, 0x3E, 0x00, 0x87);
	}
}
void  MainWindow::setAnalogGain_16(bool flag)
{
	if (flag == true)
	{
		analogGain = 0x7F;
		setGain();
		m_imageModel.sendSettingCommand(0x00, 0x3E, 0x00, 0x87);
	}
}
/*
void MainWindow::setRedGain(int gain)
{
	ui.m_rGain->setText(QString::number(gain));	
	redGain = getGainData(gain);
	m_imageModel.sendSettingCommand(0x00, 0x2D, redGain, analogGain);
}

void MainWindow::setGreenGain(int gain)
{
	ui.m_gGain->setText(QString::number(gain));
	greenGain = getGainData(gain);
	m_imageModel.sendSettingCommand(0x00, 0x2B, greenGain, analogGain);
	m_imageModel.sendSettingCommand(0x00, 0x2E, greenGain, analogGain);
}

void MainWindow::setBlueGain(int gain)
{
	ui.m_bGain->setText(QString::number(gain));
	blueGain = getGainData(gain);
	m_imageModel.sendSettingCommand(0x00, 0x2C, blueGain, analogGain);
}

void MainWindow::setGlobalGain(int gain)
{
	ui.m_rGainSet->setValue(gain);
	ui.m_gGainSet->setValue(gain);
	ui.m_bGainSet->setValue(gain);
	ui.m_globalGain->setText(QString::number(gain));
	greenGain = getGainData(gain);
	blueGain = getGainData(gain);
	redGain = getGainData(gain);
	setGain();
}
*/
void MainWindow::setRedGain_1(bool flag)
{
	if (flag == true)
	{
	redGain = getGainData(1);
	m_imageModel.sendSettingCommand(0x00, 0x2D, redGain, analogGain);
	}
}
void MainWindow::setRedGain_2(bool flag)
{
	if (flag == true)
	{
		redGain = getGainData(2);
		m_imageModel.sendSettingCommand(0x00, 0x2D, redGain, analogGain);
	}
}
void MainWindow::setRedGain_4(bool flag)
{
	if (flag == true)
	{
		redGain = getGainData(4);
		m_imageModel.sendSettingCommand(0x00, 0x2D, redGain, analogGain);
	}
}
void MainWindow::setRedGain_8(bool flag)
{
	if (flag == true)
	{
		redGain = getGainData(8);
		m_imageModel.sendSettingCommand(0x00, 0x2D, redGain, analogGain);
	}
}
void MainWindow::setRedGain_16(bool flag)
{
	if (flag == true)
	{
		redGain = getGainData(16);
		m_imageModel.sendSettingCommand(0x00, 0x2D, redGain, analogGain);
	}
}
void MainWindow::setGreenGain_1(bool flag)
{
	if (flag == true)
	{
		greenGain = getGainData(1);
		m_imageModel.sendSettingCommand(0x00, 0x2B, greenGain, analogGain);
		m_imageModel.sendSettingCommand(0x00, 0x2E, greenGain, analogGain);
	}
}
void MainWindow::setGreenGain_2(bool flag)
{
	if (flag == true)
	{
		greenGain = getGainData(2);
		m_imageModel.sendSettingCommand(0x00, 0x2B, greenGain, analogGain);
		m_imageModel.sendSettingCommand(0x00, 0x2E, greenGain, analogGain);
	}
}
void MainWindow::setGreenGain_4(bool flag)
{
	if (flag == true)
	{
		greenGain = getGainData(4);
		m_imageModel.sendSettingCommand(0x00, 0x2B, greenGain, analogGain);
		m_imageModel.sendSettingCommand(0x00, 0x2E, greenGain, analogGain);
	}
}
void MainWindow::setGreenGain_8(bool flag)
{
	if (flag == true)
	{
		greenGain = getGainData(8);
		m_imageModel.sendSettingCommand(0x00, 0x2B, greenGain, analogGain);
		m_imageModel.sendSettingCommand(0x00, 0x2E, greenGain, analogGain);
	}
}
void MainWindow::setGreenGain_16(bool flag)
{
	if (flag == true)
	{
		greenGain = getGainData(16);
		m_imageModel.sendSettingCommand(0x00, 0x2B, greenGain, analogGain);
		m_imageModel.sendSettingCommand(0x00, 0x2E, greenGain, analogGain);
	}
}
void MainWindow::setBlueGain_1(bool flag)
{
	if (flag == true)
	{
		blueGain = getGainData(1);
		m_imageModel.sendSettingCommand(0x00, 0x2C, blueGain, analogGain);
	}
}
void MainWindow::setBlueGain_2(bool flag)
{
	if (flag == true)
	{
		blueGain = getGainData(2);
		m_imageModel.sendSettingCommand(0x00, 0x2C, blueGain, analogGain);
	}
}
void MainWindow::setBlueGain_4(bool flag)
{
	if (flag == true)
	{
		blueGain = getGainData(4);
		m_imageModel.sendSettingCommand(0x00, 0x2C, blueGain, analogGain);
	}
}
void MainWindow::setBlueGain_8(bool flag)
{
	if (flag == true)
	{
		blueGain = getGainData(8);
		m_imageModel.sendSettingCommand(0x00, 0x2C, blueGain, analogGain);
	}
}
void MainWindow::setBlueGain_16(bool flag)
{
	if (flag == true)
	{
		blueGain = getGainData(16);
		m_imageModel.sendSettingCommand(0x00, 0x2C, blueGain, analogGain);
	}
}
//还需要修改逻辑，因为以前滑动条滑动时，其他颜色增益是跟着滑动的
void MainWindow::setGlobalGain_1(bool flag)
{
	if (flag == true)
	{
		ui.m_colorGainFrame->setEnabled(true);
		setRedGain_1(true);
		setGreenGain_1(true);
		setBlueGain_1(true);
		greenGain = getGainData(1);
		blueGain = getGainData(1);
		redGain = getGainData(1);
		setGain(); 	
	}
	else
	{
		ui.m_colorGainFrame->setEnabled(false);
	}
}
void MainWindow::setGlobalGain_2(bool flag)
{
	if (flag == true)
	{
		setRedGain_2(true);
		setGreenGain_2(true);
		setBlueGain_2(true);
		greenGain = getGainData(2);
		blueGain = getGainData(2);
		redGain = getGainData(2);
		setGain();
	}
}
void MainWindow::setGlobalGain_4(bool flag)
{
	if (flag == true)
	{
		setRedGain_4(true);
		setGreenGain_4(true);
		setBlueGain_4(true);
		greenGain = getGainData(4);
		blueGain = getGainData(4);
		redGain = getGainData(4);
		setGain();
	}
}
void MainWindow::setGlobalGain_8(bool flag)
{
	if (flag == true)
	{
		setRedGain_8(true);
		setGreenGain_8(true);
		setBlueGain_8(true);
		greenGain = getGainData(8);
		blueGain = getGainData(8);
		redGain = getGainData(8);
		setGain();
	}
}
void MainWindow::setGlobalGain_16(bool flag)
{
	if (flag == true)
	{
		setRedGain_16(true);
		setGreenGain_16(true);
		setBlueGain_16(true);
		greenGain = getGainData(16);
		blueGain = getGainData(16);
		redGain = getGainData(16);
		setGain();
	}
}
//需要修改
int MainWindow::getGainData(int gain)
{
	switch (gain)
	{
	case 1:
		return 0x00;		
	case 2:
		return 0x08;		
	case 3:
		return 0x10;
	case 4:
		m_imageModel.sendSettingCommand(0x00, 0x3E, 0x00, 0x87);
		return 0x18;
	case 5:
		m_imageModel.sendSettingCommand(0x00, 0x3E, 0x00, 0x87);
		return 0x20;
	case 6:
		m_imageModel.sendSettingCommand(0x00, 0x3E, 0x00, 0x87);
		return 0x28;
	case 7:
		m_imageModel.sendSettingCommand(0x00, 0x3E, 0x00, 0x87);
		return 0x30;
	case 8:
		m_imageModel.sendSettingCommand(0x00, 0x3E, 0x00, 0x87);
		return 0x38;
	case 16:
		m_imageModel.sendSettingCommand(0x00, 0x3E, 0x00, 0x87);
		return 0x78;
	}
}


void MainWindow::setExposureMode(bool isAuto)
{
	if (isAuto)
	{
		ui.m_exposureSlider->setEnabled(false);
		ui.m_exposureSpinBox->setEnabled(false);
		if (ui.m_2592->isChecked())
		{
			switchResolution_2592(true);
		}
		else if (ui.m_2048->isChecked())
		{
			switchResolution_2048(true);
		}
		else if (ui.m_1920->isChecked())
		{
			switchResolution_1920(true);
		}
		else if (ui.m_1280->isChecked())
		{
			switchResolution_1280(true);
		}
		else if (ui.m_640->isChecked())
		{
			switchResolution_640(true);
		}
	
	}
	else
	{
		ui.m_exposureSlider->setEnabled(true); 
		ui.m_exposureSpinBox->setEnabled(true);
	}
}

void MainWindow::setExposureValue(int value)
{
	//	qDebug() << value;
	uchar u3;
	uchar u4;
	if (bits_8_flag)
	{
		switch (resolutionType)
		{
		case 6:
			value = value * 1000 / (2594 /22) + 426 / 2594 ;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		case 5:
			value = value * 1000 / (2050 / 65) + 426 / 2050 ;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		case 4:
			value = value * 1000 / (1922 / 60) + 426 / 1922 ;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		case 3:
			value = value * 1000 / (1282 / 55) + 426 / 1282 ;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		case 2:
			value = value * 1000 / (972 / 85) + 426 / 972 ;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		}
	}
	else
	{
		switch (resolutionType)
		{
		case 6:
			value = value * 1000 / (2594 /12) + 426 / 2594;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		case 5:
			value = value * 1000 / (2050 / 31) + 426 / 2050 ;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		case 4:
			value = value * 1000 / (1922 / 20) + 426 / 1922 ;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		case 3:
			value = value * 1000 / (1282 /31) + 426 / 1282 ;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		case 2:
			value = value * 1000 / (972 / 46) + 426 / 972 ;
			u3 = value >> 8;
			u4 = value;
			m_imageModel.sendSettingCommand(0x00, 0x09, u3, u4);
			break;
		}
	}
}

void MainWindow::chooseSavingPath()
{
	path = QFileDialog::getExistingDirectory(this, QStringLiteral("存储路径选择"), ".");
	if (path == "")
		path = ".";
	m_imageModel.setSavingPath(path);
}

void MainWindow::takeImage()
{
	m_imageModel.takeImage();
}

void MainWindow::showCurve()
{
//	ui.m_customPlot->graph(0);
}

void MainWindow::openImage()
{
	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open Image"), path, tr("Image File ( *.bmp)"));
//	QFile imageFile = filename;
	closeCamera();
	clearPointData();
	ui.m_setLineChange->setEnabled(true);
//	QString dataFilename = filename;
//	dataFilename.replace(QString(".bmp"), QString(".png"));
	ui.m_customPlot->getDataFilename(filename);
	QPixmap pixmap_orig = QPixmap(filename);//QPixmap(filename) 把QString类型转换成const QPixmap & 类型
	openImageWidth = pixmap_orig.width();
	openImageHeight = pixmap_orig.height();
	QPixmap pixmap;
	if (openImageWidth> 1280)
	{
		pixmap = pixmap_orig.scaled(1280, 960, Qt::KeepAspectRatio, Qt::SmoothTransformation);//需要返回一个image	
		ui.m_showLabel->getImageSize(openImageWidth, openImageHeight);
	}
	else
	{
		pixmap = pixmap_orig;
	}
	ui.m_showLabel->setPixmap(pixmap);
	showImageWidth = pixmap.width();
	showImageHeight = pixmap.height();
	ui.m_showLabel->setFixedSize(QSize(showImageWidth, showImageHeight));//m_showFrame的大小为1280*960，而m_showLabel设置为在frame的center位置	
	ui.m_showLabel->getLabelSize(showImageWidth, showImageHeight);
	ui.m_customPlot->getImageSize(openImageWidth, openImageHeight);

	ui.m_horizontalLine->setEnabled(true);
	ui.m_verticalLine->setEnabled(true);
	//ui.m_point->setEnabled(true);
//	openImageFlag = true;
}

void MainWindow::setHorizontalLine()
{
	//QPoint leftButtomRightPoint =event->globalPos();                          //获取全局位置
//	leftButtomRightPoint = ui.m_showLabel->mapFromGlobal(leftButtomRightPoint);
	ui.m_showLabel->setHorizontalLine();
	
}

void MainWindow::setVerticalLine()
{
//	QPoint leftButtomRightPoint = event->globalPos();                          //获取全局位置
	//leftButtomRightPoint = ui.m_showLabel->mapFromGlobal(leftButtomRightPoint);
//	QMouseEvent mouseEvent(QEvent MouseButtonDblClick, QPoint leftButtomRightPoint, Qt::LeftButton, bool LeftButton, bool NoModifier);
	//ui.m_showLabel->mouseDoubleClickEvent;
	
	ui.m_showLabel->setVerticalLine();
}


void MainWindow::setInputDataLine()
{
//	if (openImageFlag)
//	{
	   int beginPoint_x = ui.beginPoint_x->text().toInt();
	   int  beginPoint_y = ui.beginPoint_y->text().toInt();
	   int  endPoint_x = ui.endPoint_x->text().toInt();
	   int  endPoint_y = ui.endPoint_y->text().toInt();
	   bool dataSafe_1 = (beginPoint_x >=0) && (endPoint_x>=0) && (beginPoint_y>=0) && (endPoint_y >=0);
	   bool dataSafe_2 = ( beginPoint_x < openImageWidth+1 )&& ( endPoint_x < openImageWidth+1) && (beginPoint_y < openImageHeight+1) && (endPoint_y < openImageHeight+1);
	   bool dataSafe = dataSafe_1&&dataSafe_2;
	   if (dataSafe)
	   {
		ui.m_showLabel->setInputDataLine(beginPoint_x, beginPoint_y, endPoint_x, endPoint_y);
	    }	
	    else
	    {
		QMessageBox::information(this, QStringLiteral("提示信息"), QStringLiteral("请输入图片尺寸以内的坐标值"));
	    }
//	}
//	else
//	{
//		QMessageBox::information(this, QStringLiteral("提示信息"), QStringLiteral("请打开图片"));
//	}
}

void MainWindow::getPointData(int x ,int y, int data)
{
	QString x_str = QString::number(x);
	QString y_str;
	if (resolutionType==6)
		y_str = QString::number(y_orig_fake_m);
	else
	    y_str = QString::number(y);
	ui.m_point_pos->setText(QString(x_str + ", " + y_str));
	getPoint = true;
	if (openCameraFlag)
	{
		QTime msecsTime;
		msecsTime = QTime::currentTime();
		int msecs = msecsTime.msecsTo(startTime);
		msecs = -msecs;
        //更新曲线
		if (resolutionType == 3)
		{   
			//5次取一平均
			if (updataGraphFlag >= 5)
			{
				gray_avg = (gray_avg + data) / 2 + 0.5;
				ui.m_customPlot->realtimeData(msecs, gray_avg);
				updataGraphFlag = 0;
			}
			else if (updataGraphFlag == 0)
			{
				updataGraphFlag++;
				gray_avg = data;
			}
			else
			{
				updataGraphFlag++;
				gray_avg = (gray_avg + data) / 2 + 0.5;
			}
		}
		else if (resolutionType == 4)
		{
			if (updataGraphFlag >= 3)
			{
				gray_avg = (gray_avg + data) / 2 + 0.5;
				ui.m_customPlot->realtimeData(msecs, gray_avg);		   
				updataGraphFlag = 0;
			}
			else if (updataGraphFlag == 0)
			{
				updataGraphFlag++;
				gray_avg = data;
			}
			else
			{
				updataGraphFlag++;
				gray_avg = (gray_avg + data) / 2 + 0.5;
			}
		}
		else if (resolutionType == 5)
		{
			if (updataGraphFlag >= 1)
			{
				gray_avg = (gray_avg + data) / 2 + 0.5;
				ui.m_customPlot->realtimeData(msecs, gray_avg);
	        	updataGraphFlag = 0;
			}
			else if (updataGraphFlag == 0)
			{
				updataGraphFlag++;
				gray_avg = data;
			}
			else
			{
				updataGraphFlag++;
				gray_avg = (gray_avg + data) / 2 + 0.5;
			}
		}
		else if (resolutionType == 2)
		{
			if (updataGraphFlag >= 15)
			{
				gray_avg = (gray_avg + data) / 2 + 0.5;
				ui.m_customPlot->realtimeData(msecs, gray_avg);
            	updataGraphFlag = 0;
			}
			else if (updataGraphFlag == 0)
			{
				updataGraphFlag++;
				gray_avg = data;
			}
			else
			{
				updataGraphFlag++;
				gray_avg = (gray_avg + data) / 2 + 0.5;
			}
		}
		else
		{
			ui.m_customPlot->realtimeData(msecs, data);
		}
		//显示像素点灰度值
		if (resolutionType == 3)
		{
			if (updateDataFlag >= 27)
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				QString data_str = QString::number(gray_avg_show);
				ui.m_point_data->setText(QString(data_str));
				updateDataFlag = 0;
			}
			else if (updateDataFlag == 0)
			{
				gray_avg_show = data;
				updateDataFlag++;
			}
			else
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				updateDataFlag++;
			}
		}
		else if (resolutionType == 4)
		{
			if (updateDataFlag >= 17)
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				QString data_str = QString::number(gray_avg_show);
				ui.m_point_data->setText(QString(data_str));
				updateDataFlag = 0;
			}
			else if (updateDataFlag == 0)
			{
				gray_avg_show = data;
				updateDataFlag++;
			}
			else
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				updateDataFlag++;
			}
		}
		else if (resolutionType == 5)
		{
			if (updateDataFlag >= 12)
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				QString data_str = QString::number(gray_avg_show);
				ui.m_point_data->setText(QString(data_str));
				updateDataFlag = 0;
			}
			else if (updateDataFlag == 0)
			{
				gray_avg_show = data;
				updateDataFlag++;
			}
			else
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				updateDataFlag++;
			}
		}
		else if (resolutionType == 2)
		{
			if (updateDataFlag >= 95)
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				QString data_str = QString::number(gray_avg_show);
				ui.m_point_data->setText(QString(data_str));
				updateDataFlag = 0;
			}
			else if (updateDataFlag == 0)
			{
				gray_avg_show = data;
				updateDataFlag++;
			}
			else
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				updateDataFlag++;
			}
		}
		else //分辨率类型为2592
		{
			if (updateDataFlag >= 16)//2幅取一平均
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				QString data_str = QString::number(gray_avg_show);
				ui.m_point_data->setText(QString(data_str));
				updateDataFlag = 0;
			}
			else if (updateDataFlag == 0)
			{
				gray_avg_show = data;
				updateDataFlag++;
			}
			else
			{
				gray_avg_show = (gray_avg_show + data) / 2 + 0.5;
				updateDataFlag++;
			}
		}
			
			
	}
}

void MainWindow::clearPointData()
{
	ui.m_point_pos->clear();
	ui.m_point_data->clear();
	ui.m_showLabel->setRepaintFlagFalse();
	getPoint = false;
}

void MainWindow::startCountTime(int x_orig, int y_orig_actual, int y_orig_fake, int x_move_click, int y_move_click)
{
	startTime = QTime::currentTime();
	ui.m_customPlot->clearGraph();
	gray_avg = 0;
	updataGraphFlag = 0;
	updateDataFlag = 0;
	y_orig_fake_m = y_orig_fake;
}
//中心点
void MainWindow::setPointChoosed_o()
{
	switch (resolutionType)
	{
	case 6:
		ui.m_showLabel->setPointChoosed(647, 485, 1295, 0);//在这个分辨率下，传入的yOrig没有使用到
		break;
	case 5:
		ui.m_showLabel->setPointChoosed(639, 479, 1024, 767);
		break;
	case 4:
		ui.m_showLabel->setPointChoosed(639, 359, 959, 539);
		break;
	case 3:
		ui.m_showLabel->setPointChoosed(639, 479, 639, 479);
		break;
	case 2:
		ui.m_showLabel->setPointChoosed(319, 239, 319, 239);
		break;
	}
}

void MainWindow::setPointChoosed_a()
{
	switch (resolutionType)
	{
	case 6:
		ui.m_showLabel->setPointChoosed(0, 0, 0, 9);//在这个分辨率下，传入的yOrig没有使用到
		break;
	case 5:
		ui.m_showLabel->setPointChoosed(0, 0, 0, 0);
		break;
	case 4:
		ui.m_showLabel->setPointChoosed(0, 0, 0, 0);
		break;
	case 3:
		ui.m_showLabel->setPointChoosed(0, 0, 0, 0);
		break;
	case 2:
		ui.m_showLabel->setPointChoosed(0, 0, 0, 0);
		break;
	}
}
void MainWindow::setPointChoosed_b()
{
	switch (resolutionType)
	{
	case 6:
		ui.m_showLabel->setPointChoosed(647, 242, 1279, 0);//在这个分辨率下，传入的yOrig没有使用到
		break;
	case 5:
		ui.m_showLabel->setPointChoosed(639, 239, 1023, 383);
		break;
	case 4:
		ui.m_showLabel->setPointChoosed(639, 179, 959, 269);
		break;
	case 3:
		ui.m_showLabel->setPointChoosed(639, 239, 639, 239);
		break;
	case 2:
		ui.m_showLabel->setPointChoosed(319, 119, 319, 119);
		break;
	}
}
void MainWindow::setPointChoosed_c()
{
	switch (resolutionType)
	{
	case 6:
		ui.m_showLabel->setPointChoosed(322, 485, 959, 0);//在这个分辨率下，传入的yOrig没有使用到
		break;
	case 5:
		ui.m_showLabel->setPointChoosed(319, 479, 511, 767);
		break;
	case 4:
		ui.m_showLabel->setPointChoosed(319, 359, 479, 539);
		break;
	case 3:
		ui.m_showLabel->setPointChoosed(319, 479, 319, 479);
		break;
	case 2:
		ui.m_showLabel->setPointChoosed(159, 239, 159, 239);
		break;
	}
}
void MainWindow::setPointChoosed_d()
{
	switch (resolutionType)
	{
	case 6:
		ui.m_showLabel->setPointChoosed(647, 728, 1279, 0);//在这个分辨率下，传入的yOrig没有使用到
		break;
	case 5:
		ui.m_showLabel->setPointChoosed(639, 719, 1023, 1151);
		break;
	case 4:
		ui.m_showLabel->setPointChoosed(639, 539, 959, 809);
		break;
	case 3:
		ui.m_showLabel->setPointChoosed(639, 719, 639, 719);
		break;
	case 2:
		ui.m_showLabel->setPointChoosed(319, 359, 319, 359);
		break;
	}
}
void MainWindow::setPointChoosed_e()
{
	switch (resolutionType)
	{
	case 6:
		ui.m_showLabel->setPointChoosed(1295, 971, 2591, 0);//在这个分辨率下，传入的yOrig没有使用到
		break;
	case 5:
		ui.m_showLabel->setPointChoosed(1279, 959, 2047, 1535);
		break;
	case 4:
		ui.m_showLabel->setPointChoosed(1279, 719, 1919, 1079);
		break;
	case 3:
		ui.m_showLabel->setPointChoosed(1279,959, 1279, 959);
		break;
	case 2:
		ui.m_showLabel->setPointChoosed(639, 479, 639, 479);
		break;
	}
}

void MainWindow::getPointInput()
{
	int point_x = ui.point_x->text().toInt();
	int point_y = ui.point_y->text().toInt();
	switch (resolutionType)
	{
	case 6:
		if ((point_x >= 0) && (point_x<2592) && (point_y >= 0) && (point_y<1944))
	      	ui.m_showLabel->setPointChoosed(point_x / 2, point_y/2, point_x, point_y);//在这个分辨率下，传入的yOrig没有使用到
		else
			QMessageBox::warning(this, QStringLiteral("提示信息"), QStringLiteral("请输入图片尺寸以内的坐标值"));
		break;
	case 5:
		if ((point_x >= 0) && (point_x<2048) && (point_y >= 0) && (point_y<1536))
			ui.m_showLabel->setPointChoosed(point_x / 1.6, point_y / 1.6, point_x, point_y);//需要修改前两个显示坐标值
		else
			QMessageBox::warning(this, QStringLiteral("提示信息"), QStringLiteral("请输入图片尺寸以内的坐标值"));
		break;
	case 4:
		if ((point_x >= 0) && (point_x<1920) && (point_y >= 0) && (point_y<1080))
			ui.m_showLabel->setPointChoosed(point_x / 1.5, point_y / 1.5, point_x, point_y);//需要修改前两个显示坐标值
		else
			QMessageBox::warning(this, QStringLiteral("提示信息"), QStringLiteral("请输入图片尺寸以内的坐标值"));
		break;
	case 3:
		if ((point_x >= 0) && (point_x<1280) && (point_y >= 0) && (point_y<960))
		ui.m_showLabel->setPointChoosed(point_x, point_y, point_x, point_y);
		else
			QMessageBox::warning(this, QStringLiteral("提示信息"), QStringLiteral("请输入图片尺寸以内的坐标值"));
		break;
	case 2:
		if ((point_x >= 0) && (point_x<640) && (point_y >= 0) && (point_y<480))
		ui.m_showLabel->setPointChoosed(point_x, point_y, point_x, point_y);
		else
			QMessageBox::warning(this, QStringLiteral("提示信息"), QStringLiteral("请输入图片尺寸以内的坐标值"));
		break;
	}
}

void MainWindow::takeDarkImage()
{
	//等暗图像拍完了再settrue
	ui.m_spacial_nonuniformity->setEnabled(false);
	m_imageModel.takeImage(true,16);//darkFlag=true
//	QToolTip::setFont(QFont("Agency FB", 20));
//	QToolTip::showText(QPoint(1150, 700), QStringLiteral("正在拍摄，请稍后……"));//鼠标一动qtooltip就没了
//	takingImageFlag = true;
	QMessageBox::information(this, QStringLiteral("提示信息"), QStringLiteral("正在处理，请稍后……"), QMessageBox::NoButton);

}

void MainWindow::takeLightImage()
{
	ui.m_spacial_nonuniformity->setEnabled(false);
	m_imageModel.takeImage(false,16);
//	QToolTip::setFont(QFont("Agency FB", 20));
//	QToolTip::showText(QPoint(1150, 700), QStringLiteral("正在拍摄，请稍后……"));
//	takingImageFlag = true;
	QMessageBox::information(this, QStringLiteral("提示信息"), QStringLiteral("正在处理，请稍后……"), QMessageBox::NoButton);
}

void MainWindow::countPRNU()
{
	m_imageModel.countPRNU();
}

void MainWindow::countDSNU()
{
	m_imageModel.countDSNU();
}

void MainWindow::setNonuniformityEnable()
{
	ui.m_spacial_nonuniformity->setEnabled(true);
	//QToolTip::hideText();
//	takingImageFlag = false;
	//messageBox.QMessageBox::~QMessageBox();
	QMessageBox::information(this, QStringLiteral("提示信息"), QStringLiteral("拍摄完成"), QMessageBox::NoButton);
}

void MainWindow::showPRNU(double PRNU)
{
	QString PRNU_show = QString::number(PRNU);
	ui.m_PRNU_show->setText(QString(PRNU_show));
}

void MainWindow::wrongTipsOfPRNU(bool flag)
{
	if (flag)
		QMessageBox::warning(this, QStringLiteral("提示信息"), QStringLiteral("请设置正确的拍摄环境，并重新拍摄图像"));
	else
		QMessageBox::warning(this, QStringLiteral("提示信息"), QStringLiteral("请先拍摄暗图像和亮图像"));
}

void MainWindow::showDSNU(double DSNU)
{
	QString DSNU_show = QString::number(DSNU);
	ui.m_DSNU_show->setText(QString(DSNU_show));
}

void MainWindow::wrongTipsOfDSNU(bool flag)
{
	if (flag)
		QMessageBox::warning(this, QStringLiteral("提示信息"), QStringLiteral("请设置正确的拍摄环境，并重新拍摄图像"));
	else
		QMessageBox::warning(this, QStringLiteral("提示信息"), QStringLiteral("请先拍摄暗图像"));
}
