// MotionAdjust.cpp : 实现文件
//

#include "stdafx.h"
#include "Detection.h"
#include "MotionAdjust.h"
#include "afxdialogex.h"

#include "MPC2810.h"

long glTotalCards;
long glTotalAxes;

// CMotionAdjust 对话框

IMPLEMENT_DYNAMIC(CMotionAdjust, CDialogEx)

CMotionAdjust::CMotionAdjust(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMotionAdjust::IDD, pParent)
	, m_sDistanceCamera(_T(""))
	, m_sDistancePlatform(_T(""))
	, m_sSpeedCamera(_T(""))
	, m_sSpeedPlatform(_T(""))
	, m_offset_camera(_T("-950"))
	, m_offset_platform(_T("30"))
{
	
}

CMotionAdjust::~CMotionAdjust()
{
}

void CMotionAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_CAMERA, m_sDistanceCamera);
	DDX_Text(pDX, IDC_EDIT_DISTANCE_PLATFORM, m_sDistancePlatform);
	DDX_Text(pDX, IDC_EDIT_SPEED_CAMERA, m_sSpeedCamera);
	DDX_Text(pDX, IDC_EDIT_SPEED_PLATFORM, m_sSpeedPlatform);
	DDX_Text(pDX, IDC_EDIT_OFFSET_CAMERA, m_offset_camera);
	DDX_Text(pDX, IDC_EDIT_OFFSET_PLATFORM, m_offset_platform);
}


BEGIN_MESSAGE_MAP(CMotionAdjust, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_POSITIVE_CAMERA, &CMotionAdjust::OnBnClickedRadioPositiveCamera)
	ON_BN_CLICKED(IDC_BTN_MOVE_CAMERA, &CMotionAdjust::OnBnClickedBtnMoveCamera)
	ON_BN_CLICKED(IDC_BTN_MOVE_PLATFORM, &CMotionAdjust::OnBnClickedBtnMovePlatform)
	ON_BN_CLICKED(IDC_RADIO_NEGATIVE_CAMERA, &CMotionAdjust::OnBnClickedRadioNegativeCamera)
	ON_BN_CLICKED(IDC_RADIO_POSITIVE_PLATFORM, &CMotionAdjust::OnBnClickedRadioPositivePlatform)
	ON_BN_CLICKED(IDC_RADIO_NEGATIVE_PLATFORM, &CMotionAdjust::OnBnClickedRadioNegativePlatform)
	ON_BN_CLICKED(IDC_BTN_STOP, &CMotionAdjust::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_HOME_CAMERA, &CMotionAdjust::OnBnClickedBtnHomeCamera)
	ON_BN_CLICKED(IDC_BTN_HOME_PLATFORM, &CMotionAdjust::OnBnClickedBtnHomePlatform)
END_MESSAGE_MAP()


// CMotionAdjust 消息处理程序


BOOL CMotionAdjust::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 初始化设备
	InitBoard();

	// 运动参数设置初始化
	InitParam();
	SetRadioButton();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}



void CMotionAdjust::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类

	m_bShowDlg = FALSE;

	CDialogEx::OnCancel();		
}


int CMotionAdjust::InitBoard()
{

	///*
	// code start
	int i;

	//初始化板卡
	glTotalAxes = auto_set();			//自动设置
	if (glTotalAxes <= 0)
	{
		MessageBox(_T("检测不到卡!"));
		return -1;
	}

	glTotalCards = init_board();		//初始化
	if (glTotalCards <= 0)
	{
		MessageBox(_T("初始化错误!"));
		return -1;
	}

	for (i = 0; i<glTotalAxes; i++)
	{
		set_sd_logic(i + 1, 0);
		set_el_logic(i + 1, 0);
		set_org_logic(i + 1, 0);
		set_alm_logic(i + 1, 0);
	}
	// code end
	//*/

	return 0;
}

