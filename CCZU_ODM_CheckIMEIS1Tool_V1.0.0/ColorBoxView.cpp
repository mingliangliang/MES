// ColorBoxView.cpp : implementation of the CColorBoxView class
//
#include "stdafx.h"
#include "ColorBox.h"
#include "Splash.h"
#include "ColorBoxDoc.h"
#include "ColorBoxView.h"
#include <math.h>
#include "sp_brom.h" 
#include ".\Common\Common.h"

#include "PhaseCheckExport.h"
#include "mmsystem.h"//导入声音头文件

//#include "meta.h"
// #include "metaapp.h"
#pragma comment(lib,"winmm.lib")//导入声音头文件库

#define RFT_SUCCESS         0

#define IDLE                101
#define FAIL                102
#define PASS                103
#define Testing             104
#define InitFail            105

#define USB_VCOM             17
#define META_CONNECT_TIME_OUT  80000  //30000
#define  PID_ANDROID       "2000"
#define  VID_ANDROID       "0E8D"
#define  PID                      PID_ANDROID
#define  VID                      VID_ANDROID
#define  InterfaceClassGuidConstant  Guid_ANDROID_USB2Ser

//20150728 glory add
#ifndef PreloaderUSB
#define  PreloaderUSB  1
#endif
#ifndef BootROMUSB
#define  BootROMUSB   0
#endif
static const GUID GUID_PORT_CLASS_USB2SER ={0x4D36E978L,0xE325,0x11CE,{0xBF,0xC1,0x08,0x00,0x2B,0xE1,0x03,0x18}};

//end
//20150728 glory add global variament
META_Common_struct g_sMetaComm;
int  g_iMetaStopFlag;
#define  ERROR 0
#define  OK 1
char* ResultToString(int MR);
#define CWriteToolDlg CColorBoxView
//end

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CColorBoxView

IMPLEMENT_DYNCREATE(CColorBoxView, CFormView)

BEGIN_MESSAGE_MAP(CColorBoxView, CFormView)
	//{{AFX_MSG_MAP(CColorBoxView)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_COMPORT, OnSelchangeComboComport)
	ON_EN_CHANGE(IDC_EDIT_IMEI1, OnChangeEditImei1)
	ON_EN_CHANGE(IDC_EDIT_IMEI2, OnChangeEditImei2)
	ON_EN_CHANGE(IDC_EDIT_IMEI3, OnChangeEditImei3)
	ON_EN_CHANGE(IDC_EDIT_IMEI4, OnChangeEditImei4)
	ON_WM_CREATE()
	ON_COMMAND(IDM_MODE_1, OnMode1)
	ON_COMMAND(IDM_MODE_2, OnMode2)
	ON_COMMAND(IDM_MODE_3, OnMode3)
	ON_BN_CLICKED(IDC_BTN_SELECT, OnBtnSelect)
	ON_BN_CLICKED(IDC_BTN_SELECTAP, OnBtnSelectap)
	ON_COMMAND(IDM_SCAN_MAIN_IMEI_YES, OnScanMainImeiYes)
	ON_COMMAND(IDM_SCAN_MAIN_IMEI_NO, OnScanMainImeiNo)
	ON_BN_CLICKED(IDC_BTN_CLEAN, OnBtnClean)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATEDATA,OnUpdateData)
	ON_MESSAGE(WM_SETMYFOCUS,OnSetMyFocus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorBoxView construction/destruction

CColorBoxView::CColorBoxView()
	: CFormView(CColorBoxView::IDD)
{
	//{{AFX_DATA_INIT(CColorBoxView)
	m_csCurrentProduct = _T("");
	m_csIMEI1 = _T("");
	m_csIMEI2 = _T("");
	m_csIMEI3 = _T("");
	m_csIMEI4 = _T("");
	m_csAPDBPath = _T("");
	m_csDBPath = _T("");
	m_csCurrentColor = _T("");
	m_csCurrentOrder = _T("");
	m_iComboxComPort = -1;
	m_csCurrentLine = _T("");
	//}}AFX_DATA_INIT
	//20150730 GLORY add
	g_iMetaStopFlag = 0;
	m_pMetaStopFlag = &g_iMetaStopFlag;
	if(strlen(g_sMetaComm.sPortFilter.strBromFilter) <= 0)
		sprintf(g_sMetaComm.sPortFilter.strBromFilter,"%s","VID_0E8D&PID_0003");
	if(strlen(g_sMetaComm.sPortFilter.strPreloaderFilter) <= 0)
		sprintf(g_sMetaComm.sPortFilter.strPreloaderFilter, "%s", "VID_0E8D&PID_2000");
	if(strlen(g_sMetaComm.sPortFilter.strKernelFilter) <= 0)
		sprintf(g_sMetaComm.sPortFilter.strKernelFilter, "%s", "VID_0E8D&PID_2000");	
	//end
}

CColorBoxView::~CColorBoxView()
{
}

void CColorBoxView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorBoxView)
	DDX_Control(pDX, IDC_STATIC_RESULT, m_cResult);
	DDX_Control(pDX, IDC_STATIC_CURRENT_LINE, m_ctrCurrentLine);
	DDX_Control(pDX, IDC_BTN_CLEAN, m_ctrClean);
	DDX_Control(pDX, IDC_STATIC_CURRENT_COUNT, m_cCurrentCount);
	DDX_Control(pDX, IDC_STATIC_CURRENT_ORDER, m_ctrCurrentOrder);
	DDX_Control(pDX, IDC_STATIC_CURRENT_COLOR, m_ctrCurrentColor);
	DDX_Control(pDX, IDC_BTN_SELECT, m_ctrSelectDBButton);
	DDX_Control(pDX, IDC_BTN_SELECTAP, m_ctrSelectAPDBButton);
	DDX_Control(pDX, IDC_COMBO_COMPORT, m_ctrComboxComPort);
	DDX_Control(pDX, IDC_EDIT_IMEI4, m_ctrIMEI4);
	DDX_Control(pDX, IDC_EDIT_IMEI3, m_ctrIMEI3);
	DDX_Control(pDX, IDC_EDIT_IMEI2, m_ctrIMEI2);
	DDX_Control(pDX, IDC_EDIT_IMEI1, m_ctrIMEI1);
	DDX_Control(pDX, IDC_LIST_STATE, m_state_list);
	DDX_Control(pDX, IDC_STATIC_CURRENT_PRODUCT, m_ctrCurrentProduct);
	DDX_Text(pDX, IDC_STATIC_CURRENT_PRODUCT, m_csCurrentProduct);
	DDX_Text(pDX, IDC_EDIT_IMEI1, m_csIMEI1);
	DDX_Text(pDX, IDC_EDIT_IMEI2, m_csIMEI2);
	DDX_Text(pDX, IDC_EDIT_IMEI3, m_csIMEI3);
	DDX_Text(pDX, IDC_EDIT_IMEI4, m_csIMEI4);
	DDX_Text(pDX, IDC_EDIT_APDBPATH, m_csAPDBPath);
	DDX_Text(pDX, IDC_EDIT_DBPATH, m_csDBPath);
	DDX_Text(pDX, IDC_STATIC_CURRENT_COLOR, m_csCurrentColor);
	DDX_Text(pDX, IDC_STATIC_CURRENT_ORDER, m_csCurrentOrder);
	DDX_CBIndex(pDX, IDC_COMBO_COMPORT, m_iComboxComPort);
	DDX_Text(pDX, IDC_STATIC_CURRENT_LINE, m_csCurrentLine);
	//}}AFX_DATA_MAP
}

//初始化FIXTURE PHONE NV
void InitFPNV(LPVOID x) 
{
	CColorBoxView *Sv = (CColorBoxView *)x;
 
	if(!Sv->DoInitFPNV())
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
		Sv->InsertListInfo("Init Modem NV Fail...",2);
		Sv->ShowCalResult(InitFail);
		return;
	}

	Sv->InsertListInfo("Init Modem NV Pass...",0);
	Sv->ShowCalResult(IDLE);
}

//初始化SMART PHONE NV
void InitSPNV(LPVOID x) 
{
	CColorBoxView *Sv = (CColorBoxView *)x;
	if(!Sv->DoInitSPModemNV())
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
		Sv->InsertListInfo("Init Modem NV Fail...",2);
		Sv->ShowCalResult(InitFail);
		return;
	}
	else
	{
		Sv->InsertListInfo("Init Modem NV Pass...",0);
	}
	
	
	if(!Sv->DoInitSPApNV())
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
		Sv->InsertListInfo("Init AP NV Fail...",2);
		Sv->ShowCalResult(InitFail);
		return;
	}

	Sv->InsertListInfo("Init AP NV Pass...",0);
	Sv->ShowCalResult(IDLE);
}


void SmartStartReadIMEI_Ex_New(CColorBoxView *Sv) 
{
	CString sInfo;
	int iRet = 0;
	bool bAnyOperationFail = false;

	Sv->SetupMetaModeParameters();

	META_RESULT MetaResult = META_SUCCESS;
	//EnterAPMetaMode
	if(Sv->EnterAPMetaMode() != SPMETA_DLL::META_SUCCESS)
	{
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		Sv->ShowCalResult(FAIL);
		Sv->Display("Enter ap Meta Modem Mode Fail",1);
		Sv->SP_Exit_MetaModemMode_NEW();
		return;
	}
	//SPMETA_DLL::SP_META_QueryIfFunctionSupportedByTarget_r
	if(SPMETA_DLL::META_SUCCESS == SPMETA_DLL::SP_META_QueryIfFunctionSupportedByTarget_r(Sv->m_ApExe.iMetaAP_handle, 500, "SP_META_ENCRYPTED_Support_r"))
	{
	//QueryEncryptionSupport
		if(Sv->QueryEncryptionSupport(&(Sv->m_ApExe.iMetaAP_handle)));
	}
	Sv->Display("GetSPModemInfo_Ex(): Query modem information start...");
	//GetSPModemInfo_Ex
	iRet = Sv->GetSPModemInfo_Ex(); 
	if (iRet == 0)
	{
		Sv->Display("GetSPModemInfo_Ex(): Query modem information successfully!!");
	} 
	else
	{
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		Sv->ShowCalResult(FAIL);
		bAnyOperationFail = true;
		sInfo.Format("GetSPModemInfo_Ex(): Query modem information fail, MetaResult = %s", ResultToString_SP(iRet));
		Sv->Display(sInfo);
		Sv->SP_Exit_MetaModemMode_NEW();
		return;
	}
	
	META_RESULT eMeta_Result = Sv->EnterModemMetaToInitModemDB();
	if(META_SUCCESS == eMeta_Result)
	{
		Sv->Display("EnterModemMetaToInitModemDB(): Query modem information successfully!!");
	}
	else
	{    
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		Sv->ShowCalResult(FAIL);
		bAnyOperationFail = true;
		sInfo.Format("EnterModemMetaToInitModemDB() : MetaResult = %s", ResultToString(eMeta_Result));
		Sv->Display(sInfo.GetBuffer(sInfo.GetLength()));
		Sv->SP_Exit_MetaModemMode_NEW();
		return;
	}

	CString temp;
	if(!Sv->FP_Read_PCB_BARCODE())
	{
		//警告提示音
 		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
 		Sv->InsertListInfo("Read Module SN fail...",2);
 		Sv->ShowCalResult(FAIL);
 		Sv->SP_Exit_MetaModemMode_NEW();
		Sv->CleanImeiWindow();
 		Sv->ClearUI();
 		return;
	}
	else
	{
		temp.Format("Read Module SN = %s",Sv->m_csPcbaSN);
		Sv->InsertListInfo((LPSTR)(LPCTSTR)temp,0);
	}	

	if(!Sv->SP_CHECK_NV())
	{
		//警告提示音
 		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		Sv->InsertListInfo("Check NVRAM Fail...",2);
		Sv->ShowCalResult(FAIL);
		Sv->SP_Exit_MetaModemMode_NEW();
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		return;
	}
	else
	{
		Sv->InsertListInfo("Check NVRAM Pass...",0);
	}

	//恢复出厂设置标志位
 	if(Sv->m_iResetFactory)
 	{
 		if(!Sv->SP_WriteResetFactory2Barcode())
 		{
 			//警告提示音
 			PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
 			Sv->InsertListInfo("barcode Write restore factory setting flag bit failure...",2);
 			Sv->ShowCalResult(FAIL);
 			Sv->SP_Exit_MetaModemMode_NEW();
 			Sv->CleanImeiWindow();
 			Sv->ClearUI();
 			return;
 		}
 		else
 		{
 			Sv->InsertListInfo("barcode Write restore factory setting flag bit Pass...",0);
 		}
 	}
    
	if((Sv->m_iCheckVersion) || (Sv->m_iResetFactory))
	{
		//切换到AP模式下
		META_RESULT eMeta_Result = Sv->ModemSwithToAPMeta();
		if(META_SUCCESS != eMeta_Result)
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
			Sv->InsertListInfo("Quit Modem Meta Fail...",2);
			Sv->ShowCalResult(FAIL);
			Sv->SP_Exit_MetaModemMode_NEW();
			Sv->CleanImeiWindow();
			Sv->ClearUI();
			return;
		}

		//lucky 20151215读取检测外部版本号
		if(Sv->m_iCheckVersion)
 		{
			CString get_swver;
			CString temp_ver;
			get_swver = Sv->m_ApExe.SP_REQ_GetVersion_2();
			get_swver.Replace("\n", "");

 			if(get_swver.GetLength()<3)
 			{
 				//警告提示音
 				PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
 				Sv->InsertListInfo("Read version FAIL...",2);
 				Sv->ShowCalResult(FAIL);
 				Sv->SP_Exit_MetaModemMode_NEW();
 				Sv->CleanImeiWindow();
 				Sv->ClearUI();
 				return;
 			}
			else
 			{
 				Sv->InsertListInfo("Read version Pass...",0);
 			}

 			if(get_swver.Compare(Sv->m_csVersion) != 0)
 			{

         		//警告提示音
  				PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
 				temp_ver.Format("Comparison version number failed... %s, %s", get_swver, Sv->m_csVersion);
 				Sv->InsertListInfo((LPSTR)(LPCTSTR)temp_ver,2);
				temp_ver.Format("%s, %s", get_swver, Sv->m_csVersion);
				Sv->InsertListInfo((LPSTR)(LPCTSTR)temp_ver,2);  
 				Sv->ShowCalResult(FAIL);
 				Sv->SP_Exit_MetaModemMode_NEW();
 				Sv->CleanImeiWindow();
 				Sv->ClearUI();
 				return;
 			}
 			else
 			{
 				Sv->InsertListInfo("Comparison version number success...",0);
 			}
 		}
		//end
	
		//恢复出厂设置标志位
 		if(Sv->m_iResetFactory)
		{
 			//读取AP端SN号
 			Sv->m_csAPPcbaSN = Sv->m_ApExe.ApReadBarcode(63);

 			Sv->m_csAPPcbaSN.SetAt(21,'1');

 			//AP端barcode写入恢复出厂设置标志位
 			if(!Sv->m_ApExe.ApWriteBarcode(Sv->m_csAPPcbaSN))
			{
 				PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
 				Sv->InsertListInfo("AP write barcode flag fail...",2);
 				Sv->ShowCalResult(FAIL);
 				Sv->SP_Exit_MetaModemMode_NEW();
 				Sv->CleanImeiWindow();
 				Sv->ClearUI();
 				return;
 			}
 			else
 			{
 				Sv->InsertListInfo("AP write barcode flag pass......",0);
 			}
 		}
		//从AP模式切换到MODEM模式读取IMEI号
		MetaResult = Sv->APSwithToModemMeta_Ex();			
		if (MetaResult != META_SUCCESS)
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
			Sv->InsertListInfo("Converted to Meta Modem failed...",2);
			Sv->ShowCalResult(FAIL);
			Sv->SP_Exit_MetaModemMode_NEW();
			Sv->CleanImeiWindow();
			Sv->ClearUI();
			return;
		}		
		Sv->InsertListInfo("Converted to Meta Modem Pass...",0);
	}
	
	//读取手机IMEI号
	if(!Sv->FP_Read_IMEI_Ex())
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC); 
		Sv->ShowCalResult(FAIL);
		Sv->SP_Exit_MetaModemMode_NEW();
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		return;
	}
	
	//退出AP和MODEM模式
	Sv->SP_Exit_MetaModemMode_NEW();
	
	//比对扫描IMEI和读取IMEI是否一致
	if(!Sv->Check_Read_and_Scan_IMEI())
	{
		//警告提示音
 		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC); 
		Sv->InsertListInfo("IMEI Inconsistent...",2);
		Sv->ShowCalResult(FAIL);
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		return;
	}
	else
	{
		Sv->InsertListInfo("IMEI consistent...",0);
	}
#ifndef NOSQL		
	//防呆判断
	if(!Sv->Read_Mode_Check_Database())//如果返回FALSE,则表明IMEI数据库没有数据或者COLOR中有数据已经打印完成，
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC); 
		Sv->ShowCalResult(FAIL);
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		return;
	}
	else//如果返回TRUE则表示在IMEI数据库中找到数据了，直接打印
	{
		Sv->PrintLabel();
		Sv->CleanImeiWindow();
		Sv->ClearUI();
	}

	if(!Sv->Load_Current_Product_Count())
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		Sv->InsertListInfo("To obtain the number of current orders has failed",2);
		Sv->ShowCalResult(FAIL);
		return;
	}
	//警告提示音
	PlaySound((LPCSTR)(LPCTSTR)Sv->m_csOkSndPath, NULL, SND_FILENAME | SND_ASYNC);
	Sv->InsertListInfo("Obtain the number of current orders has been successful",0);
	Sv->UpdateCount(Sv->m_csCurrentCount);
#endif
	Sv->ShowCalResult(PASS);
	return;
}

void SmartStartReadIMEI_Ex(LPVOID x) 
{	
	CColorBoxView *Sv = (CColorBoxView *)x;
	Sv->ShowCalResult(Testing);	
	
	//检测端口是否选择
	if(Sv->m_ctrComboxComPort.GetCurSel()==-1)
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
		Sv->InsertListInfo("Serial port setting failure...",2);
		Sv->ShowCalResult(FAIL);
		return;
	}
	
	//进MODEM_META
	CString text;
	Sv->m_ctrComboxComPort.GetLBText(Sv->m_ctrComboxComPort.GetCurSel(),text);	
	if(text=="USB")		
	{		
		//使用新函数处理USB模式下的操作
		SmartStartReadIMEI_Ex_New(Sv);
		return;
		//end
	}
}


void SPRDStart(LPVOID x)
{
	CColorBoxView *Sv = (CColorBoxView *)x;
	Sv->ShowCalResult(Testing);
	
	//检测端口是否选择
	if(Sv->m_ctrComboxComPort.GetCurSel()==-1)
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
		Sv->InsertListInfo("ComPort Setting Fail...",2);
		Sv->ShowCalResult(FAIL);
		return;
	}
	
	//展讯模式
	Sv->InsertListInfo("Begin To Enter SPRD Mode...",0);
	
	if (!Sv->EnterSPRD())
	{
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		Sv->InsertListInfo("Enter SPRD Mode Fail...",2);
		Sv->ShowCalResult(FAIL);
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		return;
	}
	else
	{
		Sv->InsertListInfo("Enter SPRD Mode Pass...",0);
	}

	//比对版本号 ok
 	if(Sv->m_iCheckVersion)
	{
		if(!Sv->CheckApVer_SPRD(Sv->m_csVersion))
		{
			PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
			Sv->InsertListInfo("Check AP Version Fail...",2);
			SP_StopModeProcess(Sv->g_hDiagPhone);
			SP_EndPhoneTest(Sv->g_hDiagPhone);
			Sv->ShowCalResult(FAIL);
			Sv->CleanImeiWindow();
			Sv->ClearUI();
			return;
		}
		else
		{
			Sv->InsertListInfo("Check AP Version Pass...",0);
		}
	}

	//读取SN
	CString temp;
	if(!Sv->FP_Read_PCB_BARCODE())
	{
		//警告提示音
 		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
 		Sv->InsertListInfo("Get Module SN Fail...",2);
 		SP_StopModeProcess(Sv->g_hDiagPhone);
 		SP_EndPhoneTest(Sv->g_hDiagPhone);
 		Sv->ShowCalResult(FAIL);
 		Sv->CleanImeiWindow();
 		Sv->ClearUI();
 		return;
	}

	//读取Barcode
	if(!Sv->ReadBarCode_SPRD(Sv->m_csPcbaBarcode))
	{
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		Sv->InsertListInfo("Read Barcode Fail...",2);
		SP_StopModeProcess(Sv->g_hDiagPhone);
		SP_EndPhoneTest(Sv->g_hDiagPhone);
		Sv->ShowCalResult(FAIL);
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		return;
	}
	else
	{
		Sv->InsertListInfo("Read Barcode Pass...",0);
		Sv->InsertListInfo((LPSTR)(LPCTSTR)Sv->m_csPcbaBarcode,0);
	}

	//校准终测结果 ok
	int nErrorCode = RFT_SUCCESS;
	char m_szCurrentStation[]="IMEI";
	char m_szCheckStation[]="CFT";
	if(Sv->m_iBTFT)
	{		
		SPPH_OBJECT m_hPhaseCheckObj = CreatePhaseCheckObject(Sv->g_hDiagPhone);
		if(m_hPhaseCheckObj!=NULL)
        {    
            // PutProperty(m_hPhaseCheckObj, SPPH_VERSION, SPPH_SP09);
             nErrorCode =CheckPhase(m_hPhaseCheckObj,m_szCurrentStation,m_szCheckStation);
        }
        else
        {
        	PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
			Sv->InsertListInfo("Read CFT Fail...",2);
			SP_StopModeProcess(Sv->g_hDiagPhone);
			SP_EndPhoneTest(Sv->g_hDiagPhone);
			Sv->ShowCalResult(FAIL);
			Sv->CleanImeiWindow();
			Sv->ClearUI();
			return;
        }
		if(nErrorCode != RFT_SUCCESS)
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC); 
			Sv->InsertListInfo("Read CFT Fail...",2);
			SP_StopModeProcess(Sv->g_hDiagPhone);
			SP_EndPhoneTest(Sv->g_hDiagPhone);
			Sv->ShowCalResult(FAIL);
			Sv->CleanImeiWindow();
			Sv->ClearUI();
			return;
		}
		Sv->InsertListInfo("Check CFT Pass...",0);   
	}
	
	//恢复出厂设置标志位
 	if(Sv->m_iResetFactory)
 	{
 		if(!Sv->WriteFactoryResetFlag_SPRD())
 		{
 			//警告提示音
 			PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC); 
 			Sv->InsertListInfo("Write Factory Reset Flag Fail...",2);
 			SP_StopModeProcess(Sv->g_hDiagPhone);
 			SP_EndPhoneTest(Sv->g_hDiagPhone);
 			Sv->ShowCalResult(FAIL);
 			Sv->CleanImeiWindow();
 			Sv->ClearUI();
 			return;
 		}
 		Sv->InsertListInfo("Write Factory Reset Flag Pass...",0);
 	}

	//读取手机IMEI号
	if(!Sv->FP_Read_IMEI_Ex())
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC); 
		SP_StopModeProcess(Sv->g_hDiagPhone);
		SP_EndPhoneTest(Sv->g_hDiagPhone);
		Sv->InsertListInfo("Read IMEI Fail...",2);
		Sv->ShowCalResult(FAIL);
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		return;
	}
	
	//比对扫描IMEI和读取IMEI是否一致 ok
	if(!Sv->Check_Read_and_Scan_IMEI())
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC); 
		SP_StopModeProcess(Sv->g_hDiagPhone);
		SP_EndPhoneTest(Sv->g_hDiagPhone);
		Sv->InsertListInfo("Compare IMEI Fail...",2);
		Sv->ShowCalResult(FAIL);
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		return;
	}
	else
	{
		Sv->InsertListInfo("Compare IMEI Pass...",0);
	}

	SP_StopModeProcess(Sv->g_hDiagPhone);
	SP_EndPhoneTest(Sv->g_hDiagPhone);
	
	//防呆判断
#ifndef _DEBUG
	if(!Sv->Read_Mode_Check_Database())//如果返回FALSE,则表明IMEI数据库没有数据或者COLOR中有数据已经打印完成，
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC); 
		Sv->ShowCalResult(FAIL);
		Sv->CleanImeiWindow();
		Sv->ClearUI();
		return;
	}
	else//如果返回TRUE则表示在IMEI数据库中找到数据了，直接打印
	{
		Sv->PrintLabel();
		Sv->CleanImeiWindow();
		Sv->ClearUI();
	}
	
	if(!Sv->Load_Current_Product_Count())
	{
		//警告提示音
		PlaySound((LPCSTR)(LPCTSTR)Sv->m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
		Sv->InsertListInfo("Load_Current_Product_Count Fail...",2);
		Sv->ShowCalResult(FAIL);
		return;
	}
	//警告提示音
	PlaySound((LPCSTR)(LPCTSTR)Sv->m_csOkSndPath, NULL, SND_FILENAME | SND_ASYNC);
	Sv->InsertListInfo("Load_Current_Product_Count Pass...",0);
	Sv->UpdateCount(Sv->m_csCurrentCount);
	Sv->ShowCalResult(PASS);
#endif
#ifdef _DEBUG
	Sv->CleanImeiWindow();
	Sv->ClearUI();
#endif
}


