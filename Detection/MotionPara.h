#pragma once
#include "afxwin.h"


// CMotionPara 对话框

class CMotionPara : public CDialogEx
{
	DECLARE_DYNAMIC(CMotionPara)

public:
	CMotionPara(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMotionPara();

// 对话框数据
	enum { IDD = IDD_DIALOG_MOTION_PARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// CStatic m_picture;
	virtual void OnCancel();
	CString m_sSetDistanceCamera;
	CString m_sSetDistancePlatform;
	CString m_sSetSpeedCamera;
	CString m_sSetSpeedPlatform;
	afx_msg void OnBnClickedBtnMotionOk();
	afx_msg void OnBnClickedBtnMotionCanceal();
	afx_msg void OnBnClickedBtnMotionApply();
};
