#pragma once


// CMotionAdjust 对话框

class CMotionAdjust : public CDialogEx
{
	DECLARE_DYNAMIC(CMotionAdjust)

public:
	CMotionAdjust(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMotionAdjust();

// 对话框数据
	enum { IDD = IDD_DIALOG_MOTION_ADJUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	BOOL m_bShowDlg;

	int InitBoard();

	DECLARE_MESSAGE_MAP()

public:
	void InitParam();
	void SetRadioButton();
	int m_iDirectionCamera;         // 用于保存相机运动方向单选按钮ID号
	int m_iDirectionPlatform;       // 用于保存平台运动方向单选按钮ID号

	int m_steps_mm;


public:
	virtual void OnCancel();
	CString m_sDistanceCamera;
	CString m_sDistancePlatform;
	CString m_sSpeedCamera;
	CString m_sSpeedPlatform;
	afx_msg void OnBnClickedRadioPositiveCamera();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnMoveCamera();
	afx_msg void OnBnClickedBtnMovePlatform();
	afx_msg void OnBnClickedRadioNegativeCamera();
	afx_msg void OnBnClickedRadioPositivePlatform();
	afx_msg void OnBnClickedRadioNegativePlatform();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnHomeCamera();
	afx_msg void OnBnClickedBtnHomePlatform();
	CString m_offset_camera;
	CString m_offset_platform;
};