bool CColorBoxView::EnterSPRD()
{
	CHANNEL_ATTRIBUTE ca;
	ca.ChannelType = CHANNEL_TYPE_USBMON;
	
	if (!_SPOK(SP_BeginPhoneTest(g_hDiagPhone, &ca)))
    {
        return FALSE;
    }
	
	if (NULL == m_hEnterModeSuccess)
	{
		m_hEnterModeSuccess = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	ResetEvent(m_hEnterModeSuccess);
	
	SP_EnterModeProcess(g_hDiagPhone, true, -1, RM_CALIBRATION_MODE, m_hEnterModeSuccess, 0);
	
	if (WAIT_OBJECT_0 != WaitForSingleObject(m_hEnterModeSuccess,30000))
	{
		SP_StopModeProcess(g_hDiagPhone);
		SP_EndPhoneTest(g_hDiagPhone);
		ResetEvent(m_hEnterModeSuccess);
		return FALSE;
	}
	else
	{
		ResetEvent(m_hEnterModeSuccess);
	}
	
	return TRUE;
}

bool CColorBoxView::CheckApVer_SPRD(CString sAPVersionToCompare)
{
	CString sAPVersion = "";
	char szRecvBuf[4096] = {0};
	CString temp_ver; 
    //int nOperCode = SP_GetAPVersion(g_hDiagPhone,szRecvBuf,sizeof(szRecvBuf));//获取内部版本号lucky
    //int nOperCode = SP_GetMsVersion(g_hDiagPhone,szRecvBuf,sizeof(szRecvBuf)); 
    int nOperCode = SP_GetExVersion(g_hDiagPhone,szRecvBuf,sizeof(szRecvBuf));	//获取外部版本号lucky
	sAPVersion = szRecvBuf;
	if(nOperCode != 0)
	{
		this->InsertListInfo("SPRD Read AP Version Fail...",2);
		return false;
	}
	else
	{
		sAPVersionToCompare.MakeLower();
		sAPVersion.MakeLower();
		
		if(sAPVersionToCompare != sAPVersion)
		{
			this->InsertListInfo("Compare Version Fail...",2);
			temp_ver.Format("%s, %s", sAPVersion, sAPVersionToCompare);
			this->InsertListInfo((LPSTR)(LPCTSTR)temp_ver,2);
		}
		return (sAPVersionToCompare == sAPVersion);
	}	
}

#define BARCODE_LEN  63
bool ConvertBarCode(char* intBuf, char* charbuf)
{
	if(!intBuf || !charbuf)  return false;
	try
	{
		for(int i = 0; i < BARCODE_LEN; i++)
		{
			if(i < 17)
				charbuf[i] = ' ';
			else
			{

				if(intBuf[i*4]>=48 && intBuf[i*4]<=57)    
				{    
					charbuf[i] = intBuf[i*4];
				}
				else
				{
					if(intBuf[i*4]>=65 && intBuf[i*4]<=90)
					{
						charbuf[i] = intBuf[i*4];
					}
					else
					{
						if(intBuf[i*4]>=97 && intBuf[i*4]<=122)
						{
							charbuf[i] = intBuf[i*4];
						}
						else
						{
							charbuf[i] = ' ';
						}
					}
				}
			}
		}
		return true;
	}
	catch(...)
	{
		return false;
	}
}



bool CColorBoxView::ReadBarCode_SPRD(CString& sBarcode)
{
    char szBuf[BARCODE_LEN*4] = {0};
	char szReadBuf[BARCODE_LEN+1] = {0};
	int nErrroCode = SP_ReadTraceInfo(g_hDiagPhone, (void *)szBuf, BARCODE_LEN * 4);
	if(RFT_SUCCESS != nErrroCode)
	{
		sBarcode = "";
		return false;
	}	
	else
	{
		if(!ConvertBarCode(szBuf, szReadBuf))
		{
			return false;
		}
		else
			sBarcode = szReadBuf;
	}
	return (RFT_SUCCESS == nErrroCode);
}

bool CColorBoxView::WriteFactoryResetFlag_SPRD()
{
	char szBuf[BARCODE_LEN*4] = {0};
	int nErrroCode = SP_ReadTraceInfo(g_hDiagPhone, (void *)szBuf, BARCODE_LEN * 4);
	if(RFT_SUCCESS != nErrroCode)
	{
		return false;
	}	
	else
	{
		szBuf[84] = '1';   //21*4   reset
		nErrroCode = SP_WriteTraceInfo(g_hDiagPhone, (void *)szBuf, BARCODE_LEN * 4);
		if(RFT_SUCCESS != nErrroCode)
		{
			return false;
		}	
	}
	return (RFT_SUCCESS == nErrroCode);
}


BOOL CColorBoxView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}


void CColorBoxView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	m_hEnterModeSuccess = CreateEvent(NULL, TRUE, FALSE, NULL);

	this->SetMyFont();

	this->m_Config.InitConfigPath();

	this->m_Config.ReadConfig();

	this->EnableMobileCtrl(FALSE);

	this->LoadComPort();
	
	m_csCurrentOrder = ((CColorBoxApp *)AfxGetApp())->InputOrder;
	m_csCurrentLine  = ((CColorBoxApp *)AfxGetApp())->InputProductionLine;

	this->SetTimer(1,500,0);
}

/////////////////////////////////////////////////////////////////////////////
// CColorBoxView diagnostics

#ifdef _DEBUG
void CColorBoxView::AssertValid() const
{
	CFormView::AssertValid();
}

void CColorBoxView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CColorBoxDoc* CColorBoxView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CColorBoxDoc)));
	return (CColorBoxDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CColorBoxView message handlers
void CColorBoxView::SetMyFont()
{
	//===============set imagelist=====================================
	
	HICON hIcon[3];
	hIcon[0]=AfxGetApp()->LoadIcon(IDI_PASS);
	hIcon[1]=AfxGetApp()->LoadIcon(IDI_ERRORS);
	hIcon[2]=AfxGetApp()->LoadIcon(IDI_STOP);
	if(m_imageList.m_hImageList!=NULL)
		m_imageList.DeleteImageList();
	m_imageList.Create(16,16,ILC_COLOR32,3,3);
	m_imageList.SetBkColor(RGB(255,255,255));
	m_imageList.Add(hIcon[0]);
	m_imageList.Add(hIcon[1]);
	m_imageList.Add(hIcon[2]);	

	//==========init state list=====================================
    DWORD dwStyle;
	LVCOLUMN   Column;	
	
	m_state_list.SetImageList(&m_imageList,LVSIL_SMALL);
	dwStyle = m_state_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
	//dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
	//dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
	m_state_list.SetExtendedStyle(dwStyle); //设置扩展风格

	//((CWnd*)GetDlgItem(IDC_LIST_STATE))->SetFont(&m_sysfont);
	Column.mask =   LVCF_TEXT|LVCF_FMT|LVCF_WIDTH;
	Column.cchTextMax =   30;   
	Column.cx =   170;     //column宽度，不工作！   
	Column.fmt =   LVCFMT_LEFT;     //不工作   
	Column.iImage =   0;     
	Column.iOrder =   0;     //不明白干什么用   
	Column.iSubItem =   8;        
	Column.pszText =   "Time";   
	m_state_list.InsertColumn(0,&Column);
	Column.pszText =   "Status"; 
	Column.cx =   280;
	m_state_list.InsertColumn(1,&Column);

	this->m_ctrCurrentProduct.SetFontSize(18);
	this->m_ctrCurrentProduct.SetFontName("黑体");
	this->m_ctrCurrentProduct.SetTextColor(RGB(0,0,255));

	this->m_ctrCurrentOrder.SetFontSize(18);
	this->m_ctrCurrentOrder.SetFontName("粗体");
	this->m_ctrCurrentOrder.SetTextColor(RGB(0,0,255));

	this->m_ctrCurrentColor.SetFontSize(18);
	this->m_ctrCurrentColor.SetFontName("粗体");
	this->m_ctrCurrentColor.SetTextColor(RGB(0,0,255));

	this->m_ctrCurrentLine.SetFontSize(18);
	this->m_ctrCurrentLine.SetFontName("粗体");
	this->m_ctrCurrentLine.SetTextColor(RGB(0,0,255));

	this->m_cCurrentCount.SetFontSize(30);
	this->m_cCurrentCount.SetFontName("黑体");
	this->m_cCurrentCount.SetTextColor(RGB(0,0,255));
	
	VERIFY(m_staticfont.CreateFont(
		36,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"黑体"));                 // lpszFacename

	((CWnd*)GetDlgItem(IDC_STATIC_IMEI1))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI2))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI3))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_STATIC_IMEI4))->SetFont(&m_staticfont);

	((CWnd*)GetDlgItem(IDC_EDIT_IMEI1))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_IMEI2))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_IMEI3))->SetFont(&m_staticfont);
	((CWnd*)GetDlgItem(IDC_EDIT_IMEI4))->SetFont(&m_staticfont);

	VERIFY(m_staticfont3.CreateFont(
		60,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"宋体")); 

	VERIFY(m_staticfont4.CreateFont(
		40,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"宋体")); 

	this->m_ctrClean.SetFont(&m_staticfont4);

	this->m_cResult.SetFontSize(50);
	this->m_cResult.SetFontName("黑体");
}

void CColorBoxView::OnTimer(UINT nIDEvent) 
{
	CString temp;
	CString csCFGPath;
	HINSTANCE hInstance;
	char direc[MAX_PATH];
	CFileFind findfile;
	int iRet;
	BOOL bIsFinded;
	CString filename;

	CString path;
	GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	m_csErrorSndPath = path + _T("\\MES_ERROR.wav");
	m_csOkSndPath = path + _T("\\MES_OK.wav");

	if(nIDEvent == 1)
	{
		this->KillTimer(1);
		CFormView::OnTimer(nIDEvent);

		AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_MODE_3, MF_BYCOMMAND | MF_CHECKED );

		switch (this->m_Config.m_iIsScanMainImei)
		{
		case 0:
			AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_NO, MF_BYCOMMAND | MF_CHECKED );
			break;
		case 1:
			AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_YES, MF_BYCOMMAND | MF_CHECKED );
			break;
		default:
			this->m_Config.m_iIsScanMainImei=0;
			this->m_Config.SaveConfig();
			AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_NO, MF_BYCOMMAND | MF_CHECKED );
			break;
		}
		
		EnableMobileCtrl(FALSE);

		this->SetUIDisableAll();

		InsertListInfo("Operate Mode：Scan+Read Mode...",0);
#ifndef NOSQL		
		if(!this->GetProductOrderInfo())
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
			InsertListInfo("Get Product Order Information Fail...",2);
			return;
		} 
		else
		{
			UpdateData(FALSE);
			InsertListInfo("Get Product Order Information Pass...",0);
		}
#endif
#ifndef NOSQL
		//获取工具设置
		if(!this->GetSetting())
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
			InsertListInfo("Get Product Setting Fail...",2);
			return;
		}
		else
		{
			InsertListInfo("Get Product Setting Pass...",0);
		}

		if(!this->Load_Current_Product_Count())
		{
			//警告提示音
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);
			InsertListInfo("Get the Order Number Producted Fail",2);
			return;
		}
		InsertListInfo("Get the Order Number Producted Pass",0);
		this->UpdateCount(this->m_csCurrentCount);
#endif
#ifdef NOSQL
		m_iPlatform = 1;
		m_iIMEIType = 0;
		m_iSIMLock2 = false;
		m_iSIMUnLock2 = false;
		m_iCheckVersion = false;
		m_iCheckCopyFile = false;
		m_iCheckPsCalibration = false;
#endif
#ifndef NOSQL
		if(m_iPlatform==0 || m_iPlatform==1)
		{
			//建立远程共享连接
			csCFGPath = this->m_Config.m_csCFGPath.Left(this->m_Config.m_csCFGPath.GetLength()-1);
			
			if (csCFGPath.Left(2) == "\\\\")
			{
				temp.Format("/c net use %s %s /user:%s", csCFGPath, this->m_Config.m_csLoginPSW, this->m_Config.m_csLoginName);
				hInstance = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(),"open","cmd.exe",temp,NULL,SW_HIDE);
				iRet = int(hInstance);
				if (iRet < 32)
				{
					MessageBox("Establish a Remote Sharing Connections Fail!","Error",MB_OK);
					exit(0);
				}
			}
			
			//获取服务器DataBase文件存放路径
			csCFGPath = this->m_Config.m_csCFGPath+m_csCurrentProduct+"_"+m_csCurrentOrder+"\\DataBase";
			
			//获取当前文件路径
			GetCurrentDirectory(MAX_PATH,direc);
			
			//如果本地Database文件夹有文件则删除
			bIsFinded = findfile.FindFile(CString(direc)+"\\DataBase\\*");
			while( bIsFinded )
			{
				bIsFinded = (BOOL)findfile.FindNextFile();
				if( !findfile.IsDots() && !findfile.IsDirectory()) 
				{
					filename = findfile.GetFileName();
					DeleteFile(CString(direc)+"\\DataBase\\"+filename);
				}
			}
			findfile.Close();
			
			//从服务器copy Database文件至本地
			temp.Format("cmd /c xcopy %s %s /e /i /y", csCFGPath, CString(direc)+"\\DataBase");
			
			//清空结构
			STARTUPINFO sInfo;
			PROCESS_INFORMATION pInfo;
			ZeroMemory(&sInfo,sizeof(sInfo));
			sInfo.cb=sizeof(sInfo);
			sInfo.dwFlags=STARTF_USESHOWWINDOW;
			sInfo.wShowWindow=SW_HIDE;
			ZeroMemory(&pInfo,sizeof(pInfo));
			this->Display(temp);
			if(!::CreateProcess(NULL,temp.GetBuffer(0),
				NULL,
				NULL,
				FALSE,
				0,
				NULL,
				NULL,
				&sInfo,
				&pInfo))
			{
				MessageBox("Copy File Version Fail!","Error",MB_OK);
				exit(0);
			}
			//等进程关闭
			WaitForSingleObject(pInfo.hProcess,INFINITE);
			//关闭进程和线程的句柄
			CloseHandle(pInfo.hProcess);
			CloseHandle(pInfo.hThread);
			
			//获取本地Database文件夹下文件名
			bIsFinded = findfile.FindFile(CString(direc)+"\\DataBase\\*");
			
			while( bIsFinded )
			{
				bIsFinded = (BOOL)findfile.FindNextFile();
				
				if(!findfile.IsDots() && !findfile.IsDirectory())
				{
					filename = findfile.GetFileName();
					
					if (filename.Left(3) == "BPL")
					{
						this->m_csDBPath = CString(direc)+"\\DataBase\\"+filename;
					}
					
					if (filename.Left(4) == "APDB")
					{
						this->m_csAPDBPath = CString(direc)+"\\DataBase\\"+filename;
					}
					UpdateData(FALSE);
				}
			}
			findfile.Close();
		}
#endif
#ifdef NOSQL
#ifndef SPRD
		this->m_iPlatform = 1;	
#else
		this->m_iPlatform = 2;
#endif
		this->m_csDBPath = "E:\\文档资料\\MTK DB文件\\C3588_DW_C102\\BPLGUInfoCustomAppSrcP_MT6735_S00_MOLY_LR9_W1444_MD_LWTG_MP_V16_P6_1_lwg_n";
		this->m_csAPDBPath = "E:\\文档资料\\MTK DB文件\\C3588_DW_C102\\APDB_MT6735_S01_L1.MP3_W15.23";
#endif	
		UpdateData(FALSE);

		if(this->m_iPlatform==0)
		{
			this->m_ctrSelectDBButton.EnableWindow(TRUE);
			this->m_ctrSelectAPDBButton.EnableWindow(FALSE);
		}
		else if(this->m_iPlatform==1)
		{
			this->m_ctrSelectDBButton.EnableWindow(TRUE);
			this->m_ctrSelectAPDBButton.EnableWindow(TRUE);
		}
		else if(this->m_iPlatform==2)
		{
			this->m_ctrSelectDBButton.EnableWindow(FALSE);
			this->m_ctrSelectAPDBButton.EnableWindow(FALSE);
		}
		//初始化NV
		if(this->m_iPlatform==0)
		{
			m_hThreadModemNVInit =
				CreateThread (NULL,  
				0,     
				(LPTHREAD_START_ROUTINE)InitFPNV,
				(void *)this,  
				0,     
				&m_dThreadIdModemNVInit);
		}
		else if(this->m_iPlatform==1)
		{
			m_hThreadSPNVInit =
				CreateThread (NULL,
				0,
				(LPTHREAD_START_ROUTINE)InitSPNV,
				(void *)this,  
				0,     
				&m_dThreadIdSPNVInit);
		}
		else if(this->m_iPlatform==2)
		{
			g_hDiagPhone = SP_CreatePhone(NULL);
			if (INVALID_PHONE_HANDLE == g_hDiagPhone)
			{
				return;
			}
			ShowCalResult(IDLE);
		}

		//根据IMEIType情况设置空间
		switch(this->m_iIMEIType)
		{
		case 0:
			this->m_ctrIMEI1.EnableWindow(TRUE);
			this->m_ctrIMEI2.EnableWindow(FALSE);
			this->m_ctrIMEI3.EnableWindow(FALSE);
			this->m_ctrIMEI4.EnableWindow(FALSE);
			break;
		case 1:
			this->m_ctrIMEI1.EnableWindow(TRUE);
			this->m_ctrIMEI2.EnableWindow(TRUE);
			this->m_ctrIMEI3.EnableWindow(FALSE);
			this->m_ctrIMEI4.EnableWindow(FALSE);
			break;
		case 2:
			this->m_ctrIMEI1.EnableWindow(TRUE);
			this->m_ctrIMEI2.EnableWindow(TRUE);
			this->m_ctrIMEI3.EnableWindow(TRUE);
			this->m_ctrIMEI4.EnableWindow(FALSE);
			break;
		case 3:
			this->m_ctrIMEI1.EnableWindow(TRUE);
			this->m_ctrIMEI2.EnableWindow(TRUE);
			this->m_ctrIMEI3.EnableWindow(TRUE);
			this->m_ctrIMEI4.EnableWindow(TRUE);
			break;
		default:
			break;
		}
		
		//如果是扫描主IMEI号，那么重新设置控件
		if(this->m_Config.m_iIsScanMainImei==1)
		{
			this->m_ctrIMEI1.EnableWindow(TRUE);
			this->m_ctrIMEI2.EnableWindow(FALSE);
			this->m_ctrIMEI3.EnableWindow(FALSE);
			this->m_ctrIMEI4.EnableWindow(FALSE);
		}
		
		this->m_ctrIMEI1.SetFocus();
	}
}

void CColorBoxView::InsertListInfo(CString state,int imageIndex)
{
	CString date;
	date=GetCurTimes();
#ifndef _DEBUG
	if(3 == imageIndex)
		return;
#else
	imageIndex = 0;
#endif 
	m_state_list.InsertItem(m_state_list.GetItemCount(),date,imageIndex);
	m_state_list.SetItemText(m_state_list.GetItemCount()-1,1,state);
	m_state_list.EnsureVisible(m_state_list.GetItemCount()-1,TRUE);
	
}

CString CColorBoxView::GetCurTimes()
{
	CTime t=CTime::GetCurrentTime();
	CString str=t.Format( "%Y-%m-%d  %H:%M:%S");
	return str;
	
}


void CColorBoxView::SetUIDisableAll()
{
	this->m_ctrIMEI1.EnableWindow(FALSE);
	this->m_ctrIMEI2.EnableWindow(FALSE);
	this->m_ctrIMEI3.EnableWindow(FALSE);
	this->m_ctrIMEI4.EnableWindow(FALSE);
}


void CColorBoxView::EnableMobileCtrl(bool flag)
{
	this->m_ctrSelectDBButton.EnableWindow(flag);
	this->m_ctrSelectAPDBButton.EnableWindow(flag);
}


void CColorBoxView::ClearUI()
{
	switch(this->m_iIMEIType) 
	{
	case 0:
		this->m_csIMEI1.Empty();
		break;
	case 1:
		this->m_csIMEI1.Empty();
		this->m_csIMEI2.Empty();
		break;
	case 2:
		this->m_csIMEI1.Empty();
		this->m_csIMEI2.Empty();
		this->m_csIMEI3.Empty();
		break;
	case 3:
		this->m_csIMEI1.Empty();
		this->m_csIMEI2.Empty();
		this->m_csIMEI3.Empty();
		this->m_csIMEI4.Empty();
		break;
	default:
		break;
	}

	SendMessage(WM_UPDATEDATA, FALSE, FALSE);
	SendMessage(WM_SETMYFOCUS,(WPARAM)(&this->m_ctrIMEI1),FALSE);
}


CString CColorBoxView::VariantToCString(VARIANT   var)   
{   
	CString strValue;   
	_variant_t var_t;   
	_bstr_t bst_t;   
	//time_t cur_time;   
	CTime time_value;   
	COleCurrency var_currency; 
	COleDateTime cur_time;
    
	switch(var.vt)   
	{   
	case   VT_EMPTY:   
		strValue   =   _T("");   
		break;   
	case   VT_UI1:   
		strValue.Format("%d",var.bVal);   
		break;   
	case   VT_I2:   
		strValue.Format("%d",var.iVal);   
		break;   
	case   VT_I4:   
		strValue.Format("%d",var.lVal);   
		break;   
	case   VT_R4:   
		strValue.Format("%f",var.fltVal);   
		break;   
	case   VT_R8:   
		strValue.Format("%f",var.dblVal);   
		break;   
	case   VT_CY:   
		var_currency   =   var;   
		strValue   =   var_currency.Format(0);   
		break;   
	case   VT_BSTR:   
		var_t   =   var;   
		bst_t   =   var_t;   
		strValue.Format   ("%s",(const   char*)bst_t);   
		break;   
	case   VT_NULL:   
		strValue   =   _T("");   
		break;   
	case   VT_DATE:   
		//cur_time   =   (long)var.date;   
		// time_value   =   cur_time;   
		// strValue   =   time_value.Format("%Y-%m-%d");  
		cur_time   =   var; 
		strValue   =   cur_time.Format("%Y-%m-%d"); 
		break;   
	case   VT_BOOL:   
		strValue.Format("%d",var.boolVal   );   
		break;   
	default:     
		strValue   =   _T("");   
		break;   
	}   
	return   strValue;   
}

