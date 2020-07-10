
// DetectionDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "MotionPara.h"
#include "MotionAdjust.h"
#include "CameraImage.h"
#include "afxcmn.h"
#include "ModifyPara.h"

#define WM_MOTORSTOP  WM_USER+100	





// CDetectionDlg �Ի���
class CDetectionDlg : public CDialogEx
{
// ����
public:
	CDetectionDlg(CWnd* pParent = NULL);	// ��׼���캯��
	virtual ~CDetectionDlg();        // ��������

// �Ի�������
	enum { IDD = IDD_DETECTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CMenu m_menu;    // �˵�

	CRect m_DlgRect;    // �洢�Ի���ı�ǰ�Ĵ�С���Լ���ؼ���Ӧλ�ü���Сʱʹ��

	void repaint(UINT id, int last_width, int now_width, int last_height, int now_height);         // ���»��ƿؼ�
	void repaintLogo(UINT id, CRect last_rect, CRect now_rect);     // ���·���LOGO
	void repaintLeftBTN(UINT id, CRect last_rect, CRect now_rect);  // ���·�����߰�ť
	void repaintRightBTN(UINT id, CRect last_rect, CRect now_rect);  // ���·�����߰�ť


	CFont m_font_time_hms;     // ʱ��ʱ��������
	CFont m_font_time_weekDay; // ��������������
	CFont m_font_button_label; // ��߰�ť��ǩ����


	CMotionAdjust dlgMotionAdjust;   // �ֶ��˶����ڶԻ���
	CMotionPara dlgMotionPara;       // �˶����Ʋ������öԻ���
	CModifyPara dlgModifyPara;       // �޸�ϵͳ�����Ի���
	

	CCameraImage m_camImg;           // ���ͼ������

	BOOL m_IsCameraConnected;          // �Ƿ������������LoadConfig�н��и�ֵ 

	BOOL stopdetectionflag;           //�Ƿ��ѵ��ֹͣ���  �����ֹͣ����stopdetectionflag��ΪFALSE


	void InitBoard();     // ��ʼ�����ƿ�

	int stepCamera;   //�������������
	int stepPlatform;   //ƽ̨����������
	// int directionCamera;
	int speedCamera, speedPlatform;


	double m = 2.056;  // �ص���
	double d = 42.0;  // ÿ���˶�����


	int m_sSpeedCamera;        // ����˶��ٶ�
	int m_sDistanceCamera;     // ����˶�����
	int m_sSpeedPlatform;      // ƽ̨�˶��ٶ�
	int m_sDistancePlatform;   // ƽ̨�˶�����
	int frameCountDlg;         // һ����ͼ���֡��
	int stripCountDlg;         // ͼ������

	// ϵͳ��������
	void GetSystemPara();     // ���ϵͳ����
	// ϵͳ��������
	int m_steps_mm;     // ÿ����������
	int m_delayTime;    // �ӳ�
	double m_resolution;   // �ֱ���
	int m_threshold;    // ��ֵ

	int sampleCountStrip; //ÿ��ȡ��֡�� 3.26�޸�  3.28����

	//4.10
	int graythreshold;    //�Ҷ���ֵ����


	//2019.5.31  ͼ�����ű���
	double changeratiodlg;
	//�µ�ͼ��߶ȣ�����2048
	int heightnew;  


	// �༭����Ϣ���
	CString strInfo;    // �ڱ༭���������Ϣ
	int infoLines;      // �����Ϣ����
	CFont fontEditInfo; // �༭�������


	int detectCountDlg;   // ����������������������������һ�μ��Ĵ���2019.5.31

	bool m_bStopFlag;




// ʵ��
protected:
	HICON m_hIcon;

	CBrush m_brush;
	
	//3.24����ʦ�¼Ӷ��߳�
	static UINT OnThreadFunc(LPVOID lpParam);
	void OnMotorMoving();
	LRESULT OnMotorStop(WPARAM w,LPARAM l);
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_time_hms;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CStatic m_time_weekDay;
	afx_msg void OnBnClickedBtnOpenPicture();
	afx_msg void OnBnClickedBtnOpenResult();
	afx_msg void OnBnClickedBtnMotionPara();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnStartDetection();
	afx_msg void OnBnClickedBtnMotionAdjust();
	CButton m_buttonMotion;

	CButton m_buttonStatistic;
	CButton m_buttonOpenPicture;
	CButton m_buttonOpenResult;
	CStatic m_label_motionAdjust;
	CStatic m_label_openPicture;
	CStatic m_label_openResult;
	CStatic m_label_imagePara;
	CStatic m_label_motionPara;
	CStatic m_label_statistic;
	CButton m_buttonExit;
	CButton m_buttonStartDetection;
	CButton m_buttonStopDetection;
	CButton m_buttonMotionAdjust;
	afx_msg void OnBnClickedBtnLoadTest();
	
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnStopDetection();
	CListCtrl m_list;
	afx_msg void OnModifySystemPara();
	CEdit m_infoOut;
	CScrollBar m_horizon_scrollbar;
	CSpinButtonCtrl m_spin;
	int m_pageUp_value;
	int m_pageDowm_value;
	afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
	
	CProgressCtrl m_progressImage;
	CButton m_buttonLoad;
	CStatic m_label_loadConfig;
	afx_msg void OnDestroy();
};
