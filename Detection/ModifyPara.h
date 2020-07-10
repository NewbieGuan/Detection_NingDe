#pragma once
#include "afxwin.h"


// CModifyPara 对话框

class CModifyPara : public CDialogEx
{
	DECLARE_DYNAMIC(CModifyPara)

public:
	CModifyPara(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModifyPara();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MODIFY_PARA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CFont m_font;

	void SaveSystemPara();   // 保存系统参数函数
	void GetIniSystemPara(); // 获取系统参数函数

public:
	afx_msg void OnBnClickedBtnModifyParaOk();
	afx_msg void OnBnClickedBtnModifyParaCanceal();
	afx_msg void OnBnClickedBtnModifyParaApply();
	CString m_sResolution;
	CString m_sDelayTime;
	CString m_sPulseEquivalent;
	//virtual void OnOK();
	virtual BOOL OnInitDialog();
	CStatic m_cTip;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CString m_sThreshold;
	CString m_sSampleCountStrip;

	//4.10
	CString m_sGrayThreshold;
};