void CColorBoxView::OnChangeEditImei1() 
{
	m_iFirstKeyTime = GetTickCount();
	if(m_csIMEI1.GetLength() != 0)
	{
		m_iKeyPeriod = m_iFirstKeyTime - m_iLastKeyTime;
		if(m_iKeyPeriod > 100)
		{
#ifndef _DEBUG
			InsertListInfo("Please Use The Scanner Error!",2);
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
			m_csIMEI1.Empty();
			UpdateData(FALSE);
			m_ctrIMEI1.SetFocus();
			ShowCalResult(FAIL);
			return;
#endif
		}
	}
	m_iLastKeyTime = m_iFirstKeyTime;
	
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);

	switch(this->m_iIMEIType)
	{
	case 0:	
		if(m_csIMEI1.Right(2)=="\r\n")
		{
			m_csIMEI1.Replace("\r\n","");
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			
			if(!IMEITYPE_0_CheckImei1())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
				m_csIMEI1.Empty();
				UpdateData(FALSE);
				m_ctrIMEI1.SetFocus();
				ShowCalResult(FAIL);
				return;
			}
			
			this->m_ctrIMEI1.EnableWindow(FALSE);			
			
			if(m_iPlatform==1) 
			{
				m_hThreadSPStart_Ex =
					CreateThread (NULL,  
					0,     
					(LPTHREAD_START_ROUTINE)SmartStartReadIMEI_Ex,
					(void *)this,  
					0,     
					&m_dThreadIdSPStart_Ex);
			}	
			else if(this->m_iPlatform==2)
			{
				m_hThreadSPStart =
					CreateThread (NULL,  
					0,     
					(LPTHREAD_START_ROUTINE)SPRDStart,
					(void *)this,  
					0,     
					&m_dThreadIdSPStart);
			}
		}
		break;
	case 1:
		if(this->m_Config.m_iIsScanMainImei==0) 
		{
			if(m_csIMEI1.Right(2)=="\r\n")
			{
				m_csIMEI1.Replace("\r\n",""); 
				//UpdateData(FALSE);
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				if(!IMEITYPE_1_CheckImei1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
					m_csIMEI1.Empty();
					UpdateData(FALSE);
					m_ctrIMEI1.SetFocus();
					ShowCalResult(FAIL);
					return;
				}
				this->m_ctrIMEI1.EnableWindow(FALSE);
				this->m_ctrIMEI2.SetFocus();
				this->m_ctrIMEI2.SetSel(0,-1);	
			}
		}
		else
		{
			if(m_csIMEI1.Right(2)=="\r\n")
			{
				m_csIMEI1.Replace("\r\n","");
				
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(!IMEITYPE_0_CheckImei1_Ex())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
					m_csIMEI1.Empty();
					UpdateData(FALSE);
					m_ctrIMEI1.SetFocus();
					ShowCalResult(FAIL);
					return;
				}
				this->m_ctrIMEI1.EnableWindow(FALSE);
				if(this->m_iPlatform==1)
				{
					m_hThreadSPStart_Ex =
						CreateThread (NULL,  
						0,     
						(LPTHREAD_START_ROUTINE)SmartStartReadIMEI_Ex,
						(void *)this,  
						0,     
						&m_dThreadIdSPStart_Ex);
				}	
				else if(this->m_iPlatform==2)
				{
					m_hThreadSPStart =
						CreateThread (NULL,  
						0,     
						(LPTHREAD_START_ROUTINE)SPRDStart,
						(void *)this,  
						0,     
						&m_dThreadIdSPStart);
				}
			}
		}
		break;
	case 2:
		if(this->m_Config.m_iIsScanMainImei==0) 
		{
			if(m_csIMEI1.Right(2)=="\r\n")
			{
				m_csIMEI1.Replace("\r\n","");
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				if(!IMEITYPE_1_CheckImei1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
					m_csIMEI1.Empty();
					UpdateData(FALSE);
					m_ctrIMEI1.SetFocus();
					ShowCalResult(FAIL);
					return;
				}
				this->m_ctrIMEI1.EnableWindow(FALSE);
				this->m_ctrIMEI2.SetFocus();
				this->m_ctrIMEI2.SetSel(0,-1);
			}
		}
		else
		{
			if(m_csIMEI1.Right(2)=="\r\n")
			{
				m_csIMEI1.Replace("\r\n","");
				//UpdateData(FALSE);
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(!IMEITYPE_0_CheckImei1_Ex())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
					m_csIMEI1.Empty();
					UpdateData(FALSE);
					m_ctrIMEI1.SetFocus();
					ShowCalResult(FAIL);
					return;
				}
				this->m_ctrIMEI1.EnableWindow(FALSE);
				
				if(this->m_iPlatform==1)
				{
					m_hThreadSPStart_Ex =
						CreateThread (NULL,  
						0,     
						(LPTHREAD_START_ROUTINE)SmartStartReadIMEI_Ex,
						(void *)this,  
						0,     
						&m_dThreadIdSPStart_Ex);
				}	
				else if(this->m_iPlatform==2)
				{
					m_hThreadSPStart =
						CreateThread (NULL,  
						0,     
						(LPTHREAD_START_ROUTINE)SPRDStart,
						(void *)this,  
						0,     
						&m_dThreadIdSPStart);
				}
			}
		}
		break;
	case 3:
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(m_csIMEI1.Right(2)=="\r\n")
			{
				m_csIMEI1.Replace("\r\n","");
				//UpdateData(FALSE);
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				if(!IMEITYPE_1_CheckImei1())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
					m_csIMEI1.Empty();
					UpdateData(FALSE);
					m_ctrIMEI1.SetFocus();
					ShowCalResult(FAIL);
					return;
				}
				this->m_ctrIMEI1.EnableWindow(FALSE);
				this->m_ctrIMEI2.SetFocus();
				this->m_ctrIMEI2.SetSel(0,-1);
			}
		}
		else
		{
			if(m_csIMEI1.Right(2)=="\r\n")
			{
				m_csIMEI1.Replace("\r\n","");
				//UpdateData(FALSE);
				SendMessage(WM_UPDATEDATA, FALSE, FALSE);
				
				if(!IMEITYPE_0_CheckImei1_Ex())
				{
					//警告提示音
					PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
					m_csIMEI1.Empty();
					UpdateData(FALSE);
					m_ctrIMEI1.SetFocus();
					ShowCalResult(FAIL);
					return;
				}
				this->m_ctrIMEI1.EnableWindow(FALSE);
				
				if(this->m_iPlatform==1)
				{
					m_hThreadSPStart_Ex =
						CreateThread (NULL,  
						0,     
						(LPTHREAD_START_ROUTINE)SmartStartReadIMEI_Ex,
						(void *)this,  
						0,     
						&m_dThreadIdSPStart_Ex);
				}	
				else if(this->m_iPlatform==2)
				{
					m_hThreadSPStart =
						CreateThread (NULL,  
						0,     
						(LPTHREAD_START_ROUTINE)SPRDStart,
						(void *)this,  
						0,     
						&m_dThreadIdSPStart);
				}
			}
		}
		break;
	default:
		break;
	}
}