void CMotionAdjust::InitParam()
{
	m_iDirectionCamera = IDC_RADIO_POSITIVE_CAMERA;        // 初始化相机运动方向为正向

	m_iDirectionPlatform = IDC_RADIO_POSITIVE_PLATFORM;    // 初始化平台为运动方向为正

	m_sSpeedCamera = "3000";         // 初始化相机运动速度  3000 mm/min
	m_sDistanceCamera = "20";      // 初始化相机运动距离   20 mm

	m_sSpeedPlatform = "3000";       // 初始化平台运动速度  600 mm/min
	m_sDistancePlatform = "20";    // 初始化平台运动距离    20 mm


	CString strPluse;
	::GetPrivateProfileString(_T("SaveInfo"), _T("脉冲当量"), strPluse, strPluse.GetBuffer(MAX_PATH), MAX_PATH, _T("..\\SystemPara.ini"));
	GetDlgItem(IDC_EDIT_MODIFY_MOTION_PARA_PULSE)->SetWindowText(strPluse);
	m_steps_mm = _ttoi(strPluse); //200

	// 电机每转需要的脉冲数   注意换算脉冲当量
	set_steps_pr(1, 10 * m_steps_mm);     // 轴1   相机
	set_steps_pr(2, 10 * m_steps_mm);     // 轴2   平台


	// /*
	// -----------------【轴的限位、电平】--------------------
	// 轴1
	enable_el(1, 1);    // 轴1限位
	set_el_logic(1, 1); // 轴1限位高电平有效
	// 轴2
	enable_el(2, 1);    // 轴2限位
	set_el_logic(2, 1); // 轴2限位高电平有效
	// */

	///*
	// 轴的原点、电平
	// 轴1
	enable_org(1, 1);    // 轴1原点
	set_org_logic(1, 1); // 轴1原点高电平
	// 轴2
	enable_org(2, 1);    // 轴2原点
	set_org_logic(2, 1); // 轴2运点高电平
	//*/

	UpdateData(FALSE);

}


// 设置单选按钮状态
void CMotionAdjust::SetRadioButton()
{
	CheckRadioButton(IDC_RADIO_POSITIVE_CAMERA, IDC_RADIO_NEGATIVE_CAMERA, m_iDirectionCamera);
	CheckRadioButton(IDC_RADIO_POSITIVE_PLATFORM, IDC_RADIO_NEGATIVE_PLATFORM, m_iDirectionPlatform);
}



void CMotionAdjust::OnBnClickedRadioPositiveCamera()
{
	// TODO:  在此添加控件通知处理程序代码

	m_iDirectionCamera = IDC_RADIO_POSITIVE_CAMERA;

	SetRadioButton();
}


void CMotionAdjust::OnBnClickedRadioNegativeCamera()
{
	// TODO:  在此添加控件通知处理程序代码

	m_iDirectionCamera = IDC_RADIO_NEGATIVE_CAMERA;

	SetRadioButton();
}


void CMotionAdjust::OnBnClickedRadioPositivePlatform()
{
	// TODO:  在此添加控件通知处理程序代码

	m_iDirectionPlatform = IDC_RADIO_POSITIVE_PLATFORM;

	SetRadioButton();

}



void CMotionAdjust::OnBnClickedRadioNegativePlatform()
{
	// TODO:  在此添加控件通知处理程序代码

	m_iDirectionPlatform = IDC_RADIO_NEGATIVE_PLATFORM;

	SetRadioButton();
}



// ------------------------【相机运动按钮】--------------------------
void CMotionAdjust::OnBnClickedBtnMoveCamera()
{
	// TODO:  在此添加控件通知处理程序代码

	// /*
	int stepCamera;
	int directionCamera;
	int speedCamera;

	// 读取运动参数
	UpdateData(TRUE);

	// 脉冲数
	stepCamera = ((int)_ttof(m_sDistanceCamera))*m_steps_mm;

	speedCamera = ((int)_ttof(m_sSpeedCamera)) / 60 * m_steps_mm;

	// 判断正向反向
	if (m_iDirectionCamera == IDC_RADIO_POSITIVE_CAMERA)
		directionCamera = 1;
	else
		directionCamera = -1;

	stepCamera = stepCamera*directionCamera;

	set_maxspeed(1, speedCamera);
	set_conspeed(1, speedCamera);
	set_profile(1, speedCamera / 6, speedCamera, speedCamera*20);

	// con_pmove(1, stepCamera);   // 常速运动
	fast_pmove(1, stepCamera);     // 快速运动
	// */


}


