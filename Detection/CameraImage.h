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
	BITMAPINFO *m_pBmpInfo;         // BITMAPINFO�ṹָ�룬��ʾͼ��ʱʹ��
	RGBQUAD *pColorTable;           // ��ɫ��
	unsigned char *pBmpData;        // ����ָ��
	

	unsigned char *pBmpDataShow;    // �����ʾ��ͼ������

	unsigned char *pBmpDataClear;   // ��ʾ�ͽ���һ����ɫ��ͼ������ˢ��ͼ��
	void ReflashImageDlg();

	BITMAPINFO *pBmpInfoStrip;   // ��ͼ����Ϣͷ
	BITMAPFILEHEADER bmpHeader;  // ͷ�ļ�
	BITMAPINFOHEADER bmpInfo;    // ��Ϣͷ


	void TransferData(unsigned char *pData, int nWidth, int nHeight);    // ��ʾͼ��

	void SaveImage(unsigned char *pData, int nWidth, int nHeight, CString strPath); // ����ͼ�� 
	
	BOOL PageShow(int flag);             // ���·�ҳ��ʾͼ��

	void ShowColorStripImage(CDC *pDC,CRect rcDraw);            // ��ʾһ��ͼ��


	// ******************************�����㡢���д�롿*****************************
	// ////////////���޸ĺ�
	void ImageProcessAccuracy(CWnd *pWnd = NULL);                              // ����ͼ��   
	//void ComputeAccuracy(Mat& srcImg, int i, int j);       // �����������
	void ComputeAccuracy(Mat& srcImg, int i);       // �����������
	void WriteResultToExcelAccuracy(CString strpath, BOOL flagFirst);        // д�������ļ�


	int saveIndex;   // ����Сͼ����
	int stripIndex;  // ͼ��������
	int stripShowIndex;  // ����ͼ����ʾ����
	int stripCount;  // ͼ��������
	int frameCount;  // ÿһ��ͼ��֡��
	int frameCountIndex;  // һ����ͼ��һ֡����
	int cropLine;          // �ü���
	int m_pageUpValue;     // ��ʾ�߽���ֵ
	int m_pageDownValue;   // ��ʾ�߽���ֵ
	void ResetAllIndex();  // ������������

	// ����ͼ�����
	void SetImagePara(double m_resolution, int threshold, int Count, int threshold2);  //3.28����
	// ͼ����ر���
	double m_resolution;    // �ֱ���
	int m_threshold;     // ��ֵ
	int m_DiaThreshold;  // ���˿�����ֵ��������
	int OtsuAlgThreshold(Mat image);  // ����Otsu��ֵ

	//4.10
	int graythreshold;   //�Ҷ���ֵ����

	//3.28  3.28����
	/////////////////////////////////////////////�¼�
	//3.24�޸�
	int sampleCountStrip;   // ÿ��ͼ��ȡ������
	int sampleShowIndex;    // ȡ����ʾ����


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

	vector<CString> vecStrStripName;  // ������ͼ�������

	void SetFrameCount(int count);   // ������ͼ��֡��
	void SetStripCount(int count);   // ������ͼ������


	// ����������
	double m_area;        // ����ƽ�����
	double m_length;      // ����ƽ���ܳ�
	double m_ratioArea;   // ����������
	double areaMax;       // ������
	double areaMin;       // ��С���


	// ����һ��ͼ����
	vector<long> vecAreaCount;    // ����
	vector<double> vecMeanArea;   // ƽ��������
	vector<double> vecMeanLength; // ƽ���ܳ����
	vector<double> vecRatioArea;  // ����������
	vector<double> vecAreaMax;    // ������
	vector<double> vecAreaMin;    // ��С���
	


	// ��ȷ���㱣�����
	long valueCount;         // ����
	double meanArea;         // ƽ��������
	double meanLength;       // ƽ���ܳ����
	double ratioArea;        // �����
	double ratioAreaNew;        // �������90000���������ռ�ܿ�������ı���  3.7


	double meanMinDia;       // ƽ����С����
	double meanMaxDia;       // ƽ���������

	double maxBigDia;        // ��������
	double minBigDia;        // ������С��

	double maxSmallDia;      // �������С��
	double minSmallDia;      // ������СС��

	double maxLength;        // ��������ܳ�
	double minLength;        // ������С�ܳ�

	double maxArea;          // ����������
	double minArea;          // ������С���

	double sumBigDia;        // �������֮��
	double sumSmallDia;      // ��С����֮��

	double sumLength;        // �ܳ���
	double sumArea;          // �����

	double maxDiaSD;         // ���������׼��
	double minDiaSD;       // ��С������׼��
	double lengthSD;         // �ܳ��ı�׼��
	double areaSD;           // ����ı�׼��

	vector<double> vecArea;     // ���
	vector<double> vecLength;   // �ܳ�
	vector<double> vecMaxDia;   // ÿ���������
	vector<double> vecMinDia;   // ÿ����С����

	double areaStrip;         // ÿһ�������

	int detectCount;          // ��������¼
	void SetDetectCount(int count);   // ���ü�����



	CTime startTime;       // ���ʱ�����ڣ���Ϊ�ļ�������ͳ�ƽ���ļ���
	CString startTimeCStr;  // ��⿪ʼʱ��CString
	CString childFolderCStr;  // ͼƬ���ļ�����CString
	String childFolderStr;    // ͼƬ���ļ�����String
	CString childFolderExcelCStr;  // ��������ļ���CString
	char *childFolderP;        // ���ļ�����char*
	void GetStartTime();   // �õ���⿪ʼʱ��
	void MkdirPic();          // �½�ͼƬ�ļ���


	///*
	// ���д�뵽�ļ���ر���
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

