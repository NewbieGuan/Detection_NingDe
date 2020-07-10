#pragma once

#include "SapClassBasic.h"
#include "SapClassGui.h"

#include <afxdisp.h>
#include "CApplication.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "CRange.h"
#include "CFont0.h"

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>

#include "afxwin.h"
#include <vector>
#include <wtypes.h>


using namespace std;
using namespace cv;

class CCameraImage : public CImageExWndEventHandler
{
public:
	CCameraImage();
	~CCameraImage();


public:
	BITMAPINFO *m_pBmpInfo;         // BITMAPINFO结构指针，显示图像时使用
	RGBQUAD *pColorTable;           // 颜色表
	unsigned char *pBmpData;        // 数据指针
	

	unsigned char *pBmpDataShow;    // 存放显示条图像数据

	unsigned char *pBmpDataClear;   // 显示和界面一样颜色的图像，用于刷新图像
	void ReflashImageDlg();

	BITMAPINFO *pBmpInfoStrip;   // 条图像信息头
	BITMAPFILEHEADER bmpHeader;  // 头文件
	BITMAPINFOHEADER bmpInfo;    // 信息头


	void TransferData(unsigned char *pData, int nWidth, int nHeight);    // 显示图像

	void SaveImage(unsigned char *pData, int nWidth, int nHeight, CString strPath); // 保存图像 
	
	BOOL PageShow(int flag);             // 上下翻页显示图像

	void ShowColorStripImage(CDC *pDC,CRect rcDraw);            // 显示一条图像


	// ******************************【计算、结果写入】*****************************
	// ////////////【修改后】
	void ImageProcessAccuracy(CWnd *pWnd = NULL);                              // 处理图像   
	//void ComputeAccuracy(Mat& srcImg, int i, int j);       // 计算各个参数
	void ComputeAccuracy(Mat& srcImg, int i);       // 计算各个参数
	void WriteResultToExcelAccuracy(CString strpath, BOOL flagFirst);        // 写入结果到文件


	int saveIndex;   // 保存小图索引
	int stripIndex;  // 图像条索引
	int stripShowIndex;  // 大条图像显示索引
	int stripCount;  // 图像条数量
	int frameCount;  // 每一条图像帧数
	int frameCountIndex;  // 一大条图像一帧索引
	int cropLine;          // 裁减线
	int m_pageUpValue;     // 显示边界上值
	int m_pageDownValue;   // 显示边界下值
	void ResetAllIndex();  // 重置所有索引

	// 设置图像参数
	void SetImagePara(double m_resolution, int threshold, int Count, int threshold2);  //3.28下午
	// 图像相关变量
	double m_resolution;    // 分辨率
	int m_threshold;     // 阈值
	int m_DiaThreshold;  // 过滤颗粒阈值（粒径）
	int OtsuAlgThreshold(Mat image);  // 计算Otsu阈值

	//4.10
	int graythreshold;   //灰度阈值减数

	//3.28  3.28下午
	/////////////////////////////////////////////新加
	//3.24修改
	int sampleCountStrip;   // 每条图像取样数量
	int sampleShowIndex;    // 取样显示索引


	BOOL CreateObjects();
	BOOL DestroyObjects();
	BOOL LoadConfig();
	BOOL StartGrab();
	BOOL StopGrab();
	static void XferCallback(SapXferCallbackInfo *pInfo);
	BOOL IfGrabStart();
	CString Get_Error();

	CString m_strErrorMsg;
	BOOL m_bIsLoadConfig;

	vector<CString> vecStrStripName;  // 保存条图像的名字

	void SetFrameCount(int count);   // 设置条图像帧数
	void SetStripCount(int count);   // 设置条图像数量


	// 计算结果数据
	double m_area;        // 计算平均面积
	double m_length;      // 计算平均周长
	double m_ratioArea;   // 计算的面积率
	double areaMax;       // 最大面积
	double areaMin;       // 最小面积


	// 保存一条图像结果
	vector<long> vecAreaCount;    // 数量
	vector<double> vecMeanArea;   // 平均面积结果
	vector<double> vecMeanLength; // 平均周长结果
	vector<double> vecRatioArea;  // 计算的面积率
	vector<double> vecAreaMax;    // 最大面积
	vector<double> vecAreaMin;    // 最小面积
	


	// 精确计算保存参数
	long valueCount;         // 数量
	double meanArea;         // 平均面积结果
	double meanLength;       // 平均周长结果
	double ratioArea;        // 面积率
	double ratioAreaNew;        // 面积大于90000颗粒总面积占总颗粒面积的比例  3.7


	double meanMinDia;       // 平均最小粒径
	double meanMaxDia;       // 平均最大粒径

	double maxBigDia;        // 所有最大大径
	double minBigDia;        // 所有最小大径

	double maxSmallDia;      // 所有最大小径
	double minSmallDia;      // 所有最小小径

	double maxLength;        // 所有最大周长
	double minLength;        // 所有最小周长

	double maxArea;          // 所有最大面积
	double minArea;          // 所有最小面积

	double sumBigDia;        // 最大粒径之和
	double sumSmallDia;      // 最小粒径之和

	double sumLength;        // 周长和
	double sumArea;          // 面积和

	double maxDiaSD;         // 最大粒径标准差
	double minDiaSD;       // 最小粒径标准差
	double lengthSD;         // 周长的标准差
	double areaSD;           // 面积的标准差

	vector<double> vecArea;     // 面积
	vector<double> vecLength;   // 周长
	vector<double> vecMaxDia;   // 每个最大粒径
	vector<double> vecMinDia;   // 每个最小粒径

	double areaStrip;         // 每一条的面积

	int detectCount;          // 检测次数记录
	void SetDetectCount(int count);   // 设置检测次数



	CTime startTime;       // 检测时间用于，作为文件夹名，统计结果文件名
	CString startTimeCStr;  // 检测开始时间CString
	CString childFolderCStr;  // 图片子文件夹名CString
	String childFolderStr;    // 图片子文件夹名String
	CString childFolderExcelCStr;  // 检测结果子文件夹CString
	char *childFolderP;        // 子文件夹名char*
	void GetStartTime();   // 得到检测开始时间
	void MkdirPic();          // 新建图片文件夹


	///*
	// 结果写入到文件相关变量
	CApplication app;
	CWorkbook book;
	CWorkbooks books;
	CWorksheet sheet;
	CWorksheets sheets;
	CRange range;
	CFont0 font;
	COleVariant vResult;
	LPDISPATCH lpDisp;
	VARIANT val;

	//*/


	public:
	
	CString m_appTitle;
	SapAcqDevice *m_AcqDevice;
	SapBuffer *m_Buffers;
	SapTransfer *m_Xfer;

	/**/
	BYTE *pData;
	//BYTE *pRawData;

};