// ------------------------【平台运动按钮】--------------------------
void CMotionAdjust::OnBnClickedBtnMovePlatform()
{
	// TODO:  在此添加控件通知处理程序代码


	int stepPlatform;
	int directionPlatform;
	int speedPlatform;

	// 读取运动参数
	UpdateData(TRUE);

	// 脉冲数
	stepPlatform = ((int)_ttof(m_sDistancePlatform))*m_steps_mm;

	speedPlatform = ((int)_ttof(m_sSpeedPlatform)) / 60 * m_steps_mm;

	// 判断正向反向
	if (m_iDirectionPlatform == IDC_RADIO_POSITIVE_PLATFORM)
		directionPlatform = 1;
	else
		directionPlatform= -1;

	stepPlatform = stepPlatform*directionPlatform;

	set_maxspeed(2, speedPlatform);
	set_conspeed(2, speedPlatform);
	set_profile(2, speedPlatform / 2, speedPlatform, speedPlatform / 2);

	con_pmove(2, stepPlatform);

}



// ----------------------------【急停】----------------------------
void CMotionAdjust::OnBnClickedBtnStop()
{
	// TODO:  在此添加控件通知处理程序代码

	// 这里需要判断哪个轴在运动，然后使那个轴停止
	if(check_done(1))
		sudden_stop(1);
	if(check_done(2))
		sudden_stop(2);
}


void CMotionAdjust::OnBnClickedBtnHomeCamera()
{
	int stepCamera;
	int speedCamera;
	int flag;
	// 读取运动参数
	UpdateData(TRUE);
	check_done(0);
	// 脉冲数
	stepCamera = ((int)_ttof(m_offset_camera))*m_steps_mm;
	speedCamera = ((int)_ttof(m_sSpeedCamera)) / 60 * m_steps_mm;
	set_home_mode(1, 0); //设置 1 轴回原点模式 0
	set_conspeed(1, speedCamera); //设置回零速度
	con_hmove(1, 1); //启动 1 轴正向回零运动
	Sleep(100);
	flag = 1;
	while (flag == 1)
	{
		flag = check_done(1);
		Sleep(10);
	}
	Sleep(100);
	con_pmove(1, stepCamera);
	Sleep(100);
	flag = 1;
	while (flag == 1)
	{
		flag = check_done(1);
		Sleep(10);
	}
	MessageBox(_T("相机回零完成!"));
	reset_pos(1);
}


void CMotionAdjust::OnBnClickedBtnHomePlatform()
{
	int stepPlatform;
	int speedPlatform;
	int flag;
	// 读取运动参数
	UpdateData(TRUE);
	check_done(0);
	// 脉冲数
	stepPlatform = ((int)_ttof(m_offset_platform))*m_steps_mm;
	speedPlatform = ((int)_ttof(m_sSpeedPlatform)) / 60 * m_steps_mm;
	set_home_mode(2, 0); //设置 2 轴回原点模式 0
	set_conspeed(2, speedPlatform); //设置回零速度
	con_hmove(2, -1); //启动 2 轴负向回零运动
	Sleep(100);
	flag = 1;
	while (flag==1)
	{
		flag=check_done(2);
		Sleep(10);
	}
	Sleep(100);
	con_pmove(2, stepPlatform);
	Sleep(100);
	flag = 1;
	while (flag == 1)
	{
		flag = check_done(2);
		Sleep(10);
	}
	MessageBox(_T("平台回零完成!"));
	reset_pos(2);
}
