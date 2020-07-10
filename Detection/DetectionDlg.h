
// DetectionDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "MotionPara.h"
#include "MotionAdjust.h"
#include "CameraImage.h"
#include "afxcmn.h"
#include "ModifyPara.h"

#define WM_MOTORSTOP  WM_USER+100	





// CDetectionDlg 对话框
class CDetectionDlg : public CDialogEx
{
// 构造
public:
	CDetectionDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CDetectionDlg();        // 析构函数

// 对话框数据
	enum { IDD = IDD_DETECTION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CMenu m_menu;    // 菜单

	CRect m_DlgRect;    // 存储对话框改变前的大小，以计算控件相应位置及大小时使用

	void repaint(UINT id, int last_width, int now_width, int last_height, int now_height);         // 重新绘制控件
	void repaintLogo(UINT id, CRect last_rect, CRect now_rect);     // 重新放置LOGO
	void repaintLeftBTN(UINT id, CRect last_rect, CRect now_rect);  // 重新放置左边按钮
	void repaintRightBTN(UINT id, CRect last_rect, CRect now_rect);  // 重新放置左边按钮


	CFont m_font_time_hms;     // 时间时分秒字体
	CFont m_font_time_weekDay; // 星期年月日字体
	CFont m_font_button_label; // 左边按钮标签字体


	CMotionAdjust dlgMotionAdjust;   // 手动运动调节对话框
	CMotionPara dlgMotionPara;       // 运动控制参数设置对话框
	CModifyPara dlgModifyPara;       // 修改系统参数对话框
	

	CCameraImage m_camImg;           // 相机图像处理类

	BOOL m_IsCameraConnected;          // 是否连接相机，在LoadConfig中进行赋值 

	BOOL stopdetectionflag;           //是否已点击停止检测  点击后停止检测后stopdetectionflag变为FALSE


	void InitBoard();     // 初始化控制卡

	int stepCamera;   //相机轴总脉冲数
	int stepPlatform;   //平台轴总脉冲数
	// int directionCamera;
	int speedCamera, speedPlatform;


	double m = 2.056;  // 重叠量
	double d = 42.0;  // 每次运动距离


	int m_sSpeedCamera;        // 相机运动速度
	int m_sDistanceCamera;     // 相机运动距离
	int m_sSpeedPlatform;      // 平台运动速度
	int m_sDistancePlatform;   // 平台运动距离
	int frameCountDlg;         // 一大条图像的帧数
	int stripCountDlg;         // 图像条数

	// 系统参数设置
	void GetSystemPara();     // 获得系统参数
	// 系统参数变量
	int m_steps_mm;     // 每毫米脉冲数
	int m_delayTime;    // 延迟
	double m_resolution;   // 分辨率
	int m_threshold;    // 阈值

	int sampleCountStrip; //每条取样帧数 3.26修改  3.28下午

	//4.10
	int graythreshold;    //灰度阈值减数


	//2019.5.31  图像缩放比例
	double changeratiodlg;
	//新的图像高度，代替2048
	int heightnew;  


	// 编辑框信息输出
	CString strInfo;    // 在编辑框中输出信息
	int infoLines;      // 输出信息行数
	CFont fontEditInfo; // 编辑框的字体


	int detectCountDlg;   // 检测次数，这个变量来保存软件启动一次检测的次数2019.5.31

	bool m_bStopFlag;




// 实现
protected:
	HICON m_hIcon;

	CBrush m_brush;
	
	//3.24刘老师新加多线程
	static UINT OnThreadFunc(LPVOID lpParam);
	void OnMotorMoving();
	LRESULT OnMotorStop(WPARAM w,LPARAM l);
	// 生成的消息映射函数
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