BOOL CColorBoxView::IMEITYPE_0_CheckImei1()
{
#ifdef _DEBUG
	return TRUE;
#endif
	//检测长度是否为15位
	if(m_csIMEI1.GetLength()!=15)
	{
		InsertListInfo("IMEI1 Length Is Not 15...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI1.Left(14)>this->m_imei1_end || m_csIMEI1.Left(14)<this->m_imei1_start)
		{
			InsertListInfo("IMEI1 Out Of The Range Fail...",2);
			this->m_ctrIMEI1.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;
		
		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI1='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI1);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();

			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
			
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI1 Is Not Exist In Server...",2);
				return FALSE;
			}
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}

	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI1.Left(14),calculate_digit);

	if(calculate_digit!=m_csIMEI1.Right(1))
	{
		InsertListInfo("Illegal IMEI1,The 15th IMEI1 Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

 	//检测写号是否已经做过
 	CString temp,temp2;
 	_variant_t var;
 	CString sql;
 
 	try
 	{
 		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and Status IS NOT NULL",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1);
 		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
 			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
 		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
 		if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
 		{
 			InsertListInfo("This IMEI1 have not done write IMEI...",2);
 			return FALSE;
 		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
 			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
 	}
 	catch(_com_error e)
 	{
 		InsertListInfo("Unknown Error...",2);
 		this->m_ctrIMEI1.SetSel(0,-1);
 		return FALSE;
 	}
 
	return TRUE;
}


BOOL CColorBoxView::IMEITYPE_1_CheckImei1()
{
	//检测长度是否为15位
	if(m_csIMEI1.GetLength()!=15)
	{
		InsertListInfo("IMEI1 Length Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI1.Left(14)>this->m_imei1_end || m_csIMEI1.Left(14)<this->m_imei1_start)
		{
			InsertListInfo("IMEI1 out of Range fail...",2);
			this->m_ctrIMEI1.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI1='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI1);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI1 is ot in the IMEI table...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}

	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI1.Left(14),calculate_digit);

	if(calculate_digit!=m_csIMEI1.Right(1))	
	{
		InsertListInfo("Illegal IMEI1,Parity error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}
	//检测写号是否已经做过
	CString temp,temp2;
	_variant_t var;
	CString sql;
	
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and Status IS NOT NULL",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			InsertListInfo("This IMEI1 have not done write IMEI...",2);
			return FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
 	}

	return TRUE;
}

void CColorBoxView::calculate_15_digit_imei( char *imei_14,char &calculate_digit)
{
	int check_digit,sum1,sum2,total_sum,temp_value,temp_value_2_part;
	int i;
	char temp[16];
	
	
	// the following is calculate IMEI 15th digit -- check digit //
	// according   3G TS 22.016  V3.1.0 //
	/* 
	A.2 Computation of Check Digit for an IMEI
	Computation of CD from the IMEI proceeds as follows:
	Step 1: Double the values of the odd labelled digits D1, D3, D5 ... D13 of the IMEI.
	Step 2: Add together the individual digits of all the seven numbers obtained in Step 1, and then add this sum to
	the sum of all the even labelled digits D2, D4, D6 ... D14 of the IMEI.
	Step 3: If the number obtained in Step 2 ends in 0, then set CD to be 0. If the number obtained in Step 2 does not
	end in 0, then set CD to be that number subtracted from the next higher number which does end in 0.
	
	  
		A.3 Example of computation
		IMEI (14 most significant digits):
		TAC FAC SNR
		D14  D13 D12 D11 D10 D9 D8 D7  D6 D5 D4 D3 D2 D1
		2    6   0   5   3   1  7  9  3   1  1  3  8  3
		Step 1:
		2    6   0   5   3   1  7  9  3   1  1  3  8  3 
		x2      x2      x2    x2     x2    x2   x2
		12      10      2     18     2      6    6
		Step 2:
		2 + 1 + 2 + 0 + 1 + 0 + 3 + 2 + 7 + 1 + 8 + 3 + 2 + 1 + 6 + 8 + 6 = 53
		Step 3:
		CD = 60 - 53 = 7
	*/
	for(i=0;i<14;i++)
		temp[i] = imei_14[i];
	
	sum1 = 0;
	for (i=1;i<=13;i++)		  // sum digit 1,3,5,7,9,11,13
	{
		sum1 = sum1 + (temp[i-1]-48);
		i++;
	}
	sum2 = 0;
	for (i=2;i<=14;i++)			 // sum digit 2,4,6,8,10,12,14   
	{
		temp_value =  (temp[i-1]-48) * 2 ;			// temp_value range from 0 --> 9*2 = 18
		if ( temp_value < 10 )  
			sum2 = sum2 +  temp_value ;
		else  
		{
			temp_value_2_part = temp_value - 10 ;
			sum2 = sum2 + 1 +temp_value_2_part;
		} 
		
		i++;				  
	}
	
	total_sum = sum1 + sum2 ;
	
	if ( fmod(total_sum,10) == 0 )
		check_digit = 0  ; 	   // if total_sum ends in 0 ( mod 10 reminder is 0 ) 
	else 
	{
		check_digit = (total_sum /10)*10 + 10 - total_sum;
		
	}
	calculate_digit=check_digit+48;
	
}

BOOL CColorBoxView::IMEITYPE_1_CheckImei2()
{
	
	if(m_csIMEI1==m_csIMEI2)
	{
		InsertListInfo("IMEI1与IMEI2不能相同...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}
	
	//检测长度是否为15位
	if(m_csIMEI2.GetLength()!=15)
	{
		InsertListInfo("IMEI2 Length Error...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI2.Left(14)>m_imei2_end || m_csIMEI2.Left(14)<m_imei2_start)
		{
			InsertListInfo("IMEI4 out of Range fail...",2);
			this->m_ctrIMEI2.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI2='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI2);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("is ot in the IMEI table...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI2校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI2.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI2.Right(1))
	{
		InsertListInfo("Illegal IMEI,Parity error...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	//判断IMEI1和IMEI2是否同时在colorbox表中

	CString temp,temp2;
	_variant_t var;
	CString sql;
	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') AND (IMEI2='%s') and Status IS NOT NULL",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1,(char*)(LPCTSTR)m_csIMEI2);  //@@ Call storage course @@
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			InsertListInfo("This IMEI1 IMEI2 have not done write IMEI...",2);
			return FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}	

	return TRUE;
}

void CColorBoxView::OnChangeEditImei2() 
{
	m_iFirstKeyTime = GetTickCount();
	if(m_csIMEI2.GetLength() != 0)
	{
		m_iKeyPeriod = m_iFirstKeyTime - m_iLastKeyTime;
		if(m_iKeyPeriod > 100)
		{
			InsertListInfo("Please Use The Scanner!",2);
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
			m_csIMEI2.Empty();
			UpdateData(FALSE);
			m_ctrIMEI2.SetFocus();
			ShowCalResult(FAIL);
		 	return;
		}
	}
	m_iLastKeyTime = m_iFirstKeyTime;
	
	//UpdateData();
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	switch(this->m_iIMEIType)
	{
	case 1:
		if(m_csIMEI2.Right(2)=="\r\n")
		{
			m_csIMEI2.Replace("\r\n","");
			//UpdateData(FALSE);
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			if(!IMEITYPE_1_CheckImei2_Ex())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
				m_csIMEI2.Empty();
				UpdateData(FALSE);
				m_ctrIMEI2.SetFocus();
				ShowCalResult(FAIL);
				return;
			}
			this->m_ctrIMEI2.EnableWindow(FALSE);
			
			if(this->m_iPlatform==1)
			{
				m_hThreadSPStart_Ex =
					CreateThread (NULL,  
					0,     
					(LPTHREAD_START_ROUTINE)SmartStartReadIMEI_Ex,
					(void *)this,  
					0,     
					&m_dThreadIdSPStart_Ex);
			}
			else if(this->m_iPlatform==2)
			{
				m_hThreadSPStart =
					CreateThread (NULL,  
					0,     
					(LPTHREAD_START_ROUTINE)SPRDStart,
					(void *)this,  
					0,     
					&m_dThreadIdSPStart);
			}
		}
		break;
	case 2:
		if(m_csIMEI2.Right(2)=="\r\n")
		{
			m_csIMEI2.Replace("\r\n","");
			//UpdateData(FALSE);
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			if(!IMEITYPE_2_CheckImei2())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
				m_csIMEI2.Empty();
				UpdateData(FALSE);
				m_ctrIMEI2.SetFocus();
				ShowCalResult(FAIL);
				return;
			}
			this->m_ctrIMEI2.EnableWindow(FALSE);
			this->m_ctrIMEI3.SetFocus();
			this->m_ctrIMEI3.SetSel(0,-1);			
		}
		break;
	case 3:
		if(m_csIMEI2.Right(2)=="\r\n")
		{
			m_csIMEI2.Replace("\r\n","");
			//UpdateData(FALSE);
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			if(!IMEITYPE_2_CheckImei2())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
				m_csIMEI2.Empty();
				UpdateData(FALSE);
				m_ctrIMEI2.SetFocus();
				ShowCalResult(FAIL);
				return;
			}
			this->m_ctrIMEI2.EnableWindow(FALSE);
			this->m_ctrIMEI3.SetFocus();
			this->m_ctrIMEI3.SetSel(0,-1);			
		}
		break;
	default:
		break;
	}	
}


BOOL CColorBoxView::IMEITYPE_2_CheckImei2()
{
	if(m_csIMEI2.GetLength()!=15)
	{
		InsertListInfo("IMEI2 Length Error...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI2.Left(14)>this->m_imei2_end || m_csIMEI2.Left(14)<this->m_imei2_start)
		{
			InsertListInfo("IMEI2 out of Range fail...",2);
			this->m_ctrIMEI2.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI2='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI2);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("is ot in the IMEI table...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI2.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI2.Right(1))	
	{
		InsertListInfo("Illegal IMEI2,Parity error...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}
	return TRUE;
}


BOOL CColorBoxView::IMEITYPE_3_CheckImei3()
{
	if(m_csIMEI3.GetLength()!=15)
	{
		InsertListInfo("IMEI3 Length Error...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI3.Left(14)>this->m_imei3_end || m_csIMEI3.Left(14)<this->m_imei3_start)
		{
			InsertListInfo("IMEI3 out of Range fail...",2);
			this->m_ctrIMEI3.SetSel(0,-1);
			return FALSE;	
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI3='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI3);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("is ot in the IMEI table...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI3.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI3.Right(1))	
	{
		InsertListInfo("Illegal IMEI3,Parity error...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}
	return TRUE;
}

void CColorBoxView::OnChangeEditImei3() 
{
	m_iFirstKeyTime = GetTickCount();
	if(m_csIMEI3.GetLength() != 0)
	{
		m_iKeyPeriod = m_iFirstKeyTime - m_iLastKeyTime;
		if(m_iKeyPeriod > 100)
		{   
			InsertListInfo("Please Use The Scanner!",2);
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
			m_csIMEI3.Empty();
			UpdateData(FALSE);
			m_ctrIMEI3.SetFocus();
			ShowCalResult(FAIL);
			return;
		}
	}
	m_iLastKeyTime = m_iFirstKeyTime;
	
	//UpdateData();
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);	
	switch(this->m_iIMEIType)
	{
	case 2:
		if(m_csIMEI3.Right(2)=="\r\n")
		{
			m_csIMEI3.Replace("\r\n","");
			//UpdateData(FALSE);
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			if(!IMEITYPE_2_CheckImei3_Ex())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
				m_csIMEI3.Empty();
				UpdateData(FALSE);
				m_ctrIMEI3.SetFocus();
				ShowCalResult(FAIL);
				return;
			}
			this->m_ctrIMEI3.EnableWindow(FALSE);
			
			if(this->m_iPlatform==1)
			{
				m_hThreadSPStart_Ex =
					CreateThread (NULL,  
					0,     
					(LPTHREAD_START_ROUTINE)SmartStartReadIMEI_Ex,
					(void *)this,  
					0,     
					&m_dThreadIdSPStart_Ex);
			}
			else if(this->m_iPlatform==2)
			{
				m_hThreadSPStart =
					CreateThread (NULL,  
					0,     
					(LPTHREAD_START_ROUTINE)SPRDStart,
					(void *)this,  
					0,     
					&m_dThreadIdSPStart);
			}
		}
		break;
	case 3:
		if(m_csIMEI3.Right(2)=="\r\n")
		{
			m_csIMEI3.Replace("\r\n","");
			//UpdateData(FALSE);
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			if(!IMEITYPE_3_CheckImei3())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
				m_csIMEI3.Empty();
				UpdateData(FALSE);
				m_ctrIMEI3.SetFocus();
				ShowCalResult(FAIL);
				return;
			}
			this->m_ctrIMEI3.EnableWindow(FALSE);
			this->m_ctrIMEI4.SetFocus();
			this->m_ctrIMEI4.SetSel(0,-1);			
		}
		break;
	default:
		break;
	}
}


BOOL CColorBoxView::IMEITYPE_2_CheckImei3()
{
	
	if(m_csIMEI1==m_csIMEI2)
	{
		InsertListInfo("IMEI1与IMEI2不能相同...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI2==m_csIMEI3)
	{
		InsertListInfo("IMEI2与IMEI3不能相同...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI1==m_csIMEI3)
	{
		InsertListInfo("IMEI1与IMEI3不能相同...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}
	
	//检测长度是否为15位
	if(m_csIMEI3.GetLength()!=15)
	{
		InsertListInfo("IMEI3  Length Error...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI3.Left(14)>m_imei3_end || m_csIMEI3.Left(14)<m_imei3_start)
		{
			InsertListInfo("IMEI3 out of Range fail...",2);
			this->m_ctrIMEI3.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI3='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI3);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("is ot in the IMEI table...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI3校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI3.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI3.Right(1))
	{
		InsertListInfo("Illegal IMEI,Parity error...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	CString temp,temp2;
	_variant_t var;
	CString sql;
	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') AND (IMEI2='%s') AND (IMEI3='%s') and Status IS NOT NULL",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1,(char*)(LPCTSTR)m_csIMEI2,(char*)(LPCTSTR)m_csIMEI3);  //@@ Call storage course @@
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			InsertListInfo("该IMEI1 IMEI2 IMEI3未做过耦合测试...",2);
			return FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}

BOOL CColorBoxView::IMEITYPE_2_CheckImei3_Ex()
{
	if(m_csIMEI1==m_csIMEI2)
	{
		InsertListInfo("IMEI1 is same as IMEI2 Fail...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI2==m_csIMEI3)
	{
		InsertListInfo("IMEI2 is same as IMEI3 Fail...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI1 == m_csIMEI3)
	{
		InsertListInfo("IMEI1 is same as IMEI3 Fail...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}
	
	//检测长度是否为15位
	if(m_csIMEI3.GetLength()!=15)
	{
		InsertListInfo("IMEI Length is not 15 Fail...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI3.Left(14)>m_imei3_end || m_csIMEI3.Left(14)<m_imei3_start)
		{
			InsertListInfo("IMEI3 Out of Range Error ...",2);
			this->m_ctrIMEI3.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI3='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI3);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI3 Is Not Exist in IMEI Table Error...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI3校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI3.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI3.Right(1))
	{
		InsertListInfo("illegal IMEI,The 15th IMEI Error Fail...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}

void CColorBoxView::OnChangeEditImei4() 
{
	m_iFirstKeyTime = GetTickCount();
	if(m_csIMEI4.GetLength() != 0)
	{
		m_iKeyPeriod = m_iFirstKeyTime - m_iLastKeyTime;
		if(m_iKeyPeriod > 1001111111)
		{   
			InsertListInfo("Please Use The Scanner!",2);
			PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
			m_csIMEI4.Empty();
			UpdateData(FALSE);
			m_ctrIMEI4.SetFocus();
			ShowCalResult(FAIL);
			return;
		}
	}
	m_iLastKeyTime = m_iFirstKeyTime;
	
	//UpdateData();
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
	switch(this->m_iIMEIType)
	{
	case 3:
		if(m_csIMEI4.Right(2)=="\r\n")
		{
			m_csIMEI4.Replace("\r\n","");
			//UpdateData(FALSE);
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
			if(!IMEITYPE_3_CheckImei4_Ex())
			{
				//警告提示音
				PlaySound((LPCSTR)(LPCTSTR)m_csErrorSndPath, NULL, SND_FILENAME | SND_ASYNC);  
				m_csIMEI4.Empty();
				UpdateData(FALSE);
				m_ctrIMEI4.SetFocus();
				ShowCalResult(FAIL);
				return;
			}
			this->m_ctrIMEI4.EnableWindow(FALSE);
			
			if(this->m_iPlatform==1)
			{
				m_hThreadSPStart_Ex =
					CreateThread (NULL,  
					0,     
					(LPTHREAD_START_ROUTINE)SmartStartReadIMEI_Ex,
					(void *)this,  
					0,     
					&m_dThreadIdSPStart_Ex);
			}
			else if(this->m_iPlatform==2)
			{
				m_hThreadSPStart =
					CreateThread (NULL,  
					0,     
					(LPTHREAD_START_ROUTINE)SPRDStart,
					(void *)this,  
					0,     
					&m_dThreadIdSPStart);
			}
		}
		break;
	default:
		break;
	}
}


BOOL CColorBoxView::IMEITYPE_3_CheckImei4()
{	
	if(m_csIMEI1==m_csIMEI2)
	{
		InsertListInfo("IMEI1与IMEI2不能相同...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI2==m_csIMEI3)
	{
		InsertListInfo("IMEI2与IMEI3不能相同...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI1==m_csIMEI3)
	{
		InsertListInfo("IMEI1与IMEI3不能相同...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI1==m_csIMEI4)
	{
		InsertListInfo("IMEI1与IMEI4不能相同...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI2==m_csIMEI4)
	{
		InsertListInfo("IMEI2与IMEI4不能相同...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI3==m_csIMEI4)
	{
		InsertListInfo("IMEI3与IMEI4不能相同...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}
	
	//检测长度是否为15位
	if(m_csIMEI4.GetLength()!=15)
	{
		InsertListInfo("IMEI4  Length Error...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI4.Left(14)>m_imei4_end || m_csIMEI4.Left(14)<m_imei4_start)
		{
			InsertListInfo("IMEI4 out of Range fail...",2);
			this->m_ctrIMEI4.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI4='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI4);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("is ot in the IMEI table...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI4校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI4.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI4.Right(1))
	{
		InsertListInfo("Illegal IMEI,Parity error...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}
	
	CString temp,temp2;
	_variant_t var;
	CString sql;
	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') AND (IMEI2='%s') AND (IMEI3='%s') AND (IMEI4='%s') and Status IS NOT NULL",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1,(char*)(LPCTSTR)m_csIMEI2,(char*)(LPCTSTR)m_csIMEI3,(char*)(LPCTSTR)m_csIMEI4);  //@@ Call storage course @@
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			InsertListInfo("该IMEI1 IMEI2 IMEI3 IMEI4未做过耦合测试...",2);
			return FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}


BOOL CColorBoxView::IMEITYPE_3_CheckImei4_Ex()
{
	if(m_csIMEI1==m_csIMEI2)
	{
		InsertListInfo("IMEI1 is same as IMEI2 Fail...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI2==m_csIMEI3)
	{
		InsertListInfo("IMEI2 is same as IMEI3 Fail...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI1==m_csIMEI3)
	{
		InsertListInfo("IMEI1 is same as IMEI3 Fail...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI1==m_csIMEI4)
	{
		InsertListInfo("IMEI1 is same as IMEI4 Fail...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI2==m_csIMEI4)
	{
		InsertListInfo("IMEI2 is same as 与IMEI4 Fail...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}

	if(m_csIMEI3==m_csIMEI4)
	{
		InsertListInfo("IMEI3 is same as IMEI4 Fail...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}
	
	//检测长度是否为15位
	if(m_csIMEI4.GetLength()!=15)
	{
		InsertListInfo("IMEI Length is not 15 Fail...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI4.Left(14)>m_imei4_end || m_csIMEI4.Left(14)<m_imei4_start)
		{
			InsertListInfo("IMEI Out Range Fail...",2);
			this->m_ctrIMEI4.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI4='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI4);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI4 is Not Exist in IMEI Table Fail...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI4校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI4.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI4.Right(1))
	{
		InsertListInfo("illegal IMEI,The 15th IMEI Error Fail...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}
	
	return TRUE;
}

void CColorBoxView::PrintLabel()
{
	if(m_checkColorDone)
	{
		InsertListInfo("Check Server And IMEI Pass...",0);
	}
	else
	{
		if(!SaveToColorInfo())
				return;
		InsertListInfo("Check Server And IMEI Pass...",0);
	}
}


BOOL CColorBoxView::SaveToColorInfo()
{
	CString temp; 
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("UPDATE %s_%s_ColorBox SET Status=1 where IMEI1='%s'",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1);
		((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
	}
	catch(_com_error e)
	{
		InsertListInfo("Save record to ColorBox fail...",2);
		return FALSE;
	}

	sql.Format("UPDATE %s_%s_ProductionLine SET CheckIMEIS1='%s',CheckS1Date=getdate() where IMEI1='%s'", \
		(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder, \
		this->m_csCurrentLine,this->m_csIMEI1);

	try
	{
		((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
	}
	catch(_com_error e)
	{
		return FALSE;
	}

	InsertListInfo("Save record to ColorBox success...",0);

	return TRUE;
}

int CColorBoxView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
#ifndef NOSQL	
	CSplashWnd::ShowSplashScreen(this);
#endif	
	return 0;
}

void CColorBoxView::OnMode1() 
{
	//禁止修改
	return;
}

void CColorBoxView::OnMode2() 
{
	//禁止修改
	return;
}

void CColorBoxView::OnMode3() 
{
	//禁止修改
	return;
}


void CColorBoxView::LoadComPort()
{
	CString str;
	this->m_ctrComboxComPort.ResetContent();
	int portnum = 0;   
	CHAR port[MAX_PATH];   
	COMMCONFIG comcfg;   
	ULONG BuffSize = sizeof(port);
	for(int i = 0; i< 255 ; i++) 
	{
		m_comnum[i] = 0;
	}
	for( i = 3; i< 255 ; i++)   
	{   		
		_stprintf(port,_T("COM%d"),i);   
		BuffSize = sizeof(port);   
		if(GetDefaultCommConfig(port,&comcfg,&BuffSize))   
		{
			this->m_ctrComboxComPort.AddString(port); 
			m_comnum[portnum++] = i;
		}			
		memset(port,0,sizeof(port));   
	} 	
	m_ctrComboxComPort.SetCurSel(-1);
	this->m_ctrComboxComPort.AddString("USB");
}

void CColorBoxView::OnSelchangeComboComport() 
{
	m_comport = this->m_ctrComboxComPort.GetCurSel();	
	this->m_iComboxComPort=m_comport;
	m_comport = m_comnum[m_comport];
	//UpdateData();	// TODO: Add your control notification handler code here
	SendMessage(WM_UPDATEDATA, TRUE, FALSE);
}


void CColorBoxView::OnBtnSelect() 
{	
	CPassword dlg;
	dlg.ilevel = 0; //需普通权限验证
 	if(dlg.DoModal()!=IDOK)
 	{
 		return;
 	}
	

	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle="Select Modem DBFile";
	if(filedlg.DoModal()==IDOK)
	{
		this->m_csDBPath = filedlg.GetPathName();

		if(this->m_csDBPath.IsEmpty())
		{
			
		}
		SendMessage(WM_UPDATEDATA, FALSE, FALSE);

		m_hThreadModemNVInit =
				CreateThread (NULL,
				0,     
				(LPTHREAD_START_ROUTINE)InitFPNV,
				(void *)this,  
				0,     
				&m_dThreadIdModemNVInit);
	}
	else
	{
		return;
	}
}

void CColorBoxView::OnBtnSelectap() 
{
	CPassword dlg;
	dlg.ilevel = 0; //需普通权限验证
 	if(dlg.DoModal()!=IDOK)
 	{
 		return;
 	}

	CFileDialog filedlg(TRUE);
	filedlg.m_ofn.lpstrTitle="Select AP DBFile";
	if(filedlg.DoModal()==IDOK)
	{
		this->m_csAPDBPath = filedlg.GetPathName();

		if(this->m_csAPDBPath.IsEmpty())
		{
			
		}
		SendMessage(WM_UPDATEDATA, FALSE, FALSE);

		m_hThreadSPNVInit =
				CreateThread (NULL,  
				0,     
				(LPTHREAD_START_ROUTINE)InitSPNV,
				(void *)this,  
				0,     
				&m_dThreadIdSPNVInit);
	}
	else
	{
		return;
	}	
}

bool CColorBoxView::DoInitFPNV()
{
	m_MetaappResult=METAAPP_DeInit(m_nMetaHandle);	
	m_MetaappResult=METAAPP_GetAvailableHandle(&m_nMetaHandle);
	m_MetaappResult=METAAPP_Init(m_nMetaHandle);
	m_MetaappResult=METAAPP_NVRAM_Init(m_nMetaHandle,this->m_csDBPath);
	if(m_MetaappResult != METAAPP_SUCCESS)   
	{ 
		m_MetaappResult=METAAPP_DeInit(m_nMetaHandle);
		return FALSE;
	}
	return TRUE;	
}


bool CColorBoxView::DoInitSPApNV()
{
	BOOL bResult=false;
	this->m_ApExe.MetaAPHandleDeInit();
	bResult = this->m_ApExe.MetaAPHandleInit();
	
	if(!bResult)
		return FALSE;
	
	bResult = this->m_ApExe.InitAPDatabase((LPSTR)(LPCTSTR)this->m_csAPDBPath);
	
	if(bResult)
		return TRUE;
	else
		return FALSE;
}

bool CColorBoxView::DoInitSPModemNV()
{
	m_MetaappResult=METAAPP_DeInit(m_nMetaHandle);
	
	m_MetaappResult=METAAPP_GetAvailableHandle(&m_nMetaHandle);
	m_MetaappResult=METAAPP_Init(m_nMetaHandle);
	m_MetaappResult=METAAPP_NVRAM_Init(m_nMetaHandle,this->m_csDBPath);
	if(m_MetaappResult != METAAPP_SUCCESS)   
	{
		m_MetaappResult=METAAPP_DeInit(m_nMetaHandle);
		return FALSE;
	}
	
	return TRUE;
}


AUTH_HANDLE_T CColorBoxView::Get_AuthHandle( void )
{   
    return m_sAuthHandle;
}

bool CColorBoxView::FP_ExitMetaMode()
{
	m_MetaappResult = METAAPP_DisconnectWithTarget(m_nMetaHandle);
	if (m_MetaappResult!=0)
	{
		InsertListInfo("Exit Meta Mode Fail...",2);
		return FALSE;
	}
	return TRUE;
}


bool CColorBoxView::FP_Read_PCB_BARCODE()
{
	char read_buffer_temp[70];
	char ch;
	if (m_iPlatform == 0 || m_iPlatform == 1)
	{
		m_MetaappResult=METAAPP_readBarcodefromNVRAM(m_nMetaHandle, 5000, read_buffer_temp);
		if (m_MetaappResult!=0)
		{
			return FALSE;
		}
		read_buffer_temp[63]='\0';
	}
	else if (m_iPlatform == 2)
	{
		nErrorCode = SP_ReadSN(g_hDiagPhone, true, (void *)read_buffer_temp, sizeof(read_buffer_temp)/*24*/);
		if(RFT_SUCCESS != nErrorCode)
		{
			return FALSE;
		}

		//lucky 20150915
		ch = read_buffer_temp[14];
		if((ch>=48&&ch<=57)||(ch>=97&&ch<=122)||(ch>=65&&ch<=90))
		{
			read_buffer_temp[16]='\0';
		}
		else
		{
			read_buffer_temp[14]='\0';
		}
	}

	this->m_csPcbaSN.Format("%s",read_buffer_temp);
	return TRUE;
}


bool CColorBoxView::SP_CHECK_NV()
{
#ifdef _DEBUG 
	return TRUE;
#endif
	//检测BARCODE长度
	if (this->m_csPcbaSN.GetLength()< 62)
	{
		InsertListInfo("NVRAM:Mobile phone is not calibrated or full wipe Download...",2);
		return FALSE;
	}

	//检测校准终测标志位
	if(this->m_iBTFT)
	{
		if(this->m_iPlatform==0) //fixture phone
		{
			if(m_csPcbaSN.GetAt(60)!='1' ||
				m_csPcbaSN.GetAt(61)!='0' ||
				m_csPcbaSN.GetAt(62)!='P')
			{
				InsertListInfo("NVRAM:Mobile phone is not calibrated or final test Fail...",2);
				return FALSE;
			}
			else
			{
				InsertListInfo("NVRAM:Mobile phone is  calibrated or final test Pass...",0);	
			}
		}
		else
		{
			if( m_csPcbaSN.GetAt(60)!='1' ||
				m_csPcbaSN.GetAt(61)!='0' ||
				m_csPcbaSN.GetAt(62)!='P')
			{
				InsertListInfo("NVRAM:Mobile phone is not calibrated or final test Fail......",2);
				return FALSE;
			}
			else
			{
				InsertListInfo("NVRAM:Mobile phone is  calibrated or final test Pass...",0);	
			}
		}
	}

	return TRUE;
}


bool CColorBoxView::FPCheckVersion()
{
	m_MetaappResult= METAAPP_GetTargetVerInfo( this->m_nMetaHandle, 5000, &p_verinfo);
	if(m_MetaappResult != METAAPP_SUCCESS)
	{
		return FALSE;
	}
	else
	{
		if(strcmp(p_verinfo.SW_VER,(LPSTR)(LPCTSTR)this->m_csVersion))
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	return TRUE;
}



bool CColorBoxView::FP_Read_IMEI_Ex()	
{
	this->m_csIMEI1_Ex.Empty();
	this->m_csIMEI2_Ex.Empty();
	this->m_csIMEI3_Ex.Empty();
	this->m_csIMEI4_Ex.Empty();

	switch(this->m_iIMEIType)
	{
	case 0: //读单IMEI
		if(!this->ReadSimpleIMEI_Ex())
			return FALSE;
		break;
	case 1: //读双IMEI
		//判断是否只扫描主IMEI，如果是那么不管几卡的手机都只读取比对IMEI1
		if(this->m_Config.m_iIsScanMainImei==1)
		{
			if(!this->ReadSimpleIMEI_Ex())
				return FALSE;
		}
		else
		{
			if(!this->ReadDoubleIMEI_Ex())
				return FALSE;
		}
		break;
	case 2: //读三IMEI
		//判断是否只扫描主IMEI，如果是那么不管几卡的手机都只读取比对IMEI1
		if(this->m_Config.m_iIsScanMainImei==1)
		{
			if(!this->ReadSimpleIMEI_Ex())
				return FALSE;
		}
		else
		{
			if(!this->ReadThreeIMEI_Ex())
				return FALSE;
		}
		break;	
	case 3: //读四IMEI
		//判断是否只扫描主IMEI，如果是那么不管几卡的手机都只读取比对IMEI1
		if(this->m_Config.m_iIsScanMainImei==1)
		{
			if(!this->ReadSimpleIMEI_Ex())
				return FALSE;
		}
		else
		{
			if(!this->ReadFourIMEI_Ex())
				return FALSE;
		}
		break;	
	}
	
	if (m_iPlatform == 0 || m_iPlatform == 1)
	{
		IMEISV_struct_T IMEISV;
		METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,10,1,&IMEISV);	
	}
	
	return TRUE;
}


bool CColorBoxView::ReadSimpleIMEI_Ex()
{
	FASTNVACCESS_T      nvparam;
	ZeroMemory((void *)&nvparam, sizeof(nvparam));
	IMEISV_struct_T IMEISV;
	CString temp;
	
	if (m_iPlatform == 0 || m_iPlatform == 1)
	{
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,1,&IMEISV);
		
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI Error...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI Pass...",0);
			this->m_csIMEI1_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
	}
	else if (m_iPlatform == 2)
	{
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI1;
		
		nErrorCode = SP_FastReadNvInfo (g_hDiagPhone, &nvparam);
		
		if(RFT_SUCCESS != nErrorCode)
		{
			InsertListInfo("Read IMEI Error...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI Pass...",0);
			this->m_csIMEI1_Ex=nvparam.szImei1;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
	}
	return TRUE;
}

bool CColorBoxView::ReadDoubleIMEI_Ex()
{
	FASTNVACCESS_T      nvparam;
	ZeroMemory((void *)&nvparam, sizeof(nvparam));
	IMEISV_struct_T IMEISV;
	CString temp;
	
	if (m_iPlatform == 0 || m_iPlatform == 1)
	{
		//IMEI1
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,1,&IMEISV);
		
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI1 Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI1 Pass...",0);
			this->m_csIMEI1_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
		
		//IMEI2
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,2,&IMEISV);
		
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI2 Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI2 Pass...",0);
			this->m_csIMEI2_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
	}
	else if (m_iPlatform == 2)
	{
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI1;
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI2;
		
		nErrorCode = SP_FastReadNvInfo (g_hDiagPhone, &nvparam);
		
		if(RFT_SUCCESS != nErrorCode)
		{
			InsertListInfo("Read IMEI Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI1 Pass...",0);
			this->m_csIMEI1_Ex=nvparam.szImei1;
			InsertListInfo("Read IMEI2 Pass...",0);
			this->m_csIMEI2_Ex=nvparam.szImei2;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
	}
	
	return TRUE;
}


bool CColorBoxView::ReadThreeIMEI_Ex()
{
	FASTNVACCESS_T      nvparam;
	ZeroMemory((void *)&nvparam, sizeof(nvparam));
	IMEISV_struct_T IMEISV;
	CString temp;
	
	if (m_iPlatform == 0 || m_iPlatform == 1)
	{
		//IMEI1
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,1,&IMEISV);
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI1 Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI1 Pass...",0);
			this->m_csIMEI1_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
		
		//IMEI2
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,2,&IMEISV);
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI2 Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI2 Pass...",0);
			this->m_csIMEI2_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
		
		//IMEI3
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,3,&IMEISV);
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI3 Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI3 Pass...",0);
			this->m_csIMEI3_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
	}
	else if (m_iPlatform == 2)
	{
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI1;
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI2;
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI3;
		
		nErrorCode = SP_FastReadNvInfo (g_hDiagPhone, &nvparam);
		
		if(RFT_SUCCESS != nErrorCode)
		{
			InsertListInfo("Read IMEI Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI1 Pass...",0);
			this->m_csIMEI1_Ex=nvparam.szImei1;
			InsertListInfo("Read IMEI2 Pass...",0);
			this->m_csIMEI2_Ex=nvparam.szImei2;
			InsertListInfo("Read IMEI3 Pass...",0);
			this->m_csIMEI3_Ex=nvparam.szImei3;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
	}
	
	return TRUE;
}


bool CColorBoxView::ReadFourIMEI_Ex()
{
	FASTNVACCESS_T      nvparam;
	ZeroMemory((void *)&nvparam, sizeof(nvparam));
	IMEISV_struct_T IMEISV;
	CString temp;
	
	if (m_iPlatform == 0 || m_iPlatform == 1)
	{
		//IMEI1
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,1,&IMEISV);
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI1 Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI1 Pass...",0);
			this->m_csIMEI1_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
		
		//IMEI2
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,2,&IMEISV);
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI2 Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI2 Pass...",0);
			this->m_csIMEI2_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
		
		//IMEI3
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,3,&IMEISV);
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI3 Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI3 Pass...",0);
			this->m_csIMEI3_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
		
		//IMEI4
		m_MetaappResult=METAAPP_readIMEIfromNVRAM_Ex3(m_nMetaHandle,5000,4,&IMEISV);
		if(m_MetaappResult!=METAAPP_SUCCESS)   
		{
			InsertListInfo("Read IMEI4 Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI4 Pass...",0);
			this->m_csIMEI4_Ex=IMEISV.imei;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
	}
	else if (m_iPlatform == 2)
	{
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI1;
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI2;
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI3;
		nvparam.dwOperMask |= FNAMASK_WRITE_IMEI4;
		
		nErrorCode = SP_FastReadNvInfo (g_hDiagPhone, &nvparam);
		
		if(RFT_SUCCESS != nErrorCode)
		{
			InsertListInfo("Read IMEI Fail...",2);
			return FALSE;
		}
		else
		{
			InsertListInfo("Read IMEI1 Pass...",0);
			this->m_csIMEI1_Ex=nvparam.szImei1;
			InsertListInfo("Read IMEI2 Pass...",0);
			this->m_csIMEI2_Ex=nvparam.szImei2;
			InsertListInfo("Read IMEI3 Pass...",0);
			this->m_csIMEI3_Ex=nvparam.szImei3;
			InsertListInfo("Read IMEI4 Pass...",0);
			this->m_csIMEI4_Ex=nvparam.szImei4;
			SendMessage(WM_UPDATEDATA, FALSE, FALSE);
		}
	}
	
	return TRUE;
}


bool CColorBoxView::Read_Mode_Check_Database()
{
	switch (this->m_iIMEIType)
	{
	case 0://单IMEI
		if(!Read_Mode_Check_Database_1IMEI())
			return FALSE;
		else
			return TRUE;
		break;
	case 1://双IMEI
		//判断是否是扫描主IMEI号
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(!Read_Mode_Check_Database_2IMEI())
				return FALSE;
			else
				return TRUE;
		}
		else
		{
			if(!Read_Mode_Check_Database_IMEI_Ex())
				return FALSE;
			else
				return TRUE;
		}
		break;
	case 2://三IMEI
		//判断是否是扫描主IMEI号
		if(this->m_Config.m_iIsScanMainImei==0)
		{			
			if(!Read_Mode_Check_Database_3IMEI())
				return FALSE;
			else
				return TRUE;
		}
		else
		{
			if(!Read_Mode_Check_Database_IMEI_Ex())
				return FALSE;
			else
				return TRUE;
		}
		break;
	case 3://四IMEI
		//判断是否是扫描主IMEI号
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(!Read_Mode_Check_Database_4IMEI())
				return FALSE;
			else
				return TRUE;
		}
		else
		{
			if(!Read_Mode_Check_Database_IMEI_Ex())
				return FALSE;
			else
				return TRUE;
		}
		break;
	default:
		break;
	}

	return TRUE;
}


bool CColorBoxView::Read_Mode_Check_Database_1IMEI()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;
	
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and (Status=2 or Status=3 or Status=4) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1);  //@@ Call storage course @@
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			m_checkColorDone=TRUE;
			InsertListInfo("IMEI1 is Already Exist in GiftBox Data Server...",1);
		}
		else//彩盒数据库内无记录
		{
			m_checkColorDone=FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("UnKnown Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}	
	
	return TRUE;
}

bool CColorBoxView::Read_Mode_Check_Database_2IMEI()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') AND (IMEI2='%s') and (Status=3 or Status=4 or Status=2) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1,(char*)(LPCTSTR)m_csIMEI2);  //@@ Call storage course @@
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			m_checkColorDone=TRUE;
			InsertListInfo("IMEI1 IMEI2 Are Already Exist In GiftBox Server Fail...",1);
		}
		else
		{
			m_checkColorDone=FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}	
	
	return TRUE;	
}


bool CColorBoxView::Read_Mode_Check_Database_IMEI_Ex()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;
	
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and (Status=3 or Status=4 or Status=2) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1);  //@@ Call storage course @@
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			m_checkColorDone=TRUE;
			InsertListInfo("IMEI1 Is Already Exist In GiftBox Server Fail...",1);
		}
		else
		{
			m_checkColorDone=FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}	
	
	return TRUE;	
}



bool CColorBoxView::Read_Mode_Check_Database_3IMEI()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') AND (IMEI2='%s') AND (IMEI3='%s') and (Status=3 or Status=4 or Status=2) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1,(char*)(LPCTSTR)m_csIMEI2,(char*)(LPCTSTR)m_csIMEI3);  //@@ Call storage course @@
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			m_checkColorDone=TRUE;
			InsertListInfo("IMEI1 IMEI2 IMEI3 Are Already Exist In GiftBox Server Fail...",1);
		}
		else
		{
			m_checkColorDone=FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}	

	return TRUE;	
}


bool CColorBoxView::Read_Mode_Check_Database_4IMEI()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("select * from %s_%s_ColorBox where (IMEI1='%s') AND (IMEI2='%s') AND (IMEI3='%s') AND (IMEI4='%s') and (Status=3 or Status=4 or Status=4) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1,(char*)(LPCTSTR)m_csIMEI2,(char*)(LPCTSTR)m_csIMEI3,(char*)(LPCTSTR)m_csIMEI4);  //@@ Call storage course @@
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			m_checkColorDone=TRUE;
			InsertListInfo("IMEI1 IMEI2 IMEI3 IMEI4 Are Already Exist In GiftBox Server Fail...",1);
		}
		else
		{
			m_checkColorDone=FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unkown Known...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}	
	
	return TRUE;	
}


void CColorBoxView::OnUpdateData(WPARAM wParam, LPARAM lParam) 
{ 
	UpdateData(wParam); 
}


bool CColorBoxView::SP_Enter_Modem_Meta_by_USB()
{
	char * m_exist_symbolic_set[20]={"\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0"};
	char *m_exist_port_set[20]={"\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0","\0"};
	static const GUID  Guid_ANDROID_USB2Ser={0xA5DCBF10L,0x6530,0x11D2,{0x90,0x1F,0x00,0xC0,0x4F,0xB9,0x51,0xED}};
	static const GUID GUID_PORT_CLASS_USB2SER ={0x4D36E978L,0xE325,0x11CE,{0xBF,0xC1,0x08,0x00,0x2B,0xE1,0x03,0x18}};
	int usb_port=0;
	META_RESULT eMeta_Result;
	unsigned short us_com_port;
	META_ConnectByUSB_Req usbRequest;
	META_ConnectByUSB_Report usbReport;
	unsigned int device_num=0;
	signed int bootstop;
	int BootStop=0;
	
	//########### META connection flow ###############
	SP_BOOT_ARG_S stArg;
	int iRet;
	int eType;  //0=preloader,1=bootrom
	char * ppFilter[2];
	SP_COM_FILTER_LIST_S sCOMFilter;
	SP_COM_PROPERTY_S sCOMProperty;
	
	memset (&usbRequest, 0x0, sizeof(META_ConnectByUSB_Req));
	memset (&usbReport, 0x0, sizeof(META_ConnectByUSB_Report));
	memset (&stArg, 0x0, sizeof(SP_BOOT_ARG_S));
	
	//Old parameters
   	stArg.m_bbchip_type = SP_AUTO_DETECT_BBCHIP;
	stArg.m_ext_clock = SP_AUTO_DETECT_EXT_CLOCK;
   	stArg.m_ms_boot_timeout = SP_BOOT_INFINITE;
	stArg.m_max_start_cmd_retry_count = SP_DEFAULT_BROM_START_CMD_RETRY_COUNT;
	
	//New parameters
	stArg.m_uTimeout = 20000;
	stArg.m_uRetryTime = 2000;
	stArg.m_uInterval = 10;
   	stArg.m_uBaudrate = CBR_115200;
	
	//=============================
	// Serial Link Authentication
	//stArg.m_auth_handle = SPATE_Get_AuthHandle();
	//stArg.m_scert_handle = SPATE_Get_ScertHandle();
	stArg.m_cb_sla_challenge = SLA_Challenge;
   	stArg.m_cb_sla_challenge_arg = NULL;
   	stArg.m_cb_sla_challenge_end = SLA_Challenge_END;
	stArg.m_cb_sla_challenge_end_arg = NULL;
	
	//1. Get META arguments
	//frmMainSel->SetMETAArg();
	//memcpy(&stArg, frmMainSel->GetMETAArg(), sizeof(SP_BOOT_ARG_S));
	stArg.m_pStopFlag = &BootStop;
	BootStop = SP_BOOT_INFINITE;
	stArg.m_bIsUSBEnable = 1/*temp_com_port == USB_COM_PORT?1:0*/;
	stArg.m_bIsSymbolicEnable = false;
	stArg.m_bIsCompositeDeviceEnable = 0;
	stArg.m_euBootMode = SP_META_BOOT;
	
	//2. Set COM port for META connection
	char asPreloaderPortFilter[128],asBROMPortFilter[128];
	sprintf(asPreloaderPortFilter,"%s","VID_0E8D&PID_2000");
	sprintf(asBROMPortFilter,"%s","VID_0E8D&PID_0003");
	
	ppFilter[0] = asPreloaderPortFilter;
	ppFilter[1] = asBROMPortFilter;
	sCOMFilter.m_eType=SP_WHITE_LIST;
	sCOMFilter.m_uCount =1/* (LegacyAutoDetection) ? 1: 2*/;
	sCOMFilter.m_ppFilterID = ppFilter;
	
	iRet = SP_GetIncrementCOMPortWithFilter(&sCOMFilter, &sCOMProperty, NULL, true,&BootStop,30);
	Sleep(1000);
	
	if(0 == iRet)
	{
		switch(sCOMProperty.m_iFilterIndex)
		{
		case 0:
			eType = 0/*PreloaderUSB*/;
			break;
		case 1:
			eType = 1/*BootROMUSB*/;
			break;
		default:
			return false;
		}
		
		stArg.m_uPortNumber = sCOMProperty.m_uNumber;
	}
	else
	{
		return false;
	}
	
	//3. Do META connection
	if(1 == eType)
	{   
		iRet = SP_BootROM_BootMode(&stArg);
	}
	else if(0 == eType)
	{
		iRet = SP_Preloader_BootMode(&stArg);
	}
	
   	if(iRet != SP_S_DONE)//
   	{
		return false;
   	}
	else
	{
		memset (&sCOMFilter, 0x0, sizeof(SP_COM_FILTER_LIST_S));
		memset (&sCOMProperty, 0x0, sizeof(SP_COM_PROPERTY_S));
		BootStop=SP_BOOT_INFINITE;
		
		ppFilter[0] = asPreloaderPortFilter;
		ppFilter[1] = asBROMPortFilter;
		sCOMFilter.m_uCount = 0;//1;
		sCOMFilter.m_ppFilterID = ppFilter;
		sCOMFilter.m_eType=SP_BLACK_LIST;
		
		iRet = SP_GetIncrementCOMPortWithFilter(&sCOMFilter, &sCOMProperty, &GUID_PORT_CLASS_USB2SER, 0, &BootStop, 100/*SearchKernelPortTimeOut*/);
		switch(iRet)
		{
		case SP_S_DONE:
			us_com_port = sCOMProperty.m_uNumber;
			break;
		case SP_S_STOP:
			//search_kernel_port_success=0;
			return false;
		case SP_S_TIMEOUT:
			{
				InsertListInfo("Search Kernel USB COM Port TimeOut!",2);
				return false;
			}
		default:
			{
				InsertListInfo("Search Kernel USB COM Port Fail!",2);
				return false;
			}
		}
		
		Sleep(1000);
		
		if (iRet == SP_S_DONE)
		{
			usbRequest.com_port = us_com_port;
			usbRequest.ms_connect_timeout = 80000;
			eMeta_Result = META_ConnectInMetaModeByUSB_r (this->m_nMetaHandle, &usbRequest, &bootstop, &usbReport);
		}
	}
	
	this->m_usb_port=usbRequest.com_port;
	
	switch (eMeta_Result)
	{
	case META_SUCCESS:
	case META_MAUI_DB_INCONSISTENT:
		return TRUE;
		break;
	case META_INCORRECT_TARGET_VER:
		
		InsertListInfo("Meta Warning,Modem NVRAM database inconsistent with target...",2);
		break;
		
	case META_COMM_FAIL:
		
		InsertListInfo("Meta Error,Open COM port fail...",2);
		break;
		
	case META_COM_ERROR:
		
		InsertListInfo("Meta Error,COM port doesn't exist...",2);
		break;
		
	case META_WAIT_FOR_TARGET_READY_TIMEOUT:
		
		InsertListInfo("Meta Error,Connecting timeout...",2);
		InsertListInfo("May be incorrect modem database!",2);
		break;
		
	case META_FAILED:
		
		InsertListInfo("Meta Error,Can't connect to target.",2);
		InsertListInfo("Please Enable Meta Dll.Log to get detail cause...",2);
		break;
		
	default:
		break;
    }
	return false;
}


bool CColorBoxView::SP_Enter_Modem_Meta_by_UART()
{
    int bootstop = 0;
    META_ConnectInMETA_Req BootRequest;
    int BootStop=0;
    META_ConnectInMETA_Report BootReport;
    META_RESULT mr;
	
    //########### META connection flow ###############
    SP_BOOT_ARG_S stArg;
    int iRet;
    int eType;
	
    memset (&stArg, 0x0, sizeof(SP_BOOT_ARG_S));
	
    //---------------------Connect with target ----------------------------------
    BootRequest.com_port = m_comport;
    // [META] META stage baudrate polling array, it must end with META_BAUD_END.
    BootRequest.baudrate[0] = META_BAUD115200;
    BootRequest.baudrate[1] = META_BAUD230400;
    BootRequest.baudrate[2] = META_BAUD460800;
    BootRequest.baudrate[3] = META_BAUD921600;
    BootRequest.baudrate[4] = META_BAUD57600;
    BootRequest.baudrate[5] = META_BAUD_END;
    BootRequest.flowctrl    = META_SW_FLOWCTRL;
    BootRequest.ms_connect_timeout = 90000;  //30000;
	
    //Old parameters
    stArg.m_bbchip_type = SP_AUTO_DETECT_BBCHIP;
    stArg.m_ext_clock = SP_AUTO_DETECT_EXT_CLOCK;
    stArg.m_ms_boot_timeout = SP_BOOT_INFINITE;
    stArg.m_max_start_cmd_retry_count = SP_DEFAULT_BROM_START_CMD_RETRY_COUNT;
	
    //New parameters
    stArg.m_uTimeout = 40000;
    stArg.m_uRetryTime = 4000;
    stArg.m_uInterval = 10;
    stArg.m_uBaudrate = CBR_115200;
	
	//=============================
	
    stArg.m_cb_sla_challenge = SLA_Challenge;
    stArg.m_cb_sla_challenge_arg = NULL;
    stArg.m_cb_sla_challenge_end = SLA_Challenge_END;
    stArg.m_cb_sla_challenge_end_arg = NULL;
	
    //1. Get META arguments
    stArg.m_pStopFlag = &BootStop;
    BootStop = SP_BOOT_INFINITE;
    stArg.m_bIsUSBEnable = 1/*temp_com_port == USB_COM_PORT?1:0*/;
    stArg.m_bIsSymbolicEnable = false;
    stArg.m_bIsCompositeDeviceEnable = 0;
    stArg.m_euBootMode = SP_META_BOOT;
	
    //2. Set COM port for META connection
    //Use UART port, unnecessary to scan
    stArg.m_uPortNumber = this->m_comport;
    eType =0/* PreloaderUSB*/;
	
    //3. Do META connection
    if(1/*BootROMUSB*/ == eType)
    {
		iRet = SP_BootROM_BootMode(&stArg);
    }
    else if(0/*PreloaderUSB*/ == eType)
    {
		iRet = SP_Preloader_BootMode(&stArg);
    }
	
    if(iRet != SP_S_DONE)//1)
    {
        //SP DUT not boot into META
        /*MetaResult = -1;
        enter_count = META_Retry;*/
		return false;
    }
    else
    {
		mr = META_ConnectInMetaMode_r(this->m_nMetaHandle, &BootRequest, &bootstop, &BootReport);
    }
	
	switch (mr)
	{
	case META_SUCCESS:
	case META_MAUI_DB_INCONSISTENT:
		return TRUE;
		break;
		
	case META_INCORRECT_TARGET_VER:
		InsertListInfo("Meta Warning,Modem NVRAM database inconsistent with target...",2);
		break;
		
	case META_COMM_FAIL:
		InsertListInfo("Meta Error,Open COM port fail...",2);
		break;
		
	case META_COM_ERROR:
		InsertListInfo("Meta Error,COM port doesn't exist...",2);
		break;
		
	case META_WAIT_FOR_TARGET_READY_TIMEOUT:
		InsertListInfo("Meta Error,Connecting timeout...",2);
		InsertListInfo("May be incorrect modem database!",2);
		break;
		
	case META_FAILED:
		InsertListInfo("Meta Error,Can't connect to target.",2);
		InsertListInfo("Please Enable Meta Dll.Log to get detail cause...",2);
		break;
		
	default:
		break;
    }
	return false;
}


//断开MODEM META
bool CColorBoxView::DisconnectInMetaMode()
{
	m_MetaResult = META_DisconnectInMetaMode_r(this->m_nMetaHandle);
	if(m_MetaResult != METAAPP_SUCCESS)
	{
		return FALSE;
	}
	return TRUE;
}

//断开AP META
bool CColorBoxView::DisconnectInApMeta()
{
	if(!this->m_ApExe.AP_META_DisconnectInMetaMode_r())
	{
		return FALSE;
	}
	return TRUE;
}

void CColorBoxView::SP_Exit_MetaModemMode()
{	
	METAAPP_DisconnectWithTarget(this->m_nMetaHandle);
	this->m_ApExe.Switch2SP_META_Mode(this->m_iKernelComport);
	this->m_ApExe.Exit_MetaAPMode();	
}


bool CColorBoxView::Switch_AP_TO_MODEM_META_Mode(int comport)
{
	CString text;
	m_ctrComboxComPort.GetLBText(m_ctrComboxComPort.GetCurSel(),text);	
	
    META_RESULT MetaResult;
	META_ConnectInMETA_Req BootRequest;
	int BootStop=0;
	META_ConnectInMETA_Report BootReport;
	META_ConnectByUSB_Req usbRequest;
    META_ConnectByUSB_Report usbReport;
	
	//---------------------Connect with target ----------------------------------
	BootRequest.com_port = comport;
	// [META] META stage baudrate polling array, it must end with META_BAUD_END.
	BootRequest.baudrate[0] = META_BAUD115200;
	BootRequest.baudrate[1] = META_BAUD230400;
	BootRequest.baudrate[2] = META_BAUD460800;
	BootRequest.baudrate[3] = META_BAUD921600;
	BootRequest.baudrate[4] = META_BAUD57600;
	BootRequest.baudrate[5] = META_BAUD_END;
	
	BootRequest.flowctrl = META_SW_FLOWCTRL;
	BootRequest.ms_connect_timeout = 80000; //30000;
	
	if(text=="USB")
	{
		usbRequest.com_port = comport;
		usbRequest.ms_connect_timeout =8000;
		MetaResult = META_ConnectInMetaModeByUSB_r(this->m_nMetaHandle, &usbRequest, &BootStop, &usbReport);
	}
	else
	{
        MetaResult = META_ConnectInMetaMode_r(this->m_nMetaHandle, &BootRequest, &BootStop, &BootReport);
	}
	
	if(META_SUCCESS != MetaResult && META_MAUI_DB_INCONSISTENT != MetaResult)
		return FALSE;
	
	return TRUE;
}


BOOL CColorBoxView::IMEITYPE_0_CheckImei1_Ex()
{
	//检测长度是否为15位
	if(m_csIMEI1.GetLength()!=15)
	{
		InsertListInfo("IMEI1 Length Error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI1.Left(14)>this->m_imei1_end || m_csIMEI1.Left(14)<this->m_imei1_start)
		{
			InsertListInfo("IMEI1 out of Range fail...",2);
			this->m_ctrIMEI1.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI1='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI1);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("is ot in the IMEI table...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}

	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI1.Left(14),calculate_digit);

	if(calculate_digit!=m_csIMEI1.Right(1))	
	{
		InsertListInfo("Illegal IMEI1,Parity error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

 	CString temp,temp2;
 	_variant_t var;
 	CString sql;
 	try
 	{
 		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and Status IS NOT NULL",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,(char*)(LPCTSTR)m_csIMEI1);
 		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
 			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
 		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
 		if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
 		{
			InsertListInfo("This IMEI1 have not done write IMEI...",2);
 			return FALSE;
 		}
 		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
 			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
 	}
 	catch(_com_error e)
 	{
 		InsertListInfo("Unknown Error...",2);
 		this->m_ctrIMEI1.SetSel(0,-1);
 		return FALSE;
 	}
 
	return TRUE;
}

BOOL CColorBoxView::IMEITYPE_1_CheckImei2_Ex()
{
	if(m_csIMEI1==m_csIMEI2)
	{
		InsertListInfo("IMEI1 and IMEI2 cannot be the same Fail...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}
	
	//检测长度是否为15位
	if(m_csIMEI2.GetLength()!=15)
	{
		InsertListInfo("IMEI2 Length is not 15 Fail...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI2.Left(14)>m_imei2_end || m_csIMEI2.Left(14)<m_imei2_start)
		{
			InsertListInfo("IMEI2 iS Out Of Range Fail...",2);
			this->m_ctrIMEI2.SetSel(0,-1);
			return FALSE;
		}
	}
	else if(m_iIMEIRangeType==1)
	{
		_variant_t var;
		CString sql;

		sql.Format("select * from %s_%s_IMEIRange WHERE IMEI2='%s'", \
			(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder,m_csIMEI2);
		
		try
		{
			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
			((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI2 Is Not In The Server Table Fail...",2);
				return FALSE;
			}

			if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
				((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI2校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI2.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI2.Right(1))
	{
		InsertListInfo("Error IMEI,The 15th Imei Error Fail...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}


BOOL CColorBoxView::Check_Read_and_Scan_IMEI()
{
	switch (this->m_iIMEIType)
	{
	case 0://单IMEI
		if(this->m_csIMEI1 == this->m_csIMEI1_Ex)
			return TRUE;
		else
			return FALSE;
		break;
	case 1://双IMEI
		//判断是否扫描主IMEI号
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(this->m_csIMEI1 == this->m_csIMEI1_Ex && this->m_csIMEI2 == this->m_csIMEI2_Ex)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if(this->m_csIMEI1 == this->m_csIMEI1_Ex)
				return TRUE;
			else
				return FALSE;
		}
		break;
	case 2://三IMEI
		//判断是否扫描主IMEI号
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(this->m_csIMEI1 == this->m_csIMEI1_Ex && this->m_csIMEI2 == this->m_csIMEI2_Ex  && this->m_csIMEI3 == this->m_csIMEI3_Ex)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if(this->m_csIMEI1 == this->m_csIMEI1_Ex)
				return TRUE;
			else
				return FALSE;
		}
		break;
	case 3://四IMEI
		//判断是否扫描主IMEI号
		if(this->m_Config.m_iIsScanMainImei==0)
		{
			if(this->m_csIMEI1 == this->m_csIMEI1_Ex && this->m_csIMEI2 == this->m_csIMEI2_Ex  && this->m_csIMEI3 == this->m_csIMEI3_Ex && this->m_csIMEI4 == this->m_csIMEI4_Ex)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if(this->m_csIMEI1 == this->m_csIMEI1_Ex)
				return TRUE;
			else
				return FALSE;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

void CColorBoxView::OnSetMyFocus(WPARAM wParam, LPARAM lParam)
{
	CEdit* a;
	a=(CEdit*)wParam;
	a->SetFocus();
}

bool CColorBoxView::GetProductOrderInfo()
{
	CString temp;
	_variant_t var;
	CString sql;

	try
	{
		sql.Format("select * from ProductList where Enable=1 and OrderName='%s'",m_csCurrentOrder);
		
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("ProductName");
			temp=VariantToCString(var);
			this->m_csCurrentProduct= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEIType");
			this->m_iIMEIType=var.intVal;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEIRangeType");
			this->m_iIMEIRangeType=var.intVal;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI1_STR");
			temp=VariantToCString(var);
			this->m_imei1_start= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI1_END");
			temp=VariantToCString(var);
			this->m_imei1_end= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI2_STR");
			temp=VariantToCString(var);
			this->m_imei2_start= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI2_END");
			temp=VariantToCString(var);
			this->m_imei2_end= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI3_STR");
			temp=VariantToCString(var);
			this->m_imei3_start= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI3_END");
			temp=VariantToCString(var);
			this->m_imei3_end= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI4_STR");
			temp=VariantToCString(var);
			this->m_imei4_start= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("IMEI4_END");
			temp=VariantToCString(var);
			this->m_imei4_end= temp;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("Color");
			temp=VariantToCString(var);
			this->m_csCurrentColor= temp;
			
		}
		else
		{
			return  FALSE;
		}

		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}
	
	return TRUE;
}

bool CColorBoxView::Load_Current_Product_Count()
{
	CString temp,states;
	_variant_t var;
	CString sql;

	sql.Format("select count(*) as COUNT from %s_%s_ColorBox where (Status=1 or Status=2 or Status=3 or Status=4) and Enable=1",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder);

	try
	{
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("COUNT");
			temp=VariantToCString(var);
			this->m_csCurrentCount= temp;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}

	return TRUE;
}

void CColorBoxView::UpdateCount(CString strCount)
{
	CString temp;
	temp="Total:"+strCount; 
	this->m_cCurrentCount.SetWindowText(temp);
	UpdateData(FALSE);
}

void CColorBoxView::OnScanMainImeiYes() 
{
	CPassword dlg;
	dlg.ilevel = 1; //需超级权限验证
	if(dlg.DoModal()!=IDOK)
		return;

	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_YES, MF_BYCOMMAND | MF_CHECKED );
	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_NO, MF_BYCOMMAND | MF_UNCHECKED ); 
	this->m_Config.m_iIsScanMainImei=1;
	this->m_Config.SaveConfig();
}

void CColorBoxView::OnScanMainImeiNo()
{
	CPassword dlg;
	dlg.ilevel = 1; //需超级权限验证
	if(dlg.DoModal()!=IDOK)
		return;

	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_YES, MF_BYCOMMAND | MF_UNCHECKED );
	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_MAIN_IMEI_NO, MF_BYCOMMAND | MF_CHECKED ); 
	this->m_Config.m_iIsScanMainImei=0;
	this->m_Config.SaveConfig();
}

void CColorBoxView::OnBtnClean() 
{
	this->CleanImeiWindow();
}

void CColorBoxView::CleanImeiWindow() 
{
	this->ClearUI();
		
	switch(this->m_iIMEIType) 
	{
		case 0:
			this->m_ctrIMEI1.EnableWindow(FALSE);
			this->m_ctrIMEI2.EnableWindow(FALSE);
			this->m_ctrIMEI3.EnableWindow(FALSE);
			this->m_ctrIMEI4.EnableWindow(FALSE);

			this->m_ctrIMEI1.EnableWindow(TRUE);
			break;
		case 1:
			this->m_ctrIMEI1.EnableWindow(FALSE);
			this->m_ctrIMEI2.EnableWindow(FALSE);
			this->m_ctrIMEI3.EnableWindow(FALSE);
			this->m_ctrIMEI4.EnableWindow(FALSE);
	
			this->m_ctrIMEI1.EnableWindow(TRUE);
			this->m_ctrIMEI2.EnableWindow(TRUE);
			break;
		case 2:
			this->m_ctrIMEI1.EnableWindow(FALSE);
			this->m_ctrIMEI2.EnableWindow(FALSE);
			this->m_ctrIMEI3.EnableWindow(FALSE);
			this->m_ctrIMEI4.EnableWindow(FALSE);

			this->m_ctrIMEI1.EnableWindow(TRUE);
			this->m_ctrIMEI2.EnableWindow(TRUE);
			this->m_ctrIMEI3.EnableWindow(TRUE);
			break;
		case 3:
			this->m_ctrIMEI1.EnableWindow(FALSE);
			this->m_ctrIMEI2.EnableWindow(FALSE);
			this->m_ctrIMEI3.EnableWindow(FALSE);
			this->m_ctrIMEI4.EnableWindow(FALSE);

			this->m_ctrIMEI1.EnableWindow(TRUE);
			this->m_ctrIMEI2.EnableWindow(TRUE);
			this->m_ctrIMEI3.EnableWindow(TRUE);
			this->m_ctrIMEI4.EnableWindow(TRUE);
			break;
		default:
			break;
	}
	this->m_ctrIMEI1.SetFocus();	
}


BOOL CColorBoxView::GetSetting()
{
	CString temp;
	_variant_t var;
	CString sql;
	int itemp;

	try
	{
		sql.Format("select * from SettingList where Enable=1 and ProductName='%s' and OrderName='%s'",this->m_csCurrentProduct,this->m_csCurrentOrder);
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
		((CColorBoxApp *)AfxGetApp())->m_pRst=((CColorBoxApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		
		if(!((CColorBoxApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("Platform");
			this->m_iPlatform=var.intVal;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("BTFT");
			this->m_iBTFT=var.intVal;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("ResetFactory");
			this->m_iResetFactory=var.intVal;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("CheckVersion");
			this->m_iCheckVersion=var.intVal;

			var = ((CColorBoxApp *)AfxGetApp())->m_pRst->GetCollect("VersionCode");
			temp=VariantToCString(var);
			this->m_csVersion = temp;

		}
		else
		{
			return FALSE;
		}
		if(((CColorBoxApp *)AfxGetApp())->m_pRst->State)
			((CColorBoxApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}

	return TRUE;
}


void CColorBoxView::ShowCalResult(int state)
{
	switch(state) 
	{
		case IDLE:
			this->m_cResult.SetTextColor(RGB(0,128,0));
			this->m_cResult.SetWindowText("IDLE");
			break;
		case FAIL:
			this->m_cResult.SetTextColor(RGB(255,0,0));
			this->m_cResult.SetWindowText("FAIL");
			break;
		case PASS:
			this->m_cResult.SetTextColor(RGB(0,128,0));
			this->m_cResult.SetWindowText("PASS");
			break;
		case Testing:
			this->m_cResult.SetTextColor(RGB(0,0,255));
			this->m_cResult.SetWindowText("Testing");
			break;
		case InitFail:
			this->m_cResult.SetTextColor(RGB(255,0,0));
			this->m_cResult.SetWindowText("InitFail");
		default:
			break;
	}
}


//开机时是否需要执行恢复出厂设置操作写入Barcode第22位: 1--是
BOOL CColorBoxView::SP_WriteResetFactory2Barcode()
{
	this->m_csPcbaSN.SetAt(21,'1');
	
	m_MetaappResult = METAAPP_writeBarcodetoNVRAM(m_nMetaHandle,(LPSTR)(LPCTSTR)this->m_csPcbaSN,3000);
	if(m_MetaappResult != METAAPP_SUCCESS)   
	{
		return FALSE;
	}
	
	return TRUE;
}

//20150728 glory add follow function

int CColorBoxView::ConnectWithPreloader()
{
	CString sInfo;
    this->Display("SmartPhoneSN::ConnectWithPreloader(): Start to get dynamic preloader comport...", 3);

    SP_COM_FILTER_LIST_S sCOMFilter;
    SP_COM_PROPERTY_S sCOMProperty;   
    memset(&sCOMFilter, 0, sizeof(SP_COM_FILTER_LIST_S));
    memset(&sCOMProperty, 0, sizeof(SP_COM_PROPERTY_S));

    int iRet = 0;
    unsigned int eType;   //0:BoorROMUSB,1:PreloaderUSB
    char *ppFilter[2];    

    char *asPreloaderPortFilter = g_sMetaComm.sPortFilter.strPreloaderFilter;
    char *asBROMPortFilter = g_sMetaComm.sPortFilter.strBromFilter;

    ppFilter[0]=asPreloaderPortFilter;
	ppFilter[1]=asBROMPortFilter;
   
    sCOMFilter.m_uCount=2;
    sCOMFilter.m_ppFilterID=ppFilter;
	sCOMFilter.m_eType=SP_WHITE_LIST;

    if (g_sMetaComm.bUsbEnable == false)
    {
        m_stModeArg.m_uPortNumber = g_sMetaComm.iCOMPort;
        eType = PreloaderUSB;
    } 
    else
    {     
        int iTimeout = m_stModeArg.m_uTimeout / 1000;  //timeout unit: s, but m_stModeArg.m_uTimeout unit is ms
        iRet = SP_GetIncrementCOMPortWithFilter(&sCOMFilter, &sCOMProperty, NULL, _TRUE, m_pMetaStopFlag, 30);              
        if(0 == iRet)
        {      
            m_bStopBeforeUSBInsert = false;
            switch(sCOMProperty.m_iFilterIndex)
            {
            case 0:
                eType = PreloaderUSB;
                break;

            case 1:
                eType = BootROMUSB;
                break;

            default:
                //MessageBox (NULL, "Filter index error!", "fail", MB_OK);
                return SPMETA_DLL::META_FAILED;
            }
            
            m_stModeArg.m_uPortNumber = sCOMProperty.m_uNumber;
        }
        else
        {
            if (iRet == SP_S_TIMEOUT)
            {
                this->Display("SmartPhoneSN::ConnectWithPreloader(): Get dynamic preloader comport timeout...");                                
            }

            return iRet;
        }
    }
   
    Sleep (0.01);
    if(BootROMUSB == eType)
    {
        sInfo.Format("SmartPhoneSN::ConnectWithPreloader(): Get preloader comport successfully, comport = %d", m_stModeArg.m_uPortNumber);
		this->Display(sInfo);
        iRet = SP_BootROM_BootMode(&m_stModeArg);       
        if (iRet == 0)
        {
            this->Display("SmartPhoneSN::SP_BootROM_BootMode(): Preloader boot meta mode success!!", 3);
        }   
        else
        {
            sInfo.Format("SmartPhoneSN::SP_BootROM_BootMode(): Preloader boot meta mode Fail, Err = %d", iRet);
			this->Display(sInfo);
        }

        m_bStopBeforeUSBInsert = false;
    }
    else if(PreloaderUSB == eType)
    {
        sInfo.Format("SmartPhoneSN::ConnectWithPreloader(): Get preloader comport successfully, comport = %d", m_stModeArg.m_uPortNumber);
		this->Display(sInfo);
        iRet = SP_Preloader_BootMode(&m_stModeArg);
        if (iRet == 0)
        {
            this->Display("SmartPhoneSN::SP_Preloader_BootMode(): Preloader boot meta mode success!!");
        }   
        else
        {
            sInfo.Format("SmartPhoneSN::SP_Preloader_BootMode(): Preloader boot meta mode Fail, Err = %d", iRet);
			this->Display(sInfo);
        }

        m_bStopBeforeUSBInsert = false;
    }
    
   
    return iRet;
}

int CColorBoxView::ConnectWithKernelPort_Ex(SP_METAMODE_e spMetaMode)
{
	CString sInfo;
    int iRet = 0;
    unsigned short kernelComport = 0; 
    char *pFuncName = NULL;
    
    SPMETA_DLL::META_ConnectByUSB_Req     spUSBConnReq;
    SPMETA_DLL::META_ConnectByUSB_Report  spUSBConnReport;   
    memset(&spUSBConnReq, 0, sizeof(spUSBConnReq));
    memset(&spUSBConnReport, 0, sizeof(spUSBConnReport));
    
    SP_COM_FILTER_LIST_S sCOMFilter;
    SP_COM_PROPERTY_S sCOMProperty;   
    memset(&sCOMFilter, 0, sizeof(SP_COM_FILTER_LIST_S));
    memset(&sCOMProperty, 0, sizeof(SP_COM_PROPERTY_S));
    
    const UINT filterNums = 2;
    char *ppFilter[filterNums];
    for (int i = 0; i < filterNums; i++)
    {
        ppFilter[i] = NULL;
    }
    
    // Black kernel comport filter list
    ppFilter[0] = g_sMetaComm.sPortFilter.strBromFilter;
    ppFilter[1] = g_sMetaComm.sPortFilter.strKernelFilter;
    
    sCOMFilter.m_uCount = filterNums;
    sCOMFilter.m_ppFilterID = ppFilter;
    sCOMFilter.m_eType = SP_BLACK_LIST;
    
    if (g_sMetaComm.bUsbEnable)
    {     
        //phase out SP_META_GetDynamicUSBComPort_r API, cause it have probabilistic can`t fetch kernel comport issue  
        int iTimeout = m_tMetaReq_Ex.ms_connect_timeout / 1000;  //timeout unit: s, but m_tMetaReq_Ex.ms_connect_timeout unit is ms
        iRet = SP_GetIncrementCOMPortWithFilter(&sCOMFilter, &sCOMProperty, &GUID_PORT_CLASS_USB2SER, _FALSE, m_pMetaStopFlag, 100); 
        if(0 == iRet)
        { 
            kernelComport = sCOMProperty.m_uNumber;
            m_iKernelComport = kernelComport;
            if (spMetaMode == SP_AP_META)
            {  
                spUSBConnReq.com_port = kernelComport;
                spUSBConnReq.ms_connect_timeout = m_tMetaReq_Ex.ms_connect_timeout;
                iRet = TryToOpenSPKernelComport(kernelComport);
                if(iRet == OK)
                {
                    sInfo.Format("SmartPhoneSN::SP_META_ConnectInMetaModeByUSB_r(): Start enter AP meta mode, KernelComPort = %d", kernelComport);
					this->Display(sInfo);
                    iRet = SPMETA_DLL::SP_META_ConnectInMetaModeByUSB_r (this->m_ApExe.iMetaAP_handle, &spUSBConnReq, m_pMetaStopFlag, &spUSBConnReport);
					//if(g_sMetaComm.IgnoreApdatabase)
					if(1)
					{

						if (iRet == META_SUCCESS || iRet == SPMETA_DLL::META_MAUI_DB_INCONSISTENT)
						{
							iRet = META_SUCCESS;                
							this->Display("SmartPhoneSN::SP_META_ConnectInMetaModeByUSB_r(): Enter meta successfully!!");
						}//end if (iRet == META_SUCCESS || iRet == SPMETA_DLL::META_MAUI_DB_INCONSISTENT)
					}
					else
					{
						if (iRet == META_SUCCESS)
						{               
							this->Display("SmartPhoneSN::SP_META_ConnectInMetaModeByUSB_r(): Enter meta successfully!!");
						}//end if (iRet == META_SUCCESS)
						else
						{
							pFuncName = "SP_META_ConnectInMetaModeByUSB_r()";
							sInfo.Format("SmartPhoneSN::SP_META_ConnectInMetaModeByUSB_r(): Enter meta fail, MetaResult = %s", ResultToString_SP(iRet));                       
							this->Display(sInfo);
							goto Err;
						}
					}
                }//if(iRet==OK)
                else
                {
                    iRet = 1;
                    pFuncName = "TryToOpenSPKernelComport()";
                    this->Display("SmartPhoneSN::TryToOpenSPKernelComport(): Try to open kernel comport fail!!");                       
                    goto Err;
                }
            }//end if (spMetaMode == SP_AP_META)
            else
            {
                //TO DO
                iRet = META_FAILED;
            }
        }
        else
        {
            pFuncName = "SP_GetIncrementCOMPortWithFilter()";
            sInfo.Format("SmartPhoneSN::ConnectWithKernelPort_Ex(): SP_GetIncrementCOMPortWithFilter get kernel comport fail, iRet = %d", iRet);
			this->Display(sInfo);
            return iRet;
        }
    }//end if(g_sMetaComm.bUsbEnable)
    else
    {
        kernelComport = g_sMetaComm.iCOMPort;
        m_iKernelComport = kernelComport;

        SPMETA_DLL::WM_META_ConnectInMETA_Req uartMETA_Req;
        SPMETA_DLL::WM_META_ConnectInMETA_Report uartMETA_Report;
        memset(&uartMETA_Req, 0 ,sizeof(SPMETA_DLL::WM_META_ConnectInMETA_Req));
        memset(&uartMETA_Report, 0 ,sizeof(SPMETA_DLL::WM_META_ConnectInMETA_Report));

        uartMETA_Req.com_port = m_iKernelComport;
        uartMETA_Req.baudrate[0] = SPMETA_DLL::META_BAUD921600;
        uartMETA_Req.baudrate[1] = SPMETA_DLL::META_BAUD_END;
        uartMETA_Req.flowctrl    = SPMETA_DLL::META_NO_FLOWCTRL;//META_SW_FLOWCTRL;
        uartMETA_Req.ms_connect_timeout = m_tMetaReq_Ex.ms_connect_timeout;

        
        sInfo.Format("SmartPhoneSN::SP_META_ConnectInMetaMode_r(): Start enter AP meta mode, KernelComPort = %d", kernelComport);
		this->Display(sInfo);
        iRet = SPMETA_DLL::SP_META_ConnectInMetaMode_r(m_ApExe.iMetaAP_handle, &uartMETA_Req, m_pMetaStopFlag, &uartMETA_Report);
        if (iRet == META_SUCCESS)
        {                          
            this->Display("SmartPhoneSN::SP_META_ConnectInMetaMode_r(): Enter meta successfully!!");
        }//end if (iRet == META_SUCCESS)
        else
        {
            pFuncName = "SmartPhoneSN::SP_META_ConnectInMetaMode_r()";
            sInfo.Format("SmartPhoneSN::SP_META_ConnectInMetaMode_r(): enter meta fail, MetaResult = %s", ResultToString_SP(iRet));
			this->Display(sInfo);
            goto Err;
        } 
    }
	
	return iRet;

Err:
    sInfo.Format("SmartPhoneSN::%s: MetaResult = %s", pFuncName, ResultToString_SP(iRet));
	this->Display(sInfo);
    return iRet;
}

SPMETA_DLL::META_RESULT CColorBoxView::EnterAPMetaMode()
{
	CString strmsg;
    this->Display("SmartPhoneSN::EnterAPMetaMode() : Enter ap meta start...", 3);

    SPMETA_DLL::META_RESULT spMetaResult = SPMETA_DLL::META_SUCCESS;
    int bootResult = 0;    
    this->Display("Start searching preloader com port and handshake...", 3);    	

    bootResult = ConnectWithPreloader();
    if (bootResult != 0)
    {
        this->Display("Searching preloader com port and handshake Fail!!", 3);        
        spMetaResult = (SPMETA_DLL::META_RESULT)bootResult;
        return spMetaResult;
    }
    else
    {
        this->Display("Searching preloader com port and handshake OK!!", 3);        
        this->Display("Start searching kernel com port and handshake...", 3);                
        bootResult = ConnectWithKernelPort_Ex(SP_AP_META);        
        if (bootResult == 0)
        {
            this->Display("Searching kernel com port and handshake OK!!", 3);            
            m_eMetaMode = SP_AP_META;
            m_bTargetInMetaMode = true;
        }  
        else
        {
            this->Display("Searching kernel com port and handshake Fail!!", 3);            
            if (bootResult == SPMETA_DLL::META_MAUI_DB_INCONSISTENT)
            {
                m_eMetaMode = SP_AP_META;
                m_bTargetInMetaMode = true;
            } 
            else
            {
                m_eMetaMode = SP_NOTIN_META;
                m_bTargetInMetaMode = false;
            }            
        }
    }
    
    this->Display("SmartPhoneSN::EnterAPMetaMode() : Enter ap meta end...", 3);
    spMetaResult = (SPMETA_DLL::META_RESULT)bootResult;
    return spMetaResult;
}

bool CColorBoxView::QueryEncryptionSupport(int * MetaHandle )
{
	CString sInfo;
	SPMETA_DLL::CRYPTFS_QUERYSUPPORT_CNF cryptfs_Support_cnf;
	SPMETA_DLL::META_RESULT MetaResult;
	MetaResult = SPMETA_DLL::SP_META_ENCRYPTED_Support_r(*MetaHandle,60000, &cryptfs_Support_cnf);

	if((MetaResult == META_SUCCESS) && (cryptfs_Support_cnf.support == 1))
	{
		this->Display(" Do support Encryption!",3);
		return true;
	}
    else
    {
		sInfo.Format(" Do not support Encryption : MetaResult = %s!", ResultToString_SP(MetaResult));
		this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
        return false;
    }	
	//return true;
}

int CColorBoxView::GetSPModemInfo_Ex()
{
	CString sInfo;
    int iRet = 0;
    char *pFuncName = NULL;
//	bool bExternMDDownload = (g_sMetaComm.sWriteOption.bWriteBarcode)? g_sMetaComm.sBarcodeOption.bExtMdDl:false;
	
    //Init parameters for without world phone feature
    m_sMdInfo.number_of_md = 1;
    m_sMdInfo.active_md_idx = 1;
    m_sMdInfo.number_of_mdSwImg = 1;
    m_sMdInfo.activeMdTypeIdx = 0;
    m_bDSDAProject = false;

    
    SPMETA_DLL::MODEM_QUERY_INFO_REQ pReq;
    SPMETA_DLL::MODEM_QUERY_INFO_CNF pCnf;
    memset(&pReq, 0, sizeof(SPMETA_DLL::MODEM_QUERY_INFO_REQ));
    memset(&pCnf, 0, sizeof(SPMETA_DLL::MODEM_QUERY_INFO_CNF));

    if(SPMETA_DLL::SP_META_QueryIfFunctionSupportedByTarget_r(m_ApExe.iMetaAP_handle, 5000, "SP_META_MODEM_Query_Info_r") == SPMETA_DLL::META_SUCCESS)
    {       
        iRet = SPMETA_DLL::SP_META_MODEM_Query_Info_r(m_ApExe.iMetaAP_handle, 2000, &pReq, &pCnf); 
        if (iRet != META_SUCCESS)
        {
            sInfo.Format("SmartPhoneSN::SP_META_MODEM_Query_Info_r(): Get modem info from target Fail, MetaResult = %s!", ResultToString_SP(iRet));
			this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
            return iRet;
        }
        else
        {
            sInfo.Format("SmartPhoneSN::SP_META_MODEM_Query_Info_r(): Get modem info from target successfully! md_numb = %d, active_md_idx = %d!",
                pCnf.modem_number, pCnf.modem_id);
			this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
            m_sMdInfo.number_of_md = pCnf.modem_number;
            m_sMdInfo.active_md_idx = pCnf.modem_id;
            m_bWithoutMD = (m_sMdInfo.number_of_md == 0) ? true : false;
            m_bDualModem = (m_sMdInfo.number_of_md == 2) ? true : false;
        }
    }
    
    
    
    SPMETA_DLL::MODEM_CAPABILITY_LIST_REQ pCapabilitiesReq;
    SPMETA_DLL::MODEM_CAPABILITY_LIST_CNF pCapabilitiesCnf;
    memset(&pCapabilitiesReq, 0, sizeof(pCapabilitiesReq));
    memset(&pCapabilitiesCnf, 0, sizeof(pCapabilitiesCnf));
    
    if(SPMETA_DLL::SP_META_QueryIfFunctionSupportedByTarget_r(m_ApExe.iMetaAP_handle, 5000, "SP_META_MODEM_Capability_r") == SPMETA_DLL::META_SUCCESS)
    {       
        iRet = SPMETA_DLL::SP_META_MODEM_Capability_r(m_ApExe.iMetaAP_handle, 2000, &pCapabilitiesReq, &pCapabilitiesCnf); 
        if (iRet != META_SUCCESS)
        {
            m_sMdInfo.multi_md_capability_support = 0;
            this->Display("SmartPhoneSN::SP_META_MODEM_Capability_r(): Get modem capability from target Fail!!",3);
            
            m_tMetaReq_Ex.protocol = 1;
            m_tMetaReq_Ex.channel_type = 1;
            iRet = META_SUCCESS;
        }
        else
        {                  
            m_sMdInfo.multi_md_capability_support = 1;
            memcpy(&m_SpMdCapList, &pCapabilitiesCnf, sizeof(pCapabilitiesCnf));
            m_tMetaReq_Ex.protocol = m_SpMdCapList.modem_cap[m_sMdInfo.active_md_idx-1].md_service;
            m_tMetaReq_Ex.channel_type =  m_SpMdCapList.modem_cap[m_sMdInfo.active_md_idx-1].ch_type;
            
                       
            //for MT6595 DSDA project
            if (m_sMdInfo.number_of_md >= 2)
            {
                m_bDSDAProject = true;
                int mdIndex = 0;
                for (int i = 0; i < MAX_MD_CHANNEL_NUM; i++)
                {
                    sInfo.Format("SmartPhoneSN: Modem capability, protocol = %d, channel_type = %d!",
                        m_SpMdCapList.modem_cap[i].md_service, m_SpMdCapList.modem_cap[i].ch_type);
					this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
                    if (m_SpMdCapList.modem_cap[i].md_service > 0 )
                    {
                        m_iMDChannelIndex[mdIndex] = i;
                        mdIndex += 1;
                    }

					if (m_sMdInfo.number_of_md == 2 && m_SpMdCapList.modem_cap[i].md_service ==3)
					{
						m_bDSDAProject = false;
					}

                }
            }      
        }
        sInfo.Format("SmartPhoneSN::SP_META_MODEM_Capability_r(): Get modem capability, protocol = %d, channel_type = %d!",
            m_tMetaReq_Ex.protocol, m_tMetaReq_Ex.channel_type);
		this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
    }
    
       
    SPMETA_DLL::MODEM_GET_CURRENTMODEMTYPE_REQ pCurMDTypeReq;
    SPMETA_DLL::MODEM_GET_CURENTMODEMTYPE_CNF pCurMDTypeCnf;
    memset(&pCurMDTypeReq, 0, sizeof(pCurMDTypeReq));
    memset(&pCurMDTypeCnf, 0, sizeof(pCurMDTypeCnf));
    
    if(SPMETA_DLL::SP_META_QueryIfFunctionSupportedByTarget_r(m_ApExe.iMetaAP_handle, 5000, "SP_META_MODEM_Get_CurrentModemType_r") == SPMETA_DLL::META_SUCCESS)
    {       
        iRet = SPMETA_DLL::SP_META_MODEM_Get_CurrentModemType_r(m_ApExe.iMetaAP_handle, 2000, &pCurMDTypeReq, &pCurMDTypeCnf);
        if (iRet != META_SUCCESS)
        {
            iRet = META_SUCCESS;
            this->Display("SmartPhoneSN::SP_META_MODEM_Get_CurrentModemType_r(): Get modem type from target Fail, mean that handset not support worldphone feature!!", 3);
        }
        else
        {
            m_sMdInfo.current_mdtype = pCurMDTypeCnf.current_modem_type;
            sInfo.Format("SmartPhoneSN::SP_META_MODEM_Get_CurrentModemType_r(): Get MD Image info from target success, MdType = %d.", m_sMdInfo.current_mdtype);
			this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
        }
    }
    
    
    SPMETA_DLL::MODEM_QUERY_MDIMGTYPE_REQ pMDImgTypeReq;
    SPMETA_DLL::MODEM_QUERY_MDIMGTYPE_CNF pMDImgTypeCnf;
    memset(&pMDImgTypeReq, 0, sizeof(pMDImgTypeReq));
    memset(&pMDImgTypeCnf, 0, sizeof(pMDImgTypeCnf));
    
    if(SPMETA_DLL::SP_META_QueryIfFunctionSupportedByTarget_r(m_ApExe.iMetaAP_handle, 5000, "SP_META_MODEM_Query_MDIMGType_r") == SPMETA_DLL::META_SUCCESS)
    {       
        iRet = SPMETA_DLL::SP_META_MODEM_Query_MDIMGType_r(m_ApExe.iMetaAP_handle, 2000, &pMDImgTypeReq, &pMDImgTypeCnf);
        if (iRet != META_SUCCESS)
        {
            iRet = META_SUCCESS;
            this->Display("SmartPhoneSN::SP_META_MODEM_Query_MDIMGType_r(): Get MD Image info from target fail, mean that handset not support worldphone feature!!", 3);
        }
        else
        {
            memcpy(m_sMdInfo.mdimg_type, pMDImgTypeCnf.mdimg_type, 16);
            bool bAllZero = true;
            m_bWorldPhone = true;
            m_sMdInfo.number_of_mdSwImg = 0;
            //Get current active mdtype index
            for(int i = 0; i < 16; i++)
            {
                if(m_sMdInfo.mdimg_type[i] !=0 )
                {
                    m_sMdInfo.number_of_mdSwImg += 1;                                    
                    bAllZero = false;
                }
                
                if(m_sMdInfo.current_mdtype != 0 && m_sMdInfo.mdimg_type[i] == m_sMdInfo.current_mdtype)
                {
                    m_sMdInfo.activeMdTypeIdx = i;
                }
            }
            
            //For before MT6577(include MT6577)old chip project
            //Old chip project call this api will return success, but mdimg_type array data all zero
            if (bAllZero)
            {
                m_sMdInfo.number_of_mdSwImg = 1;
                m_sMdInfo.activeMdTypeIdx = 0;
                m_bWorldPhone = false;
                this->Display("SmartPhoneSN::SP_META_MODEM_Query_MDIMGType_r(): Get MD Image info from target success but all zero, mean that handset not support worldphone feature!!", 3);
            }
            else
            {
                sInfo.Format("SmartPhoneSN::SP_META_MODEM_Query_MDIMGType_r(): Get MD Image info from target success, activeMdTypeIdx = %d, mean that handset support worldphone feature!!", m_sMdInfo.activeMdTypeIdx);
				this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
            }
            
            if (m_sMdInfo.number_of_md >= 2)
            {
                //extern modem have one SwImg
                m_sMdInfo.number_of_mdSwImg += (m_sMdInfo.number_of_md - 1);
            }
        }
    }  
	SPMETA_DLL::MODEM_QUERY_DOWNLOAD_STATUS_REQ pDLReq;
	SPMETA_DLL::MODEM_QUERY_DOWNLOAD_STATUS_CNF pDLCnf;
	   
    memset(&pDLReq, 0, sizeof(SPMETA_DLL::MODEM_QUERY_DOWNLOAD_STATUS_REQ));
    memset(&pDLCnf, 0, sizeof(SPMETA_DLL::MODEM_QUERY_DOWNLOAD_STATUS_CNF));
	   
	   //if(SPMETA_DLL:P_META_QueryIfFunctionSupportedByTarget_r(m_hSPMetaHandle, 5000, "SP_META_MODEM_Query_Download_Status_r ") == SPMETA_DLL::META_SUCCESS)
    if(m_bDSDAProject)
    {	   
	   iRet = SPMETA_DLL::SP_META_MODEM_Query_Download_Status_r (m_ApExe.iMetaAP_handle, 80000, &pDLReq, &pDLCnf);
	   if (iRet != META_SUCCESS)
	   {
		   sInfo.Format("SmartPhoneSN::SP_META_MODEM_Query_Download_Status_r(): Query extern modem download process fail, MetaResult = %s", ResultToString_SP(iRet));
		   this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
		   iRet = 0;
	   }
    }

	return iRet;
}

META_RESULT CColorBoxView::LoadModemDatabase(int MDindex)
{
	return META_SUCCESS;
	CString sInfo;
    unsigned long db = 0;
    META_RESULT meta_result = META_SUCCESS;
    
    bool bInitMD1Done = true;
    
    bool *pbInitDBDone = NULL;
    char *pStrMDDbpath = NULL;
    switch (MDindex)
    {
    case 0:
		{
			if(strlen(g_sMetaComm.sDBFileOption.strMD1Dbpath) <= 0)
				strncpy(g_sMetaComm.sDBFileOption.strMD1Dbpath,(LPCTSTR)this->m_csDBPath,strlen(this->m_csDBPath)+1);
            pbInitDBDone = &g_sMetaComm.sDBFileOption.bDBInitModem_1;
            pStrMDDbpath = g_sMetaComm.sDBFileOption.strMD1Dbpath;
		}
        break;
        
    case 1:
		{
			if(strlen(g_sMetaComm.sDBFileOption.strMD2Dbpath) <= 0)
				strncpy(g_sMetaComm.sDBFileOption.strMD2Dbpath,(LPCTSTR)this->m_csDBPath,strlen(this->m_csDBPath)+1);
			pbInitDBDone = &g_sMetaComm.sDBFileOption.bDBInitModem_2;
			pStrMDDbpath = g_sMetaComm.sDBFileOption.strMD2Dbpath;
		}
        break;
        
    default:
        sInfo.Format("SmartPhoneSN::LoadModemDatabase(): Incorrect MDIndex = %d!", MDindex);
        return META_INVALID_ARGUMENTS;
        break;
    }
    
    if (!pStrMDDbpath || strlen(pStrMDDbpath) == 0)
    {
        this->Display ("SmartPhoneSN::LoadModemDatabase(): Incorrect database file path!", 3);
        return META_INVALID_ARGUMENTS;
    }
    else
    {
        if((*pbInitDBDone) == false)
        {                             
            char *pFuncName = NULL;
            //if (g_sMetaComm.eTargetType == SMART_PHONE_DUALMODEM)
            if ((m_bDualModem == true && m_bWorldPhone == false) /*|| g_sMetaComm.eTargetType == SMART_PHONE_DUALMODEM*/)
            {
                meta_result = META_NVRAM_Init_Ex_r ( m_nMetaHandle, MDindex, pStrMDDbpath, &db);
                pFuncName = "META_NVRAM_Init_Ex_r";
            } 
            else
            {
                MDindex = m_sMdInfo.active_md_idx - 1;                 
                //For MT6595 DSDA project
                if (m_bDSDAProject)
                {
                    MDindex = m_iCurMDChanelIndex;
                    if (m_bInitExtMDdb)
                    {
                        m_sMdInfo.activeMdTypeIdx = 0;
                    }
                }

                meta_result = META_NVRAM_Init_Ex_Mdtype_r(m_nMetaHandle, MDindex, m_sMdInfo.activeMdTypeIdx, pStrMDDbpath, &db);
                pFuncName = "META_NVRAM_Init_Ex_Mdtype_r";
            }
            sInfo.Format("SmartPhoneSN::%s(): start to init MD[%d] database, DB path = \"%s\"...", 
                pFuncName, MDindex, pStrMDDbpath);
			this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
           
        #ifdef _IGNORE_DB_INCONSISTENT_
            if (META_SUCCESS == meta_result || meta_result == META_MAUI_DB_INCONSISTENT)
            {
                *pbInitDBDone = true;				
                if (META_SUCCESS == meta_result)
                {
                    MTRACE (g_hEBOOT_DEBUG, "SmartPhoneSN::%s(): Init MD[%d] database successfully!", pFuncName, MDindex);
                } 
                else if(META_MAUI_DB_INCONSISTENT == meta_result)
                {
                    MTRACE (g_hEBOOT_DEBUG, "Warning: SmartPhoneSN::%s(): Current database not match handset, MetaResult = %s!", 
                        pFuncName, ResultToString(meta_result));
                }
                meta_result = META_SUCCESS;
            }
        #else
            if (META_SUCCESS == meta_result)
            {
                *pbInitDBDone = true;				
                sInfo.Format("SmartPhoneSN::%s(): Init MD[%d] database successfully!", pFuncName, MDindex);             
				this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
            }
        #endif           
            else
            {
                *pbInitDBDone = false;
                sInfo.Format("SmartPhoneSN::%s(): Init MD[%d] database Fail, MetaResult = %s",
                    pFuncName, MDindex, ResultToString(meta_result)); 
				this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
            }
            
        }
        else
        {
            //For MT6595 DSDA project
            if (m_bDSDAProject)
            {
                MDindex = m_iCurMDChanelIndex;
            }
            sInfo.Format("SmartPhoneSN::META_NVRAM_Init_r(): MD[%d] database already init done!",  MDindex);
			this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
        }       
    }

    return meta_result;
}

META_RESULT CColorBoxView::EnterModemMetaToInitModemDB()
{  
	CString sInfo;
    META_RESULT MetaResult = META_SUCCESS;
    this->Display("SmartPhoneSN::ApSwithToModemMeta_Ex(): Switch to modem meta from AP meta start...", 3);
    MetaResult = APSwithToModemMeta_Ex();

    if (MetaResult == META_SUCCESS)
    {       
        m_eMetaMode = SP_MODEM_META;
        MetaResult = LoadModemDatabase(0);
    }
    else
    {
        sInfo.Format("SmartPhoneSN::EnterModemMetaToInitModemDB(): AP meta switch to modem meta cause error, MetaResult = %s",
            ResultToString(MetaResult));
		this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
    }

    return MetaResult;
}

META_RESULT CColorBoxView::APSwithToModemMeta_Ex()
{
	CString sInfo;
    this->Display("SmartPhoneSN::META_ConnectWithMultiModeTarget_r():  Switch to Modem meta from AP meta by META_ConnectWithMultiModeTarget_r start...!", 3);
    int iRet = 0;
    iRet = SPMETA_DLL::SP_META_DisconnectInMetaMode_r(m_ApExe.iMetaAP_handle);
    if(iRet == META_SUCCESS)
    {	  
        m_tMetaReq_Ex.com_port = m_iKernelComport;
        iRet = META_ConnectWithMultiModeTarget_r(m_nMetaHandle, &m_tMetaReq_Ex,sizeof(m_tMetaReq_Ex),m_pMetaStopFlag,&m_tMetaConnReport_Ex);
        if(iRet != META_SUCCESS)
        {        
            m_eMetaMode = SP_AP2MD_FAIL;
            sInfo.Format("SmartPhoneSN::META_ConnectWithMultiModeTarget_r():  Switch to Modem meta from AP meta Fail, MetaResult = %s", ResultToString(iRet));
			this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
        }
		else
		{           
            m_eMetaMode = SP_MODEM_META;
		    this->Display("SmartPhoneSN::META_ConnectWithMultiModeTarget_r():  Switch to Modem meta from AP meta successfully!", 3);
		}
    }	
    
    return (META_RESULT)iRet;
}

META_RESULT CColorBoxView::ModemSwithToAPMeta()
{
	CString sInfo;
    this->Display("SmartPhoneSN::ModemSwithToAPMeta(): Start switch to AP meta from Modem meta...");

    META_RESULT MetaResult = META_SUCCESS;
	
       
    MetaResult = META_DisconnectInMetaMode_r (m_nMetaHandle);        
    if (MetaResult != META_SUCCESS)
    {
        sInfo.Format("SmartPhoneSN::META_DisconnectInMetaMode_r(): Disconnect Modem meta mode Fail!, MetaResult = %s",  ResultToString(MetaResult));
		this->Display(sInfo);
    }
    else
    {       
        int BootStop = 0;
        int usb_port = 0;
        SPMETA_DLL::WM_META_ConnectInMETA_Req BootRequest; 
        SPMETA_DLL::WM_META_ConnectInMETA_Report BootReport;
        
        BootRequest.com_port = m_iKernelComport;
        BootRequest.baudrate[0] = SPMETA_DLL::META_BAUD115200;
        BootRequest.baudrate[1] = SPMETA_DLL::META_BAUD230400;
        BootRequest.baudrate[2] = SPMETA_DLL::META_BAUD460800;
        BootRequest.baudrate[3] = SPMETA_DLL::META_BAUD921600;
        BootRequest.baudrate[4] = SPMETA_DLL::META_BAUD57600;
        BootRequest.baudrate[5] = SPMETA_DLL::META_BAUD_END;
        BootRequest.flowctrl    = SPMETA_DLL::META_NO_FLOWCTRL;//META_SW_FLOWCTRL;
        BootRequest.ms_connect_timeout = m_tMetaReq_Ex.ms_connect_timeout;
              
        MetaResult = (META_RESULT)SPMETA_DLL::SP_META_ConnectInMetaMode_r(this->m_ApExe.iMetaAP_handle, &BootRequest, &BootStop, &BootReport); 
        if (META_SUCCESS != MetaResult)
        {
            m_eMetaMode = SP_MD2AP_FAIL;
            sInfo.Format("SmartPhoneSN::SP_META_ConnectInMetaMode_r(): Modem meta switch to AP meta Fail!, BootRequest.com_port = %d, MetaResult = %s", 
                BootRequest.com_port, ResultToString_SP(MetaResult));
			this->Display(sInfo);
        }  
		else
		{
			m_eMetaMode = SP_AP_META;
			this->Display("SmartPhoneSN::SP_META_ConnectInMetaMode_r(): switch to AP meta from Modem meta  successfully!");    		
		}
    }
    
    
    return MetaResult;
}

void CColorBoxView::SetupMetaModeParameters()
{
    this->Display("SmartPhoneSN::SetupMetaModeParameters() start...", 3);
	//For preloader handshake parameters
    memset (&m_stModeArg, 0x0, sizeof(SP_BOOT_ARG_S));
	//For new api parameters
	m_bUseNewConnectMauiMetaAPI = true;
    memset(&m_tMetaReq_Ex, 0, sizeof(m_tMetaReq_Ex));
    memset(&m_tMetaConnReport_Ex, 0, sizeof(m_tMetaConnReport_Ex));

    if (g_sMetaComm.iPre_Connect_Timeout < 10000)
    {
        g_sMetaComm.iPre_Connect_Timeout = 10000;
    }

    if (g_sMetaComm.iKernel_Connect_Timeout < 20000)
    {
        g_sMetaComm.iKernel_Connect_Timeout = 20000;
    }
	g_sMetaComm.bUsbEnable = true;
    m_iKernelComport = g_sMetaComm.iCOMPort;
		
    //Old parameters
    m_stModeArg.m_bbchip_type=SP_AUTO_DETECT_BBCHIP;
    m_stModeArg.m_ext_clock=SP_AUTO_DETECT_EXT_CLOCK;
    m_stModeArg.m_ms_boot_timeout=SP_BOOT_INFINITE;
    //m_stModeArg.m_max_start_cmd_retry_count = SP_DEFAULT_BROM_START_CMD_RETRY_COUNT;
    m_stModeArg.m_max_start_cmd_retry_count = 0;   
    //New parameters
    m_stModeArg.m_uTimeout = g_sMetaComm.iPre_Connect_Timeout;
    m_stModeArg.m_uRetryTime = 2000;
    m_stModeArg.m_uInterval = 10;
    m_stModeArg.m_uBaudrate = CBR_115200;
    m_stModeArg.m_pStopFlag  = &g_iMetaStopFlag;

    {
        m_stModeArg.m_auth_handle = NULL;
        m_stModeArg.m_scert_handle = NULL;
        m_stModeArg.m_cb_sla_challenge = NULL;
        m_stModeArg.m_cb_sla_challenge_arg = NULL;
        m_stModeArg.m_cb_sla_challenge_end = NULL;
        m_stModeArg.m_cb_sla_challenge_end_arg = NULL;
    }      
    m_stModeArg.m_bIsUSBEnable = g_sMetaComm.bUsbEnable;   
    m_stModeArg.m_bIsSymbolicEnable = false;
    m_stModeArg.m_bIsCompositeDeviceEnable = g_sMetaComm.bCompositeDeviceEnable;
    m_stModeArg.m_euBootMode = SP_META_BOOT;
    m_stModeArg.m_uPortNumber = 0;
    m_stModeArg.m_uMDMode = 0;

    //Common
    m_tMetaReq_Ex.com_port = m_iKernelComport;
    m_tMetaReq_Ex.ms_connect_timeout = g_sMetaComm.iKernel_Connect_Timeout;
    m_tMetaReq_Ex.close_com_port = 1;
    m_tMetaReq_Ex.InMetaMode = 1;
    m_tMetaReq_Ex.protocol = 1;          // DHL
    m_tMetaReq_Ex.channel_type = 1;      // Native channel
    m_tMetaReq_Ex.boot_meta_arg.m_bbchip_type = AUTO_DETECT_BBCHIP; 
    m_tMetaReq_Ex.boot_meta_arg.m_ext_clock = AUTO_DETECT_EXT_CLOCK;
    //UART parameters
    //m_tMetaReq_Ex.baudrate[0] = META_BAUD115200;
    //m_tMetaReq_Ex.baudrate[1] = META_BAUD230400;
    //m_tMetaReq_Ex.baudrate[2] = META_BAUD460800;
    m_tMetaReq_Ex.baudrate[0] = META_BAUD921600;
    m_tMetaReq_Ex.baudrate[1] = META_BAUD_END;   
    m_tMetaReq_Ex.flowctrl = META_SW_FLOWCTRL;
    //USB
    if (g_sMetaComm.bUsbEnable)
    {
        this->Display("SmartPhoneSN::SetupMetaModeParameters(): Target enter meta mode by [USB].",3);
        m_tMetaReq_Ex.usb_enable = 1;
        m_tMetaReq_Ex.boot_meta_arg.m_usb_enable = _TRUE;       
    } 
    else
    {
        this->Display("SmartPhoneSN::SetupMetaModeParameters(): Target enter meta mode by [UART].",3);
        m_tMetaReq_Ex.usb_enable = 0;
        m_tMetaReq_Ex.boot_meta_arg.m_usb_enable = _FALSE;
        m_tMetaReq_Ex.escape = 1;
    }                    
    
    
    this->Display("SmartPhoneSN::SetupMetaModeParameters() end...",3);
}


char* ResultToString_SP(int MR)
{
  int nError = (META_RESULT)MR;
  switch((META_RESULT)MR)
  { 
	 case SPMETA_DLL::META_SUCCESS:
		  return M2S(SPMETA_DLL::META_SUCCESS);
	 
	 case SPMETA_DLL::META_FAILED:
		  return M2S(SPMETA_DLL::META_FAILED);
	 
	 case SPMETA_DLL::META_COMM_FAIL:
		  return M2S(SPMETA_DLL::META_COMM_FAIL);
	 
	 case SPMETA_DLL::META_NORESPONSE:
		  return M2S(SPMETA_DLL::META_NORESPONSE);
	 
	 case SPMETA_DLL::META_BUFFER_LEN:
		  return M2S(SPMETA_DLL::META_BUFFER_LEN);
	 
	 case SPMETA_DLL::META_FILE_BAD:
		  return M2S(SPMETA_DLL::META_FILE_BAD);
	 
	 case SPMETA_DLL::META_LID_INVALID:
		  return M2S(SPMETA_DLL::META_LID_INVALID);
	 
	 case SPMETA_DLL::META_INTERNAL_DB_ERR:
		  return M2S(SPMETA_DLL::META_INTERNAL_DB_ERR);
	 
	 case SPMETA_DLL::META_NO_MEMORY:
		  return M2S(SPMETA_DLL::META_NO_MEMORY);
	 
	 case SPMETA_DLL::META_INVALID_ARGUMENTS:
		  return M2S(SPMETA_DLL::META_INVALID_ARGUMENTS);
	 
	 case SPMETA_DLL::META_TIMEOUT:
		  return M2S(SPMETA_DLL::META_TIMEOUT);
	 
	 case SPMETA_DLL::META_BUSY:
		  return M2S(SPMETA_DLL::META_BUSY);
	 
	 case SPMETA_DLL::META_INVALID_HANDLE:
		  return M2S(SPMETA_DLL::META_INVALID_HANDLE);
	 
	 case SPMETA_DLL::META_FAT_ERROR:
		  return M2S(SPMETA_DLL::META_FAT_ERROR);
	 
	 case SPMETA_DLL::META_FAT_DISK_FULL:
		  return M2S(SPMETA_DLL::META_FAT_DISK_FULL);
	 
	 case SPMETA_DLL::META_FAT_ROOT_DIR_FULL:
		  return M2S(SPMETA_DLL::META_FAT_ROOT_DIR_FULL);
	 
	 case SPMETA_DLL::META_FAT_INVALID_FILENAME:
		  return M2S(SPMETA_DLL::META_FAT_INVALID_FILENAME);
	 
	 case SPMETA_DLL::META_FAT_INVALID_FILE_HANDLE:
		  return M2S(SPMETA_DLL::META_FAT_INVALID_FILE_HANDLE);
	 
	 case SPMETA_DLL::META_FAT_FILE_NOT_FOUND:
		  return M2S(SPMETA_DLL::META_FAT_FILE_NOT_FOUND);
	 
	 case SPMETA_DLL::META_FAT_DRIVE_NOT_FOUND:
		  return M2S(SPMETA_DLL::META_FAT_DRIVE_NOT_FOUND);
	 
	 case SPMETA_DLL::META_FAT_PATH_NOT_FOUND:
		  return M2S(SPMETA_DLL::META_FAT_PATH_NOT_FOUND);
	 
	 case SPMETA_DLL::META_FAT_ACCESS_DENIED:
		  return M2S(SPMETA_DLL::META_FAT_ACCESS_DENIED);
	 
	 case SPMETA_DLL::META_FAT_TOO_MANY_FILES:
		  return M2S(SPMETA_DLL::META_FAT_TOO_MANY_FILES);
	 
	 case SPMETA_DLL::META_INCORRECT_TARGET_VER:
		  return M2S(SPMETA_DLL::META_INCORRECT_TARGET_VER);
	 
	 case SPMETA_DLL::META_COM_ERROR:
		  return M2S(SPMETA_DLL::META_COM_ERROR);
	 
	 case SPMETA_DLL::META_BROM_CMD_ERROR:
		  return M2S(SPMETA_DLL::META_BROM_CMD_ERROR);
	 
	 case SPMETA_DLL::META_INCORRECT_BBCHIP_TYPE:
		  return M2S(SPMETA_DLL::META_INCORRECT_BBCHIP_TYPE);
	 
	 case SPMETA_DLL::META_BROM_ERROR:
		  return M2S(SPMETA_DLL::META_BROM_ERROR);
	 
	 case SPMETA_DLL::META_STOP_BOOTUP_PROCEDURE:
		  return M2S(SPMETA_DLL::META_STOP_BOOTUP_PROCEDURE);
	 
	 case SPMETA_DLL::META_CANCEL:
		  return M2S(SPMETA_DLL::META_CANCEL);
	 
	 case SPMETA_DLL::META_CCT_NOT_IMPORT_PROFILE:
		  return M2S(SPMETA_DLL::META_CCT_NOT_IMPORT_PROFILE);
	 
	 case SPMETA_DLL::META_CCT_INVALID_SENSOR_ID:
		  return M2S(SPMETA_DLL::META_CCT_INVALID_SENSOR_ID);
	 
	 case SPMETA_DLL::META_CCT_TGT_NO_MEM_FOR_SINGLE_SHOT:
		  return M2S(SPMETA_DLL::META_CCT_TGT_NO_MEM_FOR_SINGLE_SHOT);
	 
	 case SPMETA_DLL::META_CCT_TGT_NO_MEM_FOR_MULTI_SHOT:
		  return M2S(SPMETA_DLL::META_CCT_TGT_NO_MEM_FOR_MULTI_SHOT);
	 
	 case SPMETA_DLL::META_FUNC_NOT_IMPLEMENT_YET:
		  return M2S(SPMETA_DLL::META_FUNC_NOT_IMPLEMENT_YET);
	/* 
	 case SPMETA_DLL::META_CCT_NOT_IMPLEMENT_YET:
		  return M2S(SPMETA_DLL::META_CCT_NOT_IMPLEMENT_YET);
	*/ 
	 case SPMETA_DLL::META_CCT_PREVIEW_ALREADY_STARTED:
		  return M2S(SPMETA_DLL::META_CCT_PREVIEW_ALREADY_STARTED);
	 
	 case SPMETA_DLL::META_CCT_PREVIEW_ALREADY_STOPPED:
		  return M2S(SPMETA_DLL::META_CCT_PREVIEW_ALREADY_STOPPED);
	 
	 case SPMETA_DLL::META_CCT_READ_REG_NO_CNF:
		  return M2S(SPMETA_DLL::META_CCT_READ_REG_NO_CNF);
	 
	 case SPMETA_DLL::META_CCT_WRITE_REG_NO_CNF:
		  return M2S(SPMETA_DLL::META_CCT_WRITE_REG_NO_CNF);
	 
	 case SPMETA_DLL::META_CCT_TGT_ABORT_IMAGE_SENDING:
		  return M2S(SPMETA_DLL::META_CCT_TGT_ABORT_IMAGE_SENDING);
	 
	 case SPMETA_DLL::META_CCT_READ_ONLY_REGISTER:
		  return M2S(SPMETA_DLL::META_CCT_READ_ONLY_REGISTER);
	 
	 case SPMETA_DLL::META_CCT_LOAD_FROM_NVRAM_FAIL:
		  return M2S(SPMETA_DLL::META_CCT_LOAD_FROM_NVRAM_FAIL);
	 
	 case SPMETA_DLL::META_CCT_SAVE_TO_NVRAM_FAIL:
		  return M2S(SPMETA_DLL::META_CCT_SAVE_TO_NVRAM_FAIL);
	 
	 case SPMETA_DLL::META_CCT_AE_INVALID_EC_LEVEL:
		  return M2S(SPMETA_DLL::META_CCT_AE_INVALID_EC_LEVEL);
	 
	 case SPMETA_DLL::META_CCT_AE_INVALID_EC_STEP:
		  return M2S(SPMETA_DLL::META_CCT_AE_INVALID_EC_STEP);
	 
	 case SPMETA_DLL::META_CCT_AE_ALREADY_ENABLED:
		  return M2S(SPMETA_DLL::META_CCT_AE_ALREADY_ENABLED);
	 
	 case SPMETA_DLL::META_CCT_AE_ALREADY_DISABLED:
		  return M2S(SPMETA_DLL::META_CCT_AE_ALREADY_DISABLED);
	 
	 case SPMETA_DLL::META_CCT_WB_INVALID_INDEX:
		  return M2S(SPMETA_DLL::META_CCT_WB_INVALID_INDEX);
	 
	 case SPMETA_DLL::META_CCT_NO_TGT_SENSOR_MATCH_IN_PROFILE:
		  return M2S(SPMETA_DLL::META_CCT_NO_TGT_SENSOR_MATCH_IN_PROFILE);
	 
	 case SPMETA_DLL::META_CCT_IMAGE_SENDING_FAME_NUM_ERROR:
		  return M2S(SPMETA_DLL::META_CCT_IMAGE_SENDING_FAME_NUM_ERROR);
	 
	 case SPMETA_DLL::META_CCT_AE_IS_NOT_DISABLED:
		  return M2S(SPMETA_DLL::META_CCT_AE_IS_NOT_DISABLED);
	 
	 case SPMETA_DLL::META_FAT_APP_QUOTA_FULL:
		  return M2S(SPMETA_DLL::META_FAT_APP_QUOTA_FULL);
	 
	 case SPMETA_DLL::META_IMEI_CD_ERROR:
		  return M2S(SPMETA_DLL::META_IMEI_CD_ERROR);
	 
	 case SPMETA_DLL::META_RFID_MISMATCH:
		  return M2S(SPMETA_DLL::META_RFID_MISMATCH);
	 
	 case SPMETA_DLL::META_NVRAM_DB_IS_NOT_LOADED_YET:
		  return M2S(SPMETA_DLL::META_NVRAM_DB_IS_NOT_LOADED_YET);
	 
	 case SPMETA_DLL::META_CCT_ERR_CAPTURE_WIDTH_HEIGHT_TOO_SMALL:
		  return M2S(SPMETA_DLL::META_CCT_ERR_CAPTURE_WIDTH_HEIGHT_TOO_SMALL);
	 
	 case SPMETA_DLL::META_WAIT_FOR_TARGET_READY_TIMEOUT:
		  return M2S(SPMETA_DLL::META_WAIT_FOR_TARGET_READY_TIMEOUT);
	 
	 case SPMETA_DLL::META_CCT_ERR_SENSOR_ENG_SET_INVALID_VALUE:
		  return M2S(SPMETA_DLL::META_CCT_ERR_SENSOR_ENG_SET_INVALID_VALUE);
	 
	 case SPMETA_DLL::META_CCT_ERR_SENSOR_ENG_GROUP_NOT_EXIST:
		  return M2S(SPMETA_DLL::META_CCT_ERR_SENSOR_ENG_GROUP_NOT_EXIST);
	 
	 case SPMETA_DLL::META_CCT_NO_TGT_ISP_MATCH_IN_PROFILE:
		  return M2S(SPMETA_DLL::META_CCT_NO_TGT_ISP_MATCH_IN_PROFILE);
	 
	 case SPMETA_DLL::META_CCT_TGT_ISP_SUPPORT_NOT_DEFINED:
		  return M2S(SPMETA_DLL::META_CCT_TGT_ISP_SUPPORT_NOT_DEFINED);
	 
	 case SPMETA_DLL::META_CCT_ERR_SENSOR_ENG_ITEM_NOT_EXIST:
		  return M2S(SPMETA_DLL::META_CCT_ERR_SENSOR_ENG_ITEM_NOT_EXIST);
	 
	 case SPMETA_DLL::META_CCT_ERR_INVALID_COMPENSATION_MODE:
		  return M2S(SPMETA_DLL::META_CCT_ERR_INVALID_COMPENSATION_MODE);
	 
	 case SPMETA_DLL::META_CCT_ERR_USB_COM_NOT_READY:
		  return M2S(SPMETA_DLL::META_CCT_ERR_USB_COM_NOT_READY);
	 
	 case SPMETA_DLL::META_CCT_DEFECTPIXEL_CAL_UNDER_PROCESSING:
		  return M2S(SPMETA_DLL::META_CCT_DEFECTPIXEL_CAL_UNDER_PROCESSING);
	 
	 case SPMETA_DLL::META_CCT_ERR_DEFECTPIXEL_CAL_NO_MEM:
		  return M2S(SPMETA_DLL::META_CCT_ERR_DEFECTPIXEL_CAL_NO_MEM);
	 
	 case SPMETA_DLL::META_CCT_ERR_TOO_MANY_DEFECT_PIXEL:
		  return M2S(SPMETA_DLL::META_CCT_ERR_TOO_MANY_DEFECT_PIXEL);
	 
	 case SPMETA_DLL::META_CCT_ERR_CAPTURE_JPEG_FAIL:
		  return M2S(SPMETA_DLL::META_CCT_ERR_CAPTURE_JPEG_FAIL);
	 
	 case SPMETA_DLL::META_CCT_ERR_CAPTURE_JPEG_TIMEOUT:
		  return M2S(SPMETA_DLL::META_CCT_ERR_CAPTURE_JPEG_TIMEOUT);
	 
	 case SPMETA_DLL::META_CCT_ERR_AF_FAIL:
		  return M2S(SPMETA_DLL::META_CCT_ERR_AF_FAIL);
	 
	 case SPMETA_DLL::META_CCT_ERR_AF_TIMEOUT:
		  return M2S(SPMETA_DLL::META_CCT_ERR_AF_TIMEOUT);
	 
	 case SPMETA_DLL::META_CCT_ERR_AF_LENS_OFFSET_CAL_FAIL:
		  return M2S(SPMETA_DLL::META_CCT_ERR_AF_LENS_OFFSET_CAL_FAIL);
	 
	 case SPMETA_DLL::META_CCT_ERR_PREVIEW_MUST_ENABLE:
		  return M2S(SPMETA_DLL::META_CCT_ERR_PREVIEW_MUST_ENABLE);
	 
	 case SPMETA_DLL::META_CCT_ERR_UNSUPPORT_CAPTURE_FORMAT:
		  return M2S(SPMETA_DLL::META_CCT_ERR_UNSUPPORT_CAPTURE_FORMAT);
	 
	 case SPMETA_DLL::META_CCT_ERR_EXCEED_MAX_DEFECT_PIXEL:
		  return M2S(SPMETA_DLL::META_CCT_ERR_EXCEED_MAX_DEFECT_PIXEL);
	 
	 case SPMETA_DLL::META_ERR_EXCEED_MAX_PEER_BUF_SIZE:
		  return M2S(SPMETA_DLL::META_ERR_EXCEED_MAX_PEER_BUF_SIZE);
	 
	 case SPMETA_DLL::META_CCT_ERR_INVALID_WIDTH_FACTOR:
		  return M2S(SPMETA_DLL::META_CCT_ERR_INVALID_WIDTH_FACTOR);
	 
	 case SPMETA_DLL::META_BROM_SECURITY_CHECK_FAIL:
		  return M2S(SPMETA_DLL::META_BROM_SECURITY_CHECK_FAIL);
	 
	 case SPMETA_DLL::META_CCT_ERR_PREVIEW_MUST_DISABLE:
		  return M2S(SPMETA_DLL::META_CCT_ERR_PREVIEW_MUST_DISABLE);
	 
	 case SPMETA_DLL::META_MAUI_DB_INCONSISTENT:
		  return M2S(SPMETA_DLL::META_MAUI_DB_INCONSISTENT);
	 
	 case SPMETA_DLL::META_FAT_FILEPATH_TOO_LONG:
		  return M2S(SPMETA_DLL::META_FAT_FILEPATH_TOO_LONG);
	 
	 case SPMETA_DLL::META_FAT_RESTRICTED_FILEPATH:
		  return M2S(SPMETA_DLL::META_FAT_RESTRICTED_FILEPATH);
	 
	 case SPMETA_DLL::META_FAT_DIR_NOT_EXIST:
		  return M2S(SPMETA_DLL::META_FAT_DIR_NOT_EXIST);
	 
	 case SPMETA_DLL::META_FAT_DISK_SPACE_IS_NOT_ENOUGH:
		  return M2S(SPMETA_DLL::META_FAT_DISK_SPACE_IS_NOT_ENOUGH);
	 
	 case SPMETA_DLL::META_TDMB_ERR_BAND_NOT_EXIST:
		  return M2S(SPMETA_DLL::META_TDMB_ERR_BAND_NOT_EXIST);
	 
	 case SPMETA_DLL::META_TDMB_ERR_FREQ_NOT_EXIST:
		  return M2S(SPMETA_DLL::META_TDMB_ERR_FREQ_NOT_EXIST);
	 
	 case SPMETA_DLL::META_TDMB_ERR_ENSM_NOT_EXIST:
		  return M2S(SPMETA_DLL::META_TDMB_ERR_ENSM_NOT_EXIST);
	 
	 case SPMETA_DLL::META_TDMB_ERR_SERV_NOT_EXIST:
		  return M2S(SPMETA_DLL::META_TDMB_ERR_SERV_NOT_EXIST);
	 
	 case SPMETA_DLL::META_TDMB_ERR_SUB_CHAN_NOT_EXIST:
		  return M2S(SPMETA_DLL::META_TDMB_ERR_SUB_CHAN_NOT_EXIST);
	 
	 case SPMETA_DLL::META_TDMB_ERR_DEMOD_STATE:
		  return M2S(SPMETA_DLL::META_TDMB_ERR_DEMOD_STATE);
	 
	 case SPMETA_DLL::META_PERMISSION_DENIED:
		  return M2S(SPMETA_DLL::META_PERMISSION_DENIED);
	 
	 case SPMETA_DLL::META_ENUMERATE_USB_FAIL:
		  return M2S(SPMETA_DLL::META_ENUMERATE_USB_FAIL);
	 
	 case SPMETA_DLL::META_STOP_ENUM_USB_PROCEDURE:
		  return M2S(SPMETA_DLL::META_STOP_ENUM_USB_PROCEDURE);
	 
	 case SPMETA_DLL::META_CCT_6238_AE_ALREADY_ENABLED:
		  return M2S(SPMETA_DLL::META_CCT_6238_AE_ALREADY_ENABLED);
	 
	 case SPMETA_DLL::META_CCT_6238_AE_ALREADY_DISABLED:
		  return M2S(SPMETA_DLL::META_CCT_6238_AE_ALREADY_DISABLED);
	 
	 case SPMETA_DLL::META_CCT_6238_AE_IS_NOT_DISABLED:
		  return M2S(SPMETA_DLL::META_CCT_6238_AE_IS_NOT_DISABLED);
	 
	 case SPMETA_DLL::META_CCT_6238_ISP_FLASHLIGHT_LINEARITY_PRESTROBE_FAIL:
		  return M2S(SPMETA_DLL::META_CCT_6238_ISP_FLASHLIGHT_LINEARITY_PRESTROBE_FAIL);
	 
	 case SPMETA_DLL::META_NOT_SUPPORT:
		  return M2S(SPMETA_DLL::META_NOT_SUPPORT);
	 
	 case SPMETA_DLL::META_LAST_RESULT:
		  return M2S(SPMETA_DLL::META_LAST_RESULT);
  default:
		return M2S(DEFAULT_FAIL);
  }
  
}

char* ResultToString(int MR)
{
	switch((META_RESULT)MR)
	{
	
	case META_SUCCESS:
		 return M2S(META_SUCCESS);
	
	case META_FAILED:
		 return M2S(META_FAILED);
	
	case META_COMM_FAIL:
		 return M2S(META_COMM_FAIL);
	
	case META_NORESPONSE:
		 return M2S(META_NORESPONSE);
	
	case META_EBOOT_FAILED:
		 return M2S(META_EBOOT_FAILED);
	
	case META_BUFFER_LEN:
		 return M2S(META_BUFFER_LEN);
	
	case META_FILE_BAD:
		 return M2S(META_FILE_BAD);
	
	case META_LID_INVALID:
		 return M2S(META_LID_INVALID);
	
	case META_INTERNAL_DB_ERR:
		 return M2S(META_INTERNAL_DB_ERR);
	
	case META_NO_MEMORY:
		 return M2S(META_NO_MEMORY);
	
	case META_INVALID_ARGUMENTS:
		 return M2S(META_INVALID_ARGUMENTS);
	
	case META_TIMEOUT:
		 return M2S(META_TIMEOUT);
	
	case META_BUSY:
		 return M2S(META_BUSY);
	
	case META_INVALID_HANDLE:
		 return M2S(META_INVALID_HANDLE);
	
	case META_FAT_ERROR:
		 return M2S(META_FAT_ERROR);
	
	case META_FAT_DISK_FULL:
		 return M2S(META_FAT_DISK_FULL);
	
	case META_FAT_ROOT_DIR_FULL:
		 return M2S(META_FAT_ROOT_DIR_FULL);
	
	case META_FAT_INVALID_FILENAME:
		 return M2S(META_FAT_INVALID_FILENAME);
	
	case META_FAT_INVALID_FILE_HANDLE:
		 return M2S(META_FAT_INVALID_FILE_HANDLE);
	
	case META_FAT_FILE_NOT_FOUND:
		 return M2S(META_FAT_FILE_NOT_FOUND);
	
	case META_FAT_DRIVE_NOT_FOUND:
		 return M2S(META_FAT_DRIVE_NOT_FOUND);
	
	case META_FAT_PATH_NOT_FOUND:
		 return M2S(META_FAT_PATH_NOT_FOUND);
	
	case META_FAT_ACCESS_DENIED:
		 return M2S(META_FAT_ACCESS_DENIED);
	
	case META_FAT_TOO_MANY_FILES:
		 return M2S(META_FAT_TOO_MANY_FILES);
	
	case META_INCORRECT_TARGET_VER:
		 return M2S(META_INCORRECT_TARGET_VER);
	
	case META_COM_ERROR:
		 return M2S(META_COM_ERROR);
	
	case META_BROM_CMD_ERROR:
		 return M2S(META_BROM_CMD_ERROR);
	
	case META_INCORRECT_BBCHIP_TYPE:
		 return M2S(META_INCORRECT_BBCHIP_TYPE);
	
	case META_BROM_ERROR:
		 return M2S(META_BROM_ERROR);
	
	case META_STOP_BOOTUP_PROCEDURE:
		 return M2S(META_STOP_BOOTUP_PROCEDURE);
	
	case META_CANCEL:
		 return M2S(META_CANCEL);
	
	case META_FUNC_NOT_IMPLEMENT_YET:
		 return M2S(META_FUNC_NOT_IMPLEMENT_YET);
	
	case META_FAT_APP_QUOTA_FULL:
		 return M2S(META_FAT_APP_QUOTA_FULL);
	
	case META_IMEI_CD_ERROR:
		 return M2S(META_IMEI_CD_ERROR);
	
	case META_RFID_MISMATCH:
		 return M2S(META_RFID_MISMATCH);
	
	case META_NVRAM_DB_IS_NOT_LOADED_YET:
		 return M2S(META_NVRAM_DB_IS_NOT_LOADED_YET);
	
	case META_WAIT_FOR_TARGET_READY_TIMEOUT:
		 return M2S(META_WAIT_FOR_TARGET_READY_TIMEOUT);
	
	case META_ERR_EXCEED_MAX_PEER_BUF_SIZE:
		 return M2S(META_ERR_EXCEED_MAX_PEER_BUF_SIZE);
	
	case META_BROM_SECURITY_CHECK_FAIL:
		 return M2S(META_BROM_SECURITY_CHECK_FAIL);
	
	case META_MAUI_DB_INCONSISTENT:
		 return M2S(META_MAUI_DB_INCONSISTENT);
	
	case META_FAT_FILEPATH_TOO_LONG:
		 return M2S(META_FAT_FILEPATH_TOO_LONG);
	
	case META_FAT_RESTRICTED_FILEPATH:
		 return M2S(META_FAT_RESTRICTED_FILEPATH);
	
	case META_FAT_DIR_NOT_EXIST:
		 return M2S(META_FAT_DIR_NOT_EXIST);
	
	case META_FAT_DISK_SPACE_IS_NOT_ENOUGH:
		 return M2S(META_FAT_DISK_SPACE_IS_NOT_ENOUGH);
	
	case META_TDMB_ERR_BAND_NOT_EXIST:
		 return M2S(META_TDMB_ERR_BAND_NOT_EXIST);
	
	case META_TDMB_ERR_FREQ_NOT_EXIST:
		 return M2S(META_TDMB_ERR_FREQ_NOT_EXIST);
	
	case META_TDMB_ERR_ENSM_NOT_EXIST:
		 return M2S(META_TDMB_ERR_ENSM_NOT_EXIST);
	
	case META_TDMB_ERR_SERV_NOT_EXIST:
		 return M2S(META_TDMB_ERR_SERV_NOT_EXIST);
	
	case META_TDMB_ERR_SUB_CHAN_NOT_EXIST:
		 return M2S(META_TDMB_ERR_SUB_CHAN_NOT_EXIST);
	
	case META_TDMB_ERR_DEMOD_STATE:
		 return M2S(META_TDMB_ERR_DEMOD_STATE);
	
	case META_ENUMERATE_USB_FAIL:
		 return M2S(META_ENUMERATE_USB_FAIL);
	
	case META_STOP_ENUM_USB_PROCEDURE:
		 return M2S(META_STOP_ENUM_USB_PROCEDURE);
	
	case META_MISC_TARGET_LOAD_NEED_TO_BE_PATCHED:
		 return M2S(META_MISC_TARGET_LOAD_NEED_TO_BE_PATCHED);
	
	case META_MISC_INI_FILE_SETTINGS_WRONG:
		 return M2S(META_MISC_INI_FILE_SETTINGS_WRONG);
	
	case META_MISC_FAIL_TO_READ_IMEI:
		 return M2S(META_MISC_FAIL_TO_READ_IMEI);
	
	case META_MISC_FAIL_TO_BACKUP_FILE:
		 return M2S(META_MISC_FAIL_TO_BACKUP_FILE);
	
	case META_MISC_FAIL_TO_WRITE_BACKUP_RESULT:
		 return M2S(META_MISC_FAIL_TO_WRITE_BACKUP_RESULT);
	
	case META_MISC_FAIL_TO_GET_NVRAM_FOLDER_PATH:
		 return M2S(META_MISC_FAIL_TO_GET_NVRAM_FOLDER_PATH);
	
	case META_MISC_FAIL_TO_GET_NVRAM_MUST_LIST:
		 return M2S(META_MISC_FAIL_TO_GET_NVRAM_MUST_LIST);
	
	case META_STOP_CURRENT_PROCEDURE:
		 return M2S(META_STOP_CURRENT_PROCEDURE);
	
	case META_MISC_CUSTOMIZED_NVRAM_ERROR:
		 return M2S(META_MISC_CUSTOMIZED_NVRAM_ERROR);
	
	case META_MISC_FOLDER_EMPTY_CHECKING_FAIL:
		 return M2S(META_MISC_FOLDER_EMPTY_CHECKING_FAIL);
	
	case META_MISC_TOO_MANY_BACKUP_RESULT_FILE:
		 return M2S(META_MISC_TOO_MANY_BACKUP_RESULT_FILE);
	
	case META_MISC_TOO_MANY_RESTORE_RESULT_FILE:
		 return M2S(META_MISC_TOO_MANY_RESTORE_RESULT_FILE);
	
	case META_MISC_RESTORE_RESULT_FILE_NOT_EXIST:
		 return M2S(META_MISC_RESTORE_RESULT_FILE_NOT_EXIST);
	
	case META_MISC_RESTORE_RESULT_FILE_INCOMPLETE:
		 return M2S(META_MISC_RESTORE_RESULT_FILE_INCOMPLETE);
	
	case META_FAIL_TO_CELAR_ALL_IN_BACUP_FOLDER:
		 return M2S(META_FAIL_TO_CELAR_ALL_IN_BACUP_FOLDER);
	
	case META_MISC_BACKUP_RESULT_FILE_NOT_EXIST:
		 return M2S(META_MISC_BACKUP_RESULT_FILE_NOT_EXIST);
	
	case META_MISC_BACKUP_RESULT_FILE_INCOMPLETE:
		 return M2S(META_MISC_BACKUP_RESULT_FILE_INCOMPLETE);
	
	case META_MISC_IMEI_MISMATCH:
		 return M2S(META_MISC_IMEI_MISMATCH);
	
	case META_MISC_SML_FILE_VERIFY_FAIL:
		 return M2S(META_MISC_SML_FILE_VERIFY_FAIL);
	
	case META_MISC_BACKUP_RESULT_NOT_ENOUGH_FOR_NEW_LOAD:
		 return M2S(META_MISC_BACKUP_RESULT_NOT_ENOUGH_FOR_NEW_LOAD);
	
	case META_MISC_FAIL_TO_RESTORE_FILE:
		 return M2S(META_MISC_FAIL_TO_RESTORE_FILE);
	
	case META_MISC_FAIL_TO_WRITE_RESTORE_RESULT:
		 return M2S(META_MISC_FAIL_TO_WRITE_RESTORE_RESULT);
	
	case META_MISC_USE_WRONG_API_FOR_NEW_LOAD:
		 return M2S(META_MISC_USE_WRONG_API_FOR_NEW_LOAD);
	
	case META_MISC_QUERY_TARGET_CAPABILITY_FAIL:
		 return M2S(META_MISC_QUERY_TARGET_CAPABILITY_FAIL);
	
	case META_MISC_INI_SETTINGS_ERR_IN_NVRAM_SEC:
		 return M2S(META_MISC_INI_SETTINGS_ERR_IN_NVRAM_SEC);
	
	case META_MISC_INI_SETTINGS_ERR_IN_TARGET_SEC:
		 return M2S(META_MISC_INI_SETTINGS_ERR_IN_TARGET_SEC);
	
	case META_MISC_INI_SETTINGS_ERR_IN_PC_SEC:
		 return M2S(META_MISC_INI_SETTINGS_ERR_IN_PC_SEC);
	
	case META_MISC_NO_FILES_NEED_TO_BE_UPLOAD:
		 return M2S(META_MISC_NO_FILES_NEED_TO_BE_UPLOAD);
	
	case META_FAT_ACTION_NOT_SUPPORT:
		 return M2S(META_FAT_ACTION_NOT_SUPPORT);
	
	case META_MISC_EMPTY_UPLOADFILES_AND_IMEI_SEC:
		 return M2S(META_MISC_EMPTY_UPLOADFILES_AND_IMEI_SEC);
	
	case META_MISC_INI_SETTINGS_ERR_IN_MORE_SEC:
		 return M2S(META_MISC_INI_SETTINGS_ERR_IN_MORE_SEC);
	
	case META_MISC_INI_SETTINGS_ERR_IN_DELETE_SEC:
		 return M2S(META_MISC_INI_SETTINGS_ERR_IN_DELETE_SEC);
	
	case META_MISC_CHECK_TARGET_NVRAM_FILES_FAIL:
		 return M2S(META_MISC_CHECK_TARGET_NVRAM_FILES_FAIL);
	
	case META_MISC_FAIL_TO_GET_NVRAM_FOLDER_AMOUNT:
		 return M2S(META_MISC_FAIL_TO_GET_NVRAM_FOLDER_AMOUNT);
	
	case META_AUDIO_CHECK_WAVE_FILE_FAIL:
		 return M2S(META_AUDIO_CHECK_WAVE_FILE_FAIL);
	
	case META_MISC_COLLECT_NVRAM_FOLDER_FILES_FAILED:
		 return M2S(META_MISC_COLLECT_NVRAM_FOLDER_FILES_FAILED);
	
	case META_MISC_COLLECT_NVRAM_FOLDER_FILES_FIRST:
		 return M2S(META_MISC_COLLECT_NVRAM_FOLDER_FILES_FIRST);
	
	case META_MISC_BACKUP_FILE_NOT_FOUND_IN_NVRAM:
		 return M2S(META_MISC_BACKUP_FILE_NOT_FOUND_IN_NVRAM);
	
	case META_MISC_BACKUP_MORE_FILE_NOT_FOUND_IN_NVRAM:
		 return M2S(META_MISC_BACKUP_MORE_FILE_NOT_FOUND_IN_NVRAM);
	
	case META_MISC_LOCAL_FS_UNKNOWN_ERROR:
		 return M2S(META_MISC_LOCAL_FS_UNKNOWN_ERROR);
	
	case META_MISC_RETORE_FILE_NOT_FOUND_IN_BACKUP_RESULT:
		 return M2S(META_MISC_RETORE_FILE_NOT_FOUND_IN_BACKUP_RESULT);
	
	case META_MISC_LEGACY_ADC_FILE_NOT_FOUND:
		 return M2S(META_MISC_LEGACY_ADC_FILE_NOT_FOUND);
	
	case META_MISC_LEGACY_BARCODE_FILE_NOT_FOUND:
		 return M2S(META_MISC_LEGACY_BARCODE_FILE_NOT_FOUND);
	
	case META_MISC_FILE_SIZE_MISMATCH:
		 return M2S(META_MISC_FILE_SIZE_MISMATCH);
	
	case META_MISC_RESTORE_TARGET_NOT_FOUND_IN_NVRAM:
		 return M2S(META_MISC_RESTORE_TARGET_NOT_FOUND_IN_NVRAM);
	
	case META_UTIL_CONVERT_MULTIBYTE_TO_WCHAR_FAILED:
		 return M2S(META_UTIL_CONVERT_MULTIBYTE_TO_WCHAR_FAILED);
	
	case META_FAT_RECURSIVE_CREATE_FOLDER_FAILED:
		 return M2S(META_FAT_RECURSIVE_CREATE_FOLDER_FAILED);
	
	case META_FAT_COPY_INVALID_SOURCE_FOLDER:
		 return M2S(META_FAT_COPY_INVALID_SOURCE_FOLDER);
	
	case META_FAT_COPY_EMPTY_SOURCE_FOLDER:
		 return M2S(META_FAT_COPY_EMPTY_SOURCE_FOLDER);
	
	case META_MISC_MULTIPLE_ENTRY_NOT_SUPPORTED:
		 return M2S(META_MISC_MULTIPLE_ENTRY_NOT_SUPPORTED);
	
	case META_MISC_INVALID_FORMAT:
		 return M2S(META_MISC_INVALID_FORMAT);
	
	case META_FAT_FILE_LENGTH_ZERO:
		 return M2S(META_FAT_FILE_LENGTH_ZERO);
	
	case META_MISC_INI_SETTINGS_ERR_IN_PC_FOLDER_SEC:
		 return M2S(META_MISC_INI_SETTINGS_ERR_IN_PC_FOLDER_SEC);
	
	case META_PC_FILE_NOT_FOUND:
		 return M2S(META_PC_FILE_NOT_FOUND);
	
	case META_MEMBER_FIELD_TYPE_NOT_FOUND:
		 return M2S(META_MEMBER_FIELD_TYPE_NOT_FOUND);
	
	case META_MEMBER_FIELD_OFFSET_NOT_FOUND:
		 return M2S(META_MEMBER_FIELD_OFFSET_NOT_FOUND);
	
	case META_NVRAM_GET_KEY_LENGTH_CALLBACK_ERROR:
		 return M2S(META_NVRAM_GET_KEY_LENGTH_CALLBACK_ERROR);
	
	case META_NVRAM_ALLOCATE_KEY_BUFFER_ERROR:
		 return M2S(META_NVRAM_ALLOCATE_KEY_BUFFER_ERROR);
	
	case META_NVRAM_GET_KEY_BUFFER_CALLBACK_ERROR:
		 return M2S(META_NVRAM_GET_KEY_BUFFER_CALLBACK_ERROR);
	
	case META_NVRAM_KEY_ENTRY_MISSING:
		 return M2S(META_NVRAM_KEY_ENTRY_MISSING);
	
	case META_NVRAM_KEY_ENTRY_ERROR:
		 return M2S(META_NVRAM_KEY_ENTRY_ERROR);
	
	case META_NVRAM_KEY_LENGTH_EXCEEDS_INTERNAL_BUFFER:
		 return M2S(META_NVRAM_KEY_LENGTH_EXCEEDS_INTERNAL_BUFFER);
	
	case META_FAT_SPACE_SIZE_OVERFLOW:
		 return M2S(META_FAT_SPACE_SIZE_OVERFLOW);
	
	case META_LIB_INCONSISTENT:
		 return M2S(META_LIB_INCONSISTENT);
	
	case META_INTERNAL_DB_ERROR:
		 return M2S(META_INTERNAL_DB_ERROR);
	
	case META_REQUIRED_TOOL_VERSION_CHECK_FAILD:
		 return M2S(META_REQUIRED_TOOL_VERSION_CHECK_FAILD);
	
	case META_CROSS_BRANCH_BACKUP_OPEN_FAILED:
		 return M2S(META_CROSS_BRANCH_BACKUP_OPEN_FAILED);
	
	case META_CROSS_BRANCH_BACKUP_PARSE_FAILED:
		 return M2S(META_CROSS_BRANCH_BACKUP_PARSE_FAILED);
	
	case META_CROSS_BRANCH_BACKUP_FILE_ATT_NOT_MATCH:
		 return M2S(META_CROSS_BRANCH_BACKUP_FILE_ATT_NOT_MATCH);
	
	case META_NVRAM_VALUE_NOT_LEGAL:
		 return M2S(META_NVRAM_VALUE_NOT_LEGAL);
	
	case META_INIT_CONNECTION_STARTED:
		 return M2S(META_INIT_CONNECTION_STARTED);
	
	case META_QUERY_NO_MODEM_SUPPORT:
		 return M2S(META_QUERY_NO_MODEM_SUPPORT);
	
	case META_MEMBER_FIELD_SIZE_NOT_FOUND:
		 return M2S(META_MEMBER_FIELD_SIZE_NOT_FOUND);
	
	case META_LAST_RESULT:
		 return M2S(META_LAST_RESULT);

	default:
		return M2S(DEFAULT_FAIL);
	}
}

//20150728 glory add
//Fix connect with kernel comport probabilistic fail bug
int CColorBoxView::TryToOpenSPKernelComport(int KernelCom_Num)
{
    CString sInfo;
	sInfo.Format("SmartPhoneSN::TryToOpenSPKernelComport(): Try to open kernel comport until it ready, KernelCom_Num = %d...",KernelCom_Num);
	this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
    HANDLE Hcom;
    char tmp_com[30] = {0};
    sprintf(tmp_com, "\\\\.\\COM%d", KernelCom_Num);
    
    int retryTime = 0;
    //try to open kernel comport until it ready
    do
    {
        retryTime += 1;
        sInfo.Format("SmartPhoneSN::TryToOpenSPKernelComport(): Try to open kernel comport until it ready, retryTime = %d...",retryTime);
		this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
        Hcom = CreateFile(tmp_com, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (INVALID_HANDLE_VALUE != Hcom)
        {
            int Ret = CloseHandle(Hcom);
            sInfo.Format("SmartPhoneSN::TryToOpenSPKernelComport(): Open kernel comport success, retryTime = %d...",  retryTime);
			this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
            return OK;
        }
        Sleep(1000);
    }while(retryTime <= 50);
    
    sInfo.Format("SmartPhoneSN::TryToOpenSPKernelComport(): Open kernel comport fail, retryTime = %d...", retryTime);
	this->Display(sInfo.GetBuffer(sInfo.GetLength()), 3);
    return ERROR;
}
//end

void CColorBoxView::Display(CString sMsg,int index/*=3*/)
{
	InsertListInfo(sMsg, 3);
}

SPMETA_DLL::META_RESULT CColorBoxView::ExitAPMetaMode()
{
	CString sInfo;
    SPMETA_DLL::META_RESULT spMetaResult;  
    char *pFuncName = NULL;
    
    if (m_bTargetInMetaMode)
    {
        //Switch to modem meta fail from AP meta last time, so need to reconnect to AP meta mode
        if (m_eMetaMode == SP_AP2MD_FAIL)
        {
            int BootStop = 0;
            int usb_port = 0;
            SPMETA_DLL::WM_META_ConnectInMETA_Req BootRequest; 
            SPMETA_DLL::WM_META_ConnectInMETA_Report BootReport;
            
            BootRequest.com_port = m_iKernelComport;
            BootRequest.baudrate[0] = SPMETA_DLL::META_BAUD115200;
            BootRequest.baudrate[1] = SPMETA_DLL::META_BAUD230400;
            BootRequest.baudrate[2] = SPMETA_DLL::META_BAUD460800;
            BootRequest.baudrate[3] = SPMETA_DLL::META_BAUD921600;
            BootRequest.baudrate[4] = SPMETA_DLL::META_BAUD57600;
            BootRequest.baudrate[5] = SPMETA_DLL::META_BAUD_END;
            BootRequest.flowctrl    = SPMETA_DLL::META_NO_FLOWCTRL;//META_SW_FLOWCTRL;
            BootRequest.ms_connect_timeout = m_tMetaReq_Ex.ms_connect_timeout;
            
            spMetaResult = SPMETA_DLL::SP_META_ConnectInMetaMode_r(this->m_ApExe.iMetaAP_handle, &BootRequest, &BootStop, &BootReport); 
            if (META_SUCCESS != spMetaResult)
            {
                sInfo.Format("SmartPhoneSN::SP_META_ConnectInMetaMode_r(): Connect to AP meta Fail!, BootRequest.com_port = %d, MetaResult = %s", 
                    BootRequest.com_port, ResultToString_SP(spMetaResult));
				this->Display(sInfo);
            }  
        }
        
        
        spMetaResult = SPMETA_DLL::SP_META_DisconnectWithTarget_r (this->m_ApExe.iMetaAP_handle);
        pFuncName = "SP_META_DisconnectWithTarget_r()";
        if (spMetaResult == SPMETA_DLL::META_SUCCESS)
        {
            m_bTargetInMetaMode = false;
        } 
    }

    sInfo.Format("SmartPhoneSN::ExitAPMeta(): Exit meta mode by %s successfully! MetaResult = %s", pFuncName, ResultToString_SP(spMetaResult));
	this->Display(sInfo);
    return spMetaResult;
}

META_RESULT CColorBoxView::ExitSPModemMeta()
{
	CString sInfo;
    META_RESULT MetaResult = META_SUCCESS;

    //Switch to AP meta fail from modem meta last time, so need to reconnect to modem meta mode
    if (m_eMetaMode == SP_MD2AP_FAIL)
    {
        m_tMetaReq_Ex.com_port = m_iKernelComport;
        MetaResult = META_ConnectWithMultiModeTarget_r(this->m_nMetaHandle, &m_tMetaReq_Ex,sizeof(m_tMetaReq_Ex),m_pMetaStopFlag,&m_tMetaConnReport_Ex);
        if(MetaResult != META_SUCCESS)
        {        
            m_eMetaMode = SP_AP2MD_FAIL;
            sInfo.Format("SmartPhoneSN::META_ConnectWithMultiModeTarget_r():  Connect to Modem meta Fail, MetaResult = %s", ResultToString(MetaResult));
			this->Display(sInfo);
        }     
    }

    MetaResult = ModemSwithToAPMeta(); //Must switch to AP meta first, if not target can`t shutdown successfully
    if (MetaResult == META_SUCCESS)
    {
        ExitAPMetaMode();
    }
    
    return META_SUCCESS;
}

void CColorBoxView::SP_Exit_MetaModemMode_NEW()
{
	//20150731 GLORY ADD 根据当前META模式选择从AP推出还是从MODEM退出
	if (m_eMetaMode == SP_AP_META || m_eMetaMode == SP_AP2MD_FAIL)
    {
        ExitAPMetaMode();
    }
    else if(m_eMetaMode == SP_MODEM_META || m_eMetaMode == SP_MD2AP_FAIL)
    {
        ExitSPModemMeta();
    }
	//end
}
