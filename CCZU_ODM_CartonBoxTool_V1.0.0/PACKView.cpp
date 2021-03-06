// PACKView.cpp : implementation of the CPACKView class
//

#include "stdafx.h"
#include "PACK.h"
#include "Splash.h"
#include "PACKDoc.h"
#include "PACKView.h"
#include "MainFrm.h"
#include "EspeBox.h"
#include <math.h>
#include "CartonBoxPrint.h"
#include "Password.h"
#include "PopMessageBox.h"
#include "PopMainCarton.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPACKView

IMPLEMENT_DYNCREATE(CPACKView, CFormView)

BEGIN_MESSAGE_MAP(CPACKView, CFormView)
	//{{AFX_MSG_MAP(CPACKView)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_ESPE_BOX, OnBtnEspeBox)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBtnClose)
	ON_BN_CLICKED(IDC_BTN_DISCARD, OnBtnDiscard)
	ON_EN_CHANGE(IDC_EDIT_IMEI1, OnChangeEditImei1)
	ON_EN_CHANGE(IDC_EDIT_IMEI2, OnChangeEditImei2)
	ON_EN_CHANGE(IDC_EDIT_IMEI3, OnChangeEditImei3)
	ON_EN_CHANGE(IDC_EDIT_IMEI4, OnChangeEditImei4)
	ON_BN_CLICKED(IDC_BTN_PRINT, OnBtnPrint)
	ON_WM_CREATE()
	ON_COMMAND(IDM_SCAN_ALL, OnScanAll)
	ON_COMMAND(IDM_SCAN_ONE, OnScanOne)
	ON_BN_CLICKED(IDC_BTN_SETTING, OnBtnSetting)
	ON_BN_CLICKED(IDC_CHECK_PSN_STATIC, OnCheckPsnStatic)
	ON_BN_CLICKED(IDC_CHECK_ADD_Z, OnCheckAddZ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPACKView construction/destruction

CPACKView::CPACKView()
	: CFormView(CPACKView::IDD)
{
	//{{AFX_DATA_INIT(CPACKView)
	m_csCurrentProduct = _T("");
	m_csIMEI1 = _T("");
	m_csIMEI3 = _T("");
	m_csIMEI2 = _T("");
	m_csIMEI4 = _T("");
	m_csCurrentColor = _T("");
	m_csCurrentOrder = _T("");
	m_csCurrentLine = _T("");
	m_bPSNStatic = FALSE;
	m_bAddZ = FALSE;
	m_csAdd = _T("");
	//}}AFX_DATA_INIT
	m_showState=FALSE;
}

CPACKView::~CPACKView()
{
}

void CPACKView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPACKView)
	DDX_Control(pDX, IDC_EDIT_ADD, m_ctrAdd);
	DDX_Control(pDX, IDC_CHECK_ADD_Z, m_ctrAddZ);
	DDX_Control(pDX, IDC_CHECK_PSN_STATIC, m_ctrPSNStatic);
	DDX_Control(pDX, IDC_BTN_SETTING, m_ctrSetting);
	DDX_Control(pDX, IDC_STATIC_CURRENT_LINE, m_ctrCurrentLine);
	DDX_Control(pDX, IDC_STATIC_CURRENT_COUNT, m_cCurrentCount);
	DDX_Control(pDX, IDC_STATIC_CURRENT_ORDER, m_ctrCurrentOrder);
	DDX_Control(pDX, IDC_STATIC_CURRENT_COLOR, m_ctrCurrentColor);
	DDX_Control(pDX, IDC_BTN_PRINT, m_ctrBtnPrint);
	DDX_Control(pDX, IDC_BTN_ESPE_BOX, m_ctrBtnEspeBox);
	DDX_Control(pDX, IDC_BTN_DISCARD, m_ctrBtnDiscard);
	DDX_Control(pDX, IDC_BTN_CLOSE, m_ctrBtnClose);
	DDX_Control(pDX, IDC_EDIT_IMEI4, m_ctrIMEI4);
	DDX_Control(pDX, IDC_EDIT_IMEI3, m_ctrIMEI3);
	DDX_Control(pDX, IDC_EDIT_IMEI2, m_ctrIMEI2);
	DDX_Control(pDX, IDC_EDIT_IMEI1, m_ctrIMEI1);
	DDX_Control(pDX, IDC_STATIC_CURRENT_PRODUCT, m_ctrCurrentProduct);
	DDX_Control(pDX, IDC_LIST_STATE, m_state_list);
	DDX_Control(pDX, IDC_FLEX, m_flex);
	DDX_Text(pDX, IDC_STATIC_CURRENT_PRODUCT, m_csCurrentProduct);
	DDX_Text(pDX, IDC_EDIT_IMEI1, m_csIMEI1);
	DDX_Text(pDX, IDC_EDIT_IMEI3, m_csIMEI3);
	DDX_Text(pDX, IDC_EDIT_IMEI2, m_csIMEI2);
	DDX_Text(pDX, IDC_EDIT_IMEI4, m_csIMEI4);
	DDX_Text(pDX, IDC_STATIC_CURRENT_COLOR, m_csCurrentColor);
	DDX_Text(pDX, IDC_STATIC_CURRENT_ORDER, m_csCurrentOrder);
	DDX_Text(pDX, IDC_STATIC_CURRENT_LINE, m_csCurrentLine);
	DDX_Check(pDX, IDC_CHECK_PSN_STATIC, m_bPSNStatic);
	DDX_Check(pDX, IDC_CHECK_ADD_Z, m_bAddZ);
	DDX_Text(pDX, IDC_EDIT_ADD, m_csAdd);
	//}}AFX_DATA_MAP
}

BOOL CPACKView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CPACKView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	CreateTwoFile();

	//设置字体
	this->SetMyFont();
	
	//初始化ini文件
	this->m_Config.InitConfigPath();
	
	this->m_Config.ReadConfig();

	m_csCurrentOrder = ((CPACKApp *)AfxGetApp())->InputOrder;
	m_csCurrentLine  = ((CPACKApp *)AfxGetApp())->InputProductionLine;
	
	//获取项目订单信息
	if(!this->GetProductOrderInfo())
	{
		InsertListInfo("Access to project orders information fail...",2);
		return;
	}
	else
	{
		UpdateData(FALSE);
		InsertListInfo("Access to project orders information Pass...",0);
	}
	
	//建立远程共享连接
	CString csCFGPath;
	HINSTANCE hInstance;
	CString temp;
	int iRet;
	char direc[MAX_PATH];
	
	csCFGPath = this->m_Config.m_csCFGPath.Left(this->m_Config.m_csCFGPath.GetLength()-1);
	
	if (csCFGPath.Left(2) == "\\\\")
	{
		temp.Format("/c net use %s %s /user:%s", csCFGPath, this->m_Config.m_csLoginPSW, this->m_Config.m_csLoginName);
		hInstance = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(),"open","cmd.exe",temp,NULL,SW_HIDE);
		iRet = int(hInstance);
		if (iRet < 32)
		{
			MessageBox("Establish remote shared connection failure!","Error",MB_OK);
			exit(0);
		}
	}
	
	//获取当前文件路径
	GetCurrentDirectory(MAX_PATH,direc);
	m_csLocalPath = CString(direc);
	
	//删除本地ColorResource.txt文件
	DeleteFile(m_csLocalPath+"\\CartonResource.txt");
	DeleteFile(m_csLocalPath+"\\CartonResource_Sub.txt");
	
	//获取服务器打印模板文件存放路径
	csCFGPath = this->m_Config.m_csCFGPath+m_csCurrentProduct+"_"+m_csCurrentOrder+"\\config\\CartonResource.txt";
	//从服务器copy CartonResource.txt文件至本地
	temp.Format("/c xcopy %s %s /e /i /y", csCFGPath, m_csLocalPath);
	hInstance = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(),"open","cmd.exe",temp,NULL,SW_HIDE);
	iRet = int(hInstance);
	if (iRet < 32)
	{
		MessageBox("COPY CartonResource.txt File Error!","Error",MB_OK);
		exit(0);
	}

	//获取服务器打印模板文件存放路径
	csCFGPath = this->m_Config.m_csCFGPath+m_csCurrentProduct+"_"+m_csCurrentOrder+"\\config\\CartonResource_Sub.txt";
	//从服务器copy CartonResource_Sub.txt文件至本地
	temp.Format("/c xcopy %s %s /e /i /y", csCFGPath, m_csLocalPath);
	hInstance = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(),"open","cmd.exe",temp,NULL,SW_HIDE);
	iRet = int(hInstance);
	if (iRet < 32)
	{
		MessageBox("Copy CartonResource_Sub.txt File fail!","Error",MB_OK);
		exit(0);
	}
	
	//启动定时器
	this->SetTimer(1,500,0);
}

/////////////////////////////////////////////////////////////////////////////
// CPACKView diagnostics

#ifdef _DEBUG
void CPACKView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPACKView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CPACKDoc* CPACKView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPACKDoc)));
	return (CPACKDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPACKView message handlers

void CPACKView::CreateTwoFile()
{
	CFile m_file;
	m_file.Open("c:\\temp.bat",CFile::modeCreate|CFile::modeWrite);
//	m_file.Write("copy c:\\resource.txt lpt1",strlen("copy c:\\resource.txt lpt1")); //并口
	m_file.Write("print /d:com1 c:\\resource.txt",strlen("print /d:com1 c:\\resource.txt")); //串口
	m_file.Close();
	m_file.Open("c:\\resource.txt",CFile::modeCreate|CFile::modeWrite);  
	m_file.Close();
}


void CPACKView::OnTimer(UINT nIDEvent) 
{
	CString temp;

	if(nIDEvent == 1)
	{
		this->KillTimer(1);
		CFormView::OnTimer(nIDEvent);
		
		//读取配置文件
		this->m_Config.ReadConfig();

		//设置菜单勾选
		switch (this->m_Config.m_iScanMode)
		{
		case 1:
			AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_ONE, MF_BYCOMMAND | MF_CHECKED );
			break;
		case 2:
			AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_ALL, MF_BYCOMMAND | MF_CHECKED );
			break;
		default:
			this->m_Config.m_iScanMode=1;
			this->m_Config.SaveConfig();
			AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_ONE, MF_BYCOMMAND | MF_CHECKED );
			break;
		}		

		//灰掉控件
		this->SetUIDisableAll();
		
		//设置FLEX格数
		this->SetRowCol(this->m_iMaxCount+1,11);
		this->SetNumText(this->m_iMaxCount+1);
		
		//显示MACCOUNT
		CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
		temp.Format("Standard quantity: %d",this->m_iMaxCount);
		pFrame->m_wndStatusBar.SetPaneText(2,temp);
		InsertListInfo(temp,0);//调整显示位置
		
		//显示CURRENTCOUNT
		temp.Format("Current quantity: %d",0);
		pFrame->m_wndStatusBar.SetPaneText(3,temp);
		InsertListInfo(temp,0);//调整显示位置
		
		//获取当前箱号
		if(!Get_Current_Carton_Name())
		{
			InsertListInfo("Get the current project order number failed...",2);
			return;
		}
		else
		{
			InsertListInfo("Get the current project order number Pass...",0);
			temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
			pFrame->m_wndStatusBar.SetPaneText(1,temp);
			InsertListInfo(temp,0);//调整显示位置
		}
		
		//显示当前总数
		if(!this->Load_Current_Product_Count())
		{
			InsertListInfo("To obtain the number of current orders has failed...",2);
			return;
		}
		InsertListInfo("Obtain the number of current orders has been successful...",0);
		this->UpdateCount(this->m_csCurrentCount);
		
		UpdateData(FALSE);
		
		//根据IMEIType情况设置空间
		if(this->m_Config.m_iScanMode==2)//扫描所有IMEI号
		{
			switch(this->m_iIMEIType) {
			case 0:
				this->m_ctrIMEI1.EnableWindow(TRUE);
				break;
			case 1:
				this->m_ctrIMEI1.EnableWindow(TRUE);
				this->m_ctrIMEI2.EnableWindow(TRUE);
				break;
			case 2:
				this->m_ctrIMEI1.EnableWindow(TRUE);
				this->m_ctrIMEI2.EnableWindow(TRUE);
				this->m_ctrIMEI3.EnableWindow(TRUE);
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
		}
		else//扫描主IMEI号
		{
			this->m_ctrIMEI1.EnableWindow(TRUE);
		}
		
		this->m_ctrBtnEspeBox.EnableWindow();
		this->m_ctrBtnDiscard.EnableWindow();
		this->m_ctrBtnClose.EnableWindow();
		this->m_ctrBtnPrint.EnableWindow();
		
		this->m_ctrIMEI1.SetFocus();
	}
}


void CPACKView::SetMyFont()
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
	Column.cx =   600;
	m_state_list.InsertColumn(1,&Column);
	
	this->m_ctrCurrentProduct.SetFontSize(18);
	this->m_ctrCurrentProduct.SetFontName("Arial");
	this->m_ctrCurrentProduct.SetTextColor(RGB(0,0,255));
	
	this->m_ctrCurrentOrder.SetFontSize(18);
	this->m_ctrCurrentOrder.SetFontName("Bold");
	this->m_ctrCurrentOrder.SetTextColor(RGB(0,0,255));
	
	this->m_ctrCurrentColor.SetFontSize(18);
	this->m_ctrCurrentColor.SetFontName("Bold");
	this->m_ctrCurrentColor.SetTextColor(RGB(0,0,255));

	this->m_ctrCurrentLine.SetFontSize(18);
	this->m_ctrCurrentLine.SetFontName("Bold");
	this->m_ctrCurrentLine.SetTextColor(RGB(0,0,255));

	this->m_cCurrentCount.SetFontSize(30);
	this->m_cCurrentCount.SetFontName("Arial");
	this->m_cCurrentCount.SetTextColor(RGB(0,0,255));

	m_ctrBtnEspeBox.SetIcon(IDI_ICON1);
	m_ctrBtnEspeBox.SetActiveFgColor(RGB(0,0,255));

	m_ctrBtnDiscard.SetIcon(IDI_ICON3);
	m_ctrBtnDiscard.SetActiveFgColor(RGB(0,0,255));

	m_ctrBtnClose.SetIcon(IDI_ICON2);
	m_ctrBtnClose.SetActiveFgColor(RGB(0,0,255));

	m_ctrBtnPrint.SetIcon(IDI_ICON4);
	m_ctrBtnPrint.SetActiveFgColor(RGB(0,0,255));

	m_ctrSetting.SetIcon(IDI_ICON6);
	m_ctrSetting.SetActiveFgColor(RGB(0,0,255));

	VERIFY(m_staticfont.CreateFont(
		30,                        // nHeight
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
		"MS Sans Serif"));                 // lpszFacename

	CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
	pFrame->m_wndStatusBar.SetFont(&m_staticfont);
	
}


void CPACKView::SetUIDisableAll()
{
	this->m_ctrIMEI1.EnableWindow(FALSE);
	this->m_ctrIMEI2.EnableWindow(FALSE);
	this->m_ctrIMEI3.EnableWindow(FALSE);
	this->m_ctrIMEI4.EnableWindow(FALSE);
	this->m_ctrBtnEspeBox.EnableWindow(FALSE);
	this->m_ctrBtnDiscard.EnableWindow(FALSE);
	this->m_ctrBtnClose.EnableWindow(FALSE);
	this->m_ctrBtnPrint.EnableWindow(FALSE);
}


void CPACKView::InsertListInfo(CString state,int imageIndex)
{
	CString date;
	date=GetCurTimes();
	m_state_list.InsertItem(m_state_list.GetItemCount(),date,imageIndex);
	m_state_list.SetItemText(m_state_list.GetItemCount()-1,1,state);
	m_state_list.EnsureVisible(m_state_list.GetItemCount()-1,TRUE);
}


CString CPACKView::GetCurTimes()
{
	CTime t=CTime::GetCurrentTime();
	CString str=t.Format( "%Y-%m-%d  %H:%M:%S");
	return str;
}


CString CPACKView::VariantToCString(VARIANT   var)   
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


void CPACKView::SetRowCol(int rows,int cols)
{
	m_flex.SetRows(rows);
	m_flex.SetCols(cols);
	
	m_flex.SetColWidth(0,280);
	m_flex.SetColWidth(1,1700);//IMEI1
	m_flex.SetColWidth(2,1700);//IMEI2
	m_flex.SetColWidth(3,1700);//IMEI3
	m_flex.SetColWidth(4,1700);//IMEI4
	m_flex.SetColWidth(5,2000);//SN
	m_flex.SetColWidth(6,1900);//PSN
	m_flex.SetColWidth(7,1500);//BT
	m_flex.SetColWidth(8,1500);//MAC
	m_flex.SetColWidth(9,1500);//Weigh
	m_flex.SetColWidth(10,1700);//NetCode

	//reset current row and col
	this->m_cur_row=1;
	this->m_cur_col=1;
}


void CPACKView::SetNumText(int rows)
{
	//设置FLEX居中
	for(int row=0;row<rows;row++)
	{
		m_flex.SetRow(row);
		m_flex.SetCol(1);
		m_flex.SetCellAlignment(4);
		
		m_flex.SetRow(row);
		m_flex.SetCol(2);
		m_flex.SetCellAlignment(4);
		
		m_flex.SetRow(row);
		m_flex.SetCol(3);
		m_flex.SetCellAlignment(4);

		m_flex.SetRow(row);
		m_flex.SetCol(4);
		m_flex.SetCellAlignment(4);
		
		m_flex.SetRow(row);
		m_flex.SetCol(5);
		m_flex.SetCellAlignment(4);
		
		m_flex.SetRow(row);
		m_flex.SetCol(6);
		m_flex.SetCellAlignment(4);
		
		m_flex.SetRow(row);
		m_flex.SetCol(7);
		m_flex.SetCellAlignment(4);
		
		m_flex.SetRow(row);
		m_flex.SetCol(8);
		m_flex.SetCellAlignment(4);

		m_flex.SetRow(row);
		m_flex.SetCol(9);
		m_flex.SetCellAlignment(4);

		m_flex.SetRow(row);
		m_flex.SetCol(10);
		m_flex.SetCellAlignment(4);
	}
	
	//设置序列号
	CString strT;
	for(int count=1;count<rows;count++)
	{
		strT.Format("%d",count);
		m_flex.SetTextMatrix(count,0,strT);
	}
	
	m_flex.SetTextMatrix(0,1,"IMEI1");
	m_flex.SetTextMatrix(0,2,"IMEI2");
	m_flex.SetTextMatrix(0,3,"IMEI3");
	m_flex.SetTextMatrix(0,4,"IMEI4");
	m_flex.SetTextMatrix(0,5,"SN");
	m_flex.SetTextMatrix(0,6,"PSN");
	m_flex.SetTextMatrix(0,7,"BT");
	m_flex.SetTextMatrix(0,8,"MAC");
	m_flex.SetTextMatrix(0,9,"Weigh");
	m_flex.SetTextMatrix(0,10,"NetCode");
}


bool CPACKView::Get_Current_Carton_Name()
{
	CString temp;
	_variant_t var;
	CString sql;

	UpdateData();
	
	try
	{
		sql.Format("select * from %s_%s_CartonBox where status != 0 ORDER BY CartonName DESC",this->m_csCurrentProduct,this->m_csCurrentOrder);//@@ Call storage course @@
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		
		if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF) //空数据
		{
			if(m_csCartonNameBegin.GetLength()>1 && m_csCartonNameBegin.GetLength()==m_iCartonNameLength)
			{
				m_csCurrent_Carton=m_csCartonNameBegin;
				if(m_bAddZ)
				{
					m_csCurrent_Carton += m_csAdd;
				}
			}
			else
			{
				CString cur="                       ";
				for(int counts=0;counts<m_iCartonNameLength-1;counts++)
				{
					cur.SetAt(counts,'0');
				}
				cur.SetAt(m_iCartonNameLength-1,'1');
				m_csCurrent_Carton=cur.Left(m_iCartonNameLength);
				if(m_bAddZ)
				{
					m_csCurrent_Carton += m_csAdd;
				}
			}
		}
		else
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("CartonName");
			temp=VariantToCString(var);
			m_csCurrent_Carton=temp;
			CurrentAdd(m_csCurrent_Carton,10);
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}	

	return TRUE;
}


//每次扫入第一个数据时重新查找箱号
bool CPACKView::ReGet_Current_Carton_Name()
{
	CString temp;
	_variant_t var;
	CString sql;

	UpdateData();
	
	try
	{
		sql.Format("select * from %s_%s_CartonBox where status != 0 ORDER BY CartonName DESC",this->m_csCurrentProduct,this->m_csCurrentOrder);//@@ Call storage course @@
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		
		if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF) //空数据
		{
			if(m_csCartonNameBegin.GetLength()>1 && m_csCartonNameBegin.GetLength()==m_iCartonNameLength)
			{
				m_csReGet_Current_Carton=m_csCartonNameBegin;
				if (m_bAddZ)
				{
					m_csReGet_Current_Carton += m_csAdd;
				}
			}
			else
			{	
				CString cur="                       ";
				for(int counts=0;counts<m_iCartonNameLength-1;counts++)
				{
					cur.SetAt(counts,'0');
				}
				cur.SetAt(m_iCartonNameLength-1,'1');
				m_csReGet_Current_Carton=cur.Left(m_iCartonNameLength);
				if (m_bAddZ)
				{
					m_csReGet_Current_Carton += m_csAdd;
				}
			}
		}
		else
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("CartonName");
			temp=VariantToCString(var);
			m_csReGet_Current_Carton=temp;
			CurrentAdd(m_csReGet_Current_Carton,10);
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}	

	return TRUE;
}


void CPACKView::CurrentAdd(CString& current,UINT step) 
{
	UpdateData();

	if (m_bAddZ)
	{
		current = current.Left(current.GetLength()-1);
	}
	
	int i = current.GetLength();
	long l = strtol((LPSTR)(LPCTSTR)current, NULL, step); 
	l++; //增1;
	char buf[33];
	ltoa(l, buf, step);
	CString fmt; fmt.Format("%%0%ds", i); 
	current.Format(fmt, buf); 
	current.MakeUpper();

	if (m_bAddZ)
	{
		current += m_csAdd;
	}
} 


void CPACKView::OnBtnEspeBox() 
{
	CPassword psw;
	psw.ilevel = 0; //需普通权限验证
	if(IDOK!=psw.DoModal())
		return;

	CEspeBox dlg;
	dlg.m_csCartonNameStatic=this->m_csCartonNameStatic;
	if(IDOK!=dlg.DoModal())
		return;

	CString temp;
	_variant_t var;
	CString sql;
	double dWeigh;

	try
	{
		sql.Format("select * from %s_%s_CartonBox where CartonName='%s' and Status=1",this->m_csCurrentProduct,this->m_csCurrentOrder,dlg.m_csEspeBoxName);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF) //空数据
		{
			CString strMsg;
			strMsg.Format("The container number is invalid or has been sealed box \'%s%s\' !",this->m_csCartonNameStatic,dlg.m_csEspeBoxName);
			InsertListInfo(strMsg,2);
		}
		else
		{
			//获取当前箱号以备后续继续插入使用
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("CartonName");
			temp=VariantToCString(var);
			m_csCurrent_Carton=temp;
			
			//显示当前箱号
			CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
			temp.Format("The current No. : %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
			pFrame->m_wndStatusBar.SetPaneText(1,temp);
			InsertListInfo(temp,0);//lucky 调整显示位置

			//========================未封箱处理====================
			this->m_csaIMEI1.RemoveAll();
			this->m_csaIMEI2.RemoveAll();
			this->m_csaIMEI3.RemoveAll();
			this->m_csaIMEI4.RemoveAll();
			this->m_csaSN.RemoveAll();
			this->m_csaPSN.RemoveAll();
			this->m_csaBT.RemoveAll();
			this->m_csaMAC.RemoveAll();
			this->m_csaWeigh.RemoveAll();
			this->m_csaNet.RemoveAll();

			this->m_flex.Clear();
			SetNumText(m_flex.GetRows());

			//=================获取所有信息并保存到Array中============
			sql.Format("select * from %s_%s_ColorBox where CartonName='%s' and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,dlg.m_csEspeBoxName);
			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
			((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
			while(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI1");
				temp=VariantToCString(var);
				m_csaIMEI1.Add(temp);
				
				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI2");
				temp=VariantToCString(var);
				m_csaIMEI2.Add(temp);
				
				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI3");
				temp=VariantToCString(var);
				m_csaIMEI3.Add(temp);

				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI4");
				temp=VariantToCString(var);
				m_csaIMEI4.Add(temp);

				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BarCode");
				temp=VariantToCString(var);
				m_csaSN.Add(temp);
				
				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ProductSN");
				temp=VariantToCString(var);
				m_csaPSN.Add(temp);

				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BTAddress");
				temp=VariantToCString(var);
				m_csaBT.Add(temp);	

				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("WIFIMAC");
				temp=VariantToCString(var);
				m_csaMAC.Add(temp);

				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ColorBoxWeigh");
				temp=VariantToCString(var);
				m_csaWeigh.Add(temp);

				var=((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("NetCode");
				temp=VariantToCString(var);
				m_csaNet.Add(temp);
				
				((CPACKApp *)AfxGetApp())->m_pRst->MoveNext();
			}

			//插入信息到flex
			for(int i=0 ; i<m_csaIMEI1.GetSize() ;i++)
			{
				m_flex.SetTextMatrix(i+1,1,m_csaIMEI1.GetAt(i));
				m_flex.SetTextMatrix(i+1,2,m_csaIMEI2.GetAt(i));
				m_flex.SetTextMatrix(i+1,3,m_csaIMEI3.GetAt(i));
				m_flex.SetTextMatrix(i+1,4,m_csaIMEI4.GetAt(i));
				m_flex.SetTextMatrix(i+1,5,m_csaSN.GetAt(i));
				m_flex.SetTextMatrix(i+1,6,m_csaPSN.GetAt(i));
				m_flex.SetTextMatrix(i+1,7,m_csaBT.GetAt(i));
				m_flex.SetTextMatrix(i+1,8,m_csaMAC.GetAt(i));
				m_flex.SetTextMatrix(i+1,10,m_csaNet.GetAt(i));

				dWeigh=atof((char*)(LPCTSTR)m_csaWeigh.GetAt(i));
				temp.Format("%.1lf",dWeigh);
				m_flex.SetTextMatrix(i+1,9,temp);
			}

			//设置当前行数
			m_cur_row=m_csaIMEI1.GetSize()+1;

			//设置当前箱号数量
			temp.Format("The current No.:%d",m_csaIMEI1.GetSize());
			pFrame->m_wndStatusBar.SetPaneText(3,temp);
			InsertListInfo(temp,0);//lucky 调整显示位置

			temp.Format("The specific number of \'%s%s\' call success !", this->m_csCartonNameStatic,dlg.m_csEspeBoxName);
			InsertListInfo(temp,1);	
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
	}
}


void CPACKView::OnBtnClose() 
{
	CPassword psw;
	psw.ilevel = 1; //需超级权限验证
	if(IDOK!=psw.DoModal())
		return;

	if(m_csaIMEI1.GetSize()==0)
	{
		InsertListInfo("The packing quantity is not sealing or using a specific number call after sealing...",2);
		return;
	}

	if(IDYES!=MessageBox("Are you sure you want to close this box?","Remind",MB_ICONQUESTION |MB_YESNO))
	{
		return;
	}

	CString temp;
	_variant_t var;
	CString sql;

	//Update block_id to 1
	try
	{
		sql.Format("UPDATE %s_%s_CartonBox SET Status=2 where CartonName='%s'",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csCurrent_Carton);
		((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		return;
	}

	this->m_csaIMEI1.RemoveAll();
	this->m_csaIMEI2.RemoveAll();
	this->m_csaIMEI3.RemoveAll();
	this->m_csaIMEI4.RemoveAll();
	this->m_csaSN.RemoveAll();
	this->m_csaPSN.RemoveAll();
	this->m_csaBT.RemoveAll();
	this->m_csaMAC.RemoveAll();
	this->m_csaWeigh.RemoveAll();
	this->m_csaNet.RemoveAll();
	
	this->m_flex.Clear();
	SetNumText(m_flex.GetRows()); 
	m_cur_row=1;

	CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
	temp.Format("Current quantity:%d",0);
	pFrame->m_wndStatusBar.SetPaneText(3,temp);
	InsertListInfo(temp,0);//调整显示位置
	
	temp.Format("\'%s\' Successfully sealed box!",m_csCurrent_Carton);
	InsertListInfo(temp,0);
	
	//获取当前箱号
	if(!Get_Current_Carton_Name())
	{
		InsertListInfo("The current number of failure to obtain the current project...!",0);
	}
	else
	{
		temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
		pFrame->m_wndStatusBar.SetPaneText(1,temp);
		InsertListInfo(temp,0);//调整显示位置
	}
}


void CPACKView::OnBtnDiscard() 
{
	CPassword psw;
	psw.ilevel = 1; //需超级权限验证
	if(IDOK!=psw.DoModal())
		return;

	CEspeBox dlg;
	dlg.m_csCartonNameStatic=this->m_csCartonNameStatic;
	if(IDOK!=dlg.DoModal())
		return;

	CString temp;
	_variant_t var;
	CString sql;

	//检查箱号是否存在
	try
	{
		sql.Format("select * from %s_%s_CartonBox where CartonName='%s'",this->m_csCurrentProduct,this->m_csCurrentOrder,dlg.m_csEspeBoxName);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			temp.Format("Invalid number\'%s%s\' !",this->m_csCartonNameStatic,dlg.m_csEspeBoxName);
			InsertListInfo(temp,2);
			return;
		}
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		return;
	}

	if(IDYES!=MessageBox("Are you sure you want to scrap this box?","Remind",MB_ICONQUESTION |MB_YESNO))
	{
		return;
	}

	//报废箱号
	try
	{
		sql.Format("UPDATE %s_%s_CartonBox SET Status=0 where CartonName='%s'",this->m_csCurrentProduct,this->m_csCurrentOrder,dlg.m_csEspeBoxName);
		((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		return;
	}

	try
	{
		sql.Format("UPDATE %s_%s_ColorBox SET CartonName=NULL where CartonName='%s'",this->m_csCurrentProduct,this->m_csCurrentOrder,dlg.m_csEspeBoxName);
		((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		return;
	}

	//当前箱号报废重置参数
	if(dlg.m_csEspeBoxName==this->m_csCurrent_Carton)
	{
		this->m_csaIMEI1.RemoveAll();
		this->m_csaIMEI2.RemoveAll();
		this->m_csaIMEI3.RemoveAll();
		this->m_csaIMEI4.RemoveAll();
		this->m_csaSN.RemoveAll();
		this->m_csaPSN.RemoveAll();
		this->m_csaBT.RemoveAll();
		this->m_csaMAC.RemoveAll();
		this->m_csaWeigh.RemoveAll();
		this->m_csaNet.RemoveAll();
		
		this->m_flex.Clear();
		SetNumText(m_flex.GetRows());
		m_cur_row=1;

		CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
		temp.Format("Current quantity:%d",0);
		pFrame->m_wndStatusBar.SetPaneText(3,temp);
		InsertListInfo(temp,0);//调整显示位置

		//获取当前箱号
		if(!Get_Current_Carton_Name())
		{
			InsertListInfo("The current number of failure to obtain the current project...!",0);
		}
		else
		{
			temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
			pFrame->m_wndStatusBar.SetPaneText(1,temp);
			InsertListInfo(temp,0);//调整显示位置
		}
	}

	temp.Format("\'%s\' Has been successfully scrapped!",dlg.m_csEspeBoxName);
	InsertListInfo(temp,0);
}


void CPACKView::calculate_15_digit_imei( char *imei_14,char &calculate_digit)
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


void CPACKView::OnChangeEditImei1() 
{
	UpdateData();
	CString temp;
	double dWeigh;

	if(this->m_Config.m_iScanMode==1)
	{
		if(m_csIMEI1.Right(2)=="\r\n")
		{
			m_csIMEI1.Replace("\r\n","");
			UpdateData(FALSE);
			//判断IMEI1合法性
			if(!IMEITYPE_0_CheckImei1_Ex())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			else
			{
				InsertListInfo("IMEI Legitimate!",0);
			}
			
			//插入数据库
			if(!this->InsertDataToSQL())
			{
				PopMessageBox_Ex();
				return;
			}
			
			//显示到FLEX
			m_flex.SetTextMatrix(m_cur_row,1,m_csIMEI1);
			m_flex.SetTextMatrix(m_cur_row,2,m_csIMEI2_Ex);
			m_flex.SetTextMatrix(m_cur_row,3,m_csIMEI3_Ex);
			m_flex.SetTextMatrix(m_cur_row,4,m_csIMEI4_Ex);
			m_flex.SetTextMatrix(m_cur_row,5,m_csSN);
			m_flex.SetTextMatrix(m_cur_row,6,m_csPSN);
			m_flex.SetTextMatrix(m_cur_row,7,m_csBT);
			m_flex.SetTextMatrix(m_cur_row,8,m_csMAC);
			m_flex.SetTextMatrix(m_cur_row,10,m_csNet);
			
			dWeigh=atof((char*)(LPCTSTR)this->m_csWeigh);
			m_csWeigh.Format("%.1lf",dWeigh);
			m_flex.SetTextMatrix(m_cur_row,9,m_csWeigh);

			m_csIMEI1.Empty();
			m_csIMEI2_Ex.Empty();
			m_csIMEI3_Ex.Empty();
			m_csIMEI4_Ex.Empty();
			m_csSN.Empty();
			m_csPSN.Empty();
			m_csBT.Empty();
			m_csMAC.Empty();
			m_csWeigh.Empty();
			m_csNet.Empty();
			UpdateData(FALSE);	
			this->m_ctrIMEI1.SetFocus();
			m_cur_row++;
			CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
			temp.Format("Current quantity:%d",m_cur_row-1);
			pFrame->m_wndStatusBar.SetPaneText(3,temp);
			InsertListInfo(temp,0);//调整显示位置

			//显示当前总数
			if(!this->Load_Current_Product_Count())
			{
				InsertListInfo("To obtain the number of current orders has failed",2);
				return;
			}
			InsertListInfo("Obtain the number of current orders has been successful",0);
			this->UpdateCount(this->m_csCurrentCount);
			
			//检测是否满箱子
			if(m_cur_row-1 == this->m_iMaxCount)
			{
				//更新Status值=2
				if(!UpdateStatusTo2())
				{
					InsertListInfo("Failed to seal the box, please contact the Engineer...",2);
				}
				
				m_flex.Clear();
				SetNumText(m_flex.GetRows());
				m_cur_col=1;
				m_cur_row=1;
				
				temp.Format("Carton: \'%s\' Sealed box!",this->m_csCurrent_Carton);
				InsertListInfo(temp,0);
				PopMainCarton_Ex(temp);
				OnBtnPrint();//弹出打印框
				temp.Format("Current quantity:%d",0);
				pFrame->m_wndStatusBar.SetPaneText(3,temp);
				InsertListInfo(temp,0);//调整显示位置
				
				//获取当前箱号
				if(!Get_Current_Carton_Name())
				{
					InsertListInfo("The current number of failure to obtain the current project...!",0);
				}
				else
				{
					temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
					pFrame->m_wndStatusBar.SetPaneText(1,temp);
					InsertListInfo(temp,0);//调整显示位置
				}
				
				//显示当前总数
				if(!this->Load_Current_Product_Count())
				{
					InsertListInfo("To obtain the number of current orders has failed",2);
					return;
				}
				InsertListInfo("Obtain the number of current orders has been successful",0);
				this->UpdateCount(this->m_csCurrentCount);
			}			
		}

		return;
	}

	switch(this->m_iIMEIType)
	{
	case 0:
		if(m_csIMEI1.Right(2)=="\r\n")
		{
			m_csIMEI1.Replace("\r\n","");
			UpdateData(FALSE);
			//判断IMEI1合法性
			if(!IMEITYPE_0_CheckImei1_Ex())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			else
			{
				InsertListInfo("IMEI Legitimate!",0);
			}
			
			//插入数据库
			if(!this->InsertDataToSQL())
			{
				PopMessageBox_Ex();
				return;
			}

			//显示到FLEX
			m_flex.SetTextMatrix(m_cur_row,1,m_csIMEI1);
			m_flex.SetTextMatrix(m_cur_row,5,m_csSN);
			m_flex.SetTextMatrix(m_cur_row,6,m_csPSN);
			m_flex.SetTextMatrix(m_cur_row,7,m_csBT);
			m_flex.SetTextMatrix(m_cur_row,8,m_csMAC);
			m_flex.SetTextMatrix(m_cur_row,10,m_csNet);

			dWeigh=atof((char*)(LPCTSTR)this->m_csWeigh);
			m_csWeigh.Format("%.1lf",dWeigh);
			m_flex.SetTextMatrix(m_cur_row,9,m_csWeigh);

			m_csIMEI1.Empty();
			m_csSN.Empty();
			m_csPSN.Empty();
			m_csBT.Empty();
			m_csMAC.Empty();
			m_csWeigh.Empty();
			m_csNet.Empty();

			UpdateData(FALSE);
			this->m_ctrIMEI1.SetFocus();
			m_cur_row++;
			CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
			temp.Format("Current quantity:%d",m_cur_row-1);
			pFrame->m_wndStatusBar.SetPaneText(3,temp);
			InsertListInfo(temp,0);//调整显示位置

			//显示当前总数
			if(!this->Load_Current_Product_Count())
			{
				InsertListInfo("To obtain the number of current orders has failed",2);
				return;
			}
			InsertListInfo("Obtain the number of current orders has been successful",0);
			this->UpdateCount(this->m_csCurrentCount);
			
			//检测是否满箱子
			if( m_cur_row-1 == this->m_iMaxCount)
			{
				//更新Status值=2
				if(!UpdateStatusTo2())
				{
					InsertListInfo("Failed to seal the box, please contact the Engineer...",2);
				}

				m_flex.Clear();
				SetNumText(m_flex.GetRows());
				m_cur_col=1;
				m_cur_row=1;
				
				temp.Format("Carton: \'%s\' Sealed box!",this->m_csCurrent_Carton);
				InsertListInfo(temp,0);
				PopMainCarton_Ex(temp);
				OnBtnPrint();//弹出打印框
				temp.Format("Current quantity:%d",0);
				pFrame->m_wndStatusBar.SetPaneText(3,temp);
				InsertListInfo(temp,0);//调整显示位置

				//获取当前箱号
				if(!Get_Current_Carton_Name())
				{
					InsertListInfo("The current number of failure to obtain the current project...!",0);
				}
				else
				{
					temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
					pFrame->m_wndStatusBar.SetPaneText(1,temp);
					InsertListInfo(temp,0);//调整显示位置
				}
			}			
		}
		break;
	case 1:
		if(m_csIMEI1.Right(2)=="\r\n")
		{
			m_csIMEI1.Replace("\r\n","");
			UpdateData(FALSE);
			if(!IMEITYPE_1_CheckImei1())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			this->m_ctrIMEI2.SetFocus();
			this->m_ctrIMEI2.SetSel(0,-1);
		}
		break;
	case 2:
		if(m_csIMEI1.Right(2)=="\r\n")
		{
			m_csIMEI1.Replace("\r\n","");
			UpdateData(FALSE);
			if(!IMEITYPE_1_CheckImei1())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			this->m_ctrIMEI2.SetFocus();
			this->m_ctrIMEI2.SetSel(0,-1);
		}
		break;
	case 3:
		if(m_csIMEI1.Right(2)=="\r\n")
		{
			m_csIMEI1.Replace("\r\n","");
			UpdateData(FALSE);
			if(!IMEITYPE_1_CheckImei1())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			this->m_ctrIMEI2.SetFocus();
			this->m_ctrIMEI2.SetSel(0,-1);
		}
		break;
	default:
		break;
	}
}


void CPACKView::OnChangeEditImei2() 
{
	UpdateData();
	CString temp;
	double dWeigh;

	switch(this->m_iIMEIType)
	{
	case 1:
		if(m_csIMEI2.Right(2)=="\r\n")
		{
			m_csIMEI2.Replace("\r\n","");
			UpdateData(FALSE);
			if(!IMEITYPE_1_CheckImei2())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			else
			{
				InsertListInfo("IMEI Legitimate!",0);
			}

			if(!this->InsertDataToSQL())
			{
				PopMessageBox_Ex();
				return;
			}
				
			//显示到FLEX
			m_flex.SetTextMatrix(m_cur_row,1,m_csIMEI1);
			m_flex.SetTextMatrix(m_cur_row,2,m_csIMEI2);
			m_flex.SetTextMatrix(m_cur_row,5,m_csSN);
			m_flex.SetTextMatrix(m_cur_row,6,m_csPSN);
			m_flex.SetTextMatrix(m_cur_row,7,m_csBT);
			m_flex.SetTextMatrix(m_cur_row,8,m_csMAC);
			m_flex.SetTextMatrix(m_cur_row,10,m_csNet);

			dWeigh=atof((char*)(LPCTSTR)this->m_csWeigh);
			m_csWeigh.Format("%.1lf",dWeigh);
			m_flex.SetTextMatrix(m_cur_row,9,m_csWeigh);

			m_csIMEI1.Empty();
			m_csIMEI2.Empty();
			m_csSN.Empty();
			m_csPSN.Empty();
			m_csBT.Empty();
			m_csMAC.Empty();
			m_csWeigh.Empty();
			m_csNet.Empty();

			UpdateData(FALSE);	
			this->m_ctrIMEI1.SetFocus();
			m_cur_row++;
			CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
			temp.Format("Current quantity:%d",m_cur_row-1);
			pFrame->m_wndStatusBar.SetPaneText(3,temp);
			InsertListInfo(temp,0);//调整显示位置

			//显示当前总数
			if(!this->Load_Current_Product_Count())
			{
				InsertListInfo("To obtain the number of current orders has failed",2);
				return;
			}
			InsertListInfo("Obtain the number of current orders has been successful",0);
			this->UpdateCount(this->m_csCurrentCount);
			
			//检测是否满箱子
			if( m_cur_row-1 == this->m_iMaxCount)
			{
				//更新Status值=2
				if(!UpdateStatusTo2())
				{
					InsertListInfo("Failed to seal the box, please contact the Engineer...",2);
				}
				
				m_flex.Clear();
				SetNumText(m_flex.GetRows());
				m_cur_col=1;  
				m_cur_row=1;

				temp.Format("Carton: \'%s\' Sealed box!",this->m_csCurrent_Carton);
				InsertListInfo(temp,0);
				PopMainCarton_Ex(temp);
				OnBtnPrint();//弹出打印框
				temp.Format("Current quantity:%d",0);
				pFrame->m_wndStatusBar.SetPaneText(3,temp);
				InsertListInfo(temp,0);//调整显示位置
				
				//获取当前箱号
				if(!Get_Current_Carton_Name())
				{
					InsertListInfo("The current number of failure to obtain the current project...!",0);
				}
				else
				{
					temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
					pFrame->m_wndStatusBar.SetPaneText(1,temp);
					InsertListInfo(temp,0);//调整显示位置
				}
				
				//显示当前总数
				if(!this->Load_Current_Product_Count())
				{
					InsertListInfo("To obtain the number of current orders has failed",2);
					return;
				}
				InsertListInfo("Obtain the number of current orders has been successful",0);
				this->UpdateCount(this->m_csCurrentCount);
			}
		}
		break;
	case 2:
		if(m_csIMEI2.Right(2)=="\r\n")
		{
			m_csIMEI2.Replace("\r\n","");
			UpdateData(FALSE);
			if(!IMEITYPE_2_CheckImei2())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			this->m_ctrIMEI3.SetFocus();
			this->m_ctrIMEI3.SetSel(0,-1);			
		}
		break;
	case 3:
		if(m_csIMEI2.Right(2)=="\r\n")
		{
			m_csIMEI2.Replace("\r\n","");
			UpdateData(FALSE);
			if(!IMEITYPE_2_CheckImei2())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			this->m_ctrIMEI3.SetFocus();
			this->m_ctrIMEI3.SetSel(0,-1);			
		}
		break;
	default:
		break;
	}
}


void CPACKView::OnChangeEditImei3() 
{
	UpdateData();
	CString temp;
	double dWeigh;

	switch(this->m_iIMEIType)
	{
	case 2:
		if(m_csIMEI3.Right(2)=="\r\n")
		{
			m_csIMEI3.Replace("\r\n","");
			UpdateData(FALSE);
			if(!IMEITYPE_2_CheckImei3())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			else
			{
				InsertListInfo("IMEI Legitimate!",0);
			}

			if(!this->InsertDataToSQL())
			{
				PopMessageBox_Ex();
				return;
			}
			
			//显示到FLEX
			m_flex.SetTextMatrix(m_cur_row,1,m_csIMEI1);
			m_flex.SetTextMatrix(m_cur_row,2,m_csIMEI2);
			m_flex.SetTextMatrix(m_cur_row,3,m_csIMEI3);
			m_flex.SetTextMatrix(m_cur_row,5,m_csSN);
			m_flex.SetTextMatrix(m_cur_row,6,m_csPSN);
			m_flex.SetTextMatrix(m_cur_row,7,m_csBT);
			m_flex.SetTextMatrix(m_cur_row,8,m_csMAC);
			m_flex.SetTextMatrix(m_cur_row,10,m_csNet);

			dWeigh=atof((char*)(LPCTSTR)this->m_csWeigh);
			m_csWeigh.Format("%.1lf",dWeigh);
			m_flex.SetTextMatrix(m_cur_row,9,m_csWeigh);

			m_csIMEI1.Empty();
			m_csIMEI2.Empty();
			m_csIMEI3.Empty();
			m_csSN.Empty();
			m_csPSN.Empty();
			m_csBT.Empty();
			m_csMAC.Empty();
			m_csWeigh.Empty();
			m_csNet.Empty();

			UpdateData(FALSE);	
			this->m_ctrIMEI1.SetFocus();
			m_cur_row++;
			CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
			temp.Format("Current quantity:%d",m_cur_row-1);
			pFrame->m_wndStatusBar.SetPaneText(3,temp);
			InsertListInfo(temp,0);//调整显示位置

			//显示当前总数
			if(!this->Load_Current_Product_Count())
			{
				InsertListInfo("To obtain the number of current orders has failed",2);
				return;
			}
			InsertListInfo("Obtain the number of current orders has been successful",0);
			this->UpdateCount(this->m_csCurrentCount);
			
			//检测是否满箱子
			if( m_cur_row-1 == this->m_iMaxCount)
			{
				//更新Status值=2
				if(!UpdateStatusTo2())
				{
					InsertListInfo("Failed to seal the box, please contact the Engineer...",2);
				}
				
				m_flex.Clear();
				SetNumText(m_flex.GetRows());
				m_cur_col=1;  
				m_cur_row=1;
				
				temp.Format("Carton: \'%s\' Sealed box!",this->m_csCurrent_Carton);
				InsertListInfo(temp,0);
				PopMainCarton_Ex(temp);
				OnBtnPrint();//弹出打印框
				temp.Format("Current quantity:%d",0);
				pFrame->m_wndStatusBar.SetPaneText(3,temp);
				InsertListInfo(temp,0);//调整显示位置
				
				//获取当前箱号
				if(!Get_Current_Carton_Name())
				{
					InsertListInfo("The current number of failure to obtain the current project...!",0);
				}
				else
				{
					temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
					pFrame->m_wndStatusBar.SetPaneText(1,temp);
					InsertListInfo(temp,0);//调整显示位置
				}

				//显示当前总数
				if(!this->Load_Current_Product_Count())
				{
					InsertListInfo("To obtain the number of current orders has failed",2);
					return;
				}
				InsertListInfo("Obtain the number of current orders has been successful",0);
				this->UpdateCount(this->m_csCurrentCount);
			}
		}
		break;
	case 3:
		if(m_csIMEI3.Right(2)=="\r\n")
		{
			m_csIMEI3.Replace("\r\n","");
			UpdateData(FALSE);
			if(!IMEITYPE_3_CheckImei3())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			this->m_ctrIMEI4.SetFocus();
			this->m_ctrIMEI4.SetSel(0,-1);			
		}
		break;
	default:
		break;
	}
}


void CPACKView::OnChangeEditImei4() 
{
	UpdateData();
	CString temp;
	double dWeigh;

	switch(this->m_iIMEIType)
	{
	case 3:
		if(m_csIMEI4.Right(2)=="\r\n")
		{
			m_csIMEI4.Replace("\r\n","");
			UpdateData(FALSE);
			if(!IMEITYPE_3_CheckImei4())
			{
				m_ctrIMEI1.SetFocus();
				m_ctrIMEI1.SetSel(0,-1);
				PopMessageBox_Ex();
				return;
			}
			else
			{
				InsertListInfo("IMEI Legitimate!",0);
			}
			
			if(!this->InsertDataToSQL())
			{
				PopMessageBox_Ex();
				return;
			}
			
			//显示到FLEX
			m_flex.SetTextMatrix(m_cur_row,1,m_csIMEI1);
			m_flex.SetTextMatrix(m_cur_row,2,m_csIMEI2);
			m_flex.SetTextMatrix(m_cur_row,3,m_csIMEI3);
			m_flex.SetTextMatrix(m_cur_row,4,m_csIMEI4);
			m_flex.SetTextMatrix(m_cur_row,5,m_csSN);
			m_flex.SetTextMatrix(m_cur_row,6,m_csPSN);
			m_flex.SetTextMatrix(m_cur_row,7,m_csBT);
			m_flex.SetTextMatrix(m_cur_row,8,m_csMAC);
			m_flex.SetTextMatrix(m_cur_row,10,m_csNet);

			dWeigh=atof((char*)(LPCTSTR)this->m_csWeigh);
			m_csWeigh.Format("%.1lf",dWeigh);
			m_flex.SetTextMatrix(m_cur_row,9,m_csWeigh);

			m_csIMEI1.Empty();
			m_csIMEI2.Empty();
			m_csIMEI3.Empty();
			m_csIMEI4.Empty();
			m_csSN.Empty();
			m_csPSN.Empty();
			m_csBT.Empty();
			m_csMAC.Empty();
			m_csWeigh.Empty();
			m_csNet.Empty();

			UpdateData(FALSE);	
			this->m_ctrIMEI1.SetFocus();
			m_cur_row++;
			CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
			temp.Format("Current quantity:%d",m_cur_row-1);
			pFrame->m_wndStatusBar.SetPaneText(3,temp);
			InsertListInfo(temp,0);//调整显示位置
			
			//显示当前总数
			if(!this->Load_Current_Product_Count())
			{
				InsertListInfo("To obtain the number of current orders has failed",2);
				return;
			}
			InsertListInfo("Obtain the number of current orders has been successful",0);
			this->UpdateCount(this->m_csCurrentCount);
			
			//检测是否满箱子
			if( m_cur_row-1 == this->m_iMaxCount)
			{
				//更新Status值=2
				if(!UpdateStatusTo2())
				{
					InsertListInfo("Failed to seal the box, please contact the Engineer...",2);
				}
				
				m_flex.Clear();
				SetNumText(m_flex.GetRows());
				//reset current row and col
				m_cur_col=1;  
				m_cur_row=1;
				
				temp.Format("Carton: \'%s\' Sealed box!",this->m_csCurrent_Carton);
				InsertListInfo(temp,0);
				PopMainCarton_Ex(temp);
				OnBtnPrint();//弹出打印框
				temp.Format("Current quantity:%d",0);
				pFrame->m_wndStatusBar.SetPaneText(3,temp);
				InsertListInfo(temp,0);//调整显示位置
				
				//获取当前箱号
				if(!Get_Current_Carton_Name())
				{
					InsertListInfo("The current number of failure to obtain the current project...!",0);
				}
				else
				{
					temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
					pFrame->m_wndStatusBar.SetPaneText(1,temp);
					InsertListInfo(temp,0);//调整显示位置
				}

				//显示当前总数
				if(!this->Load_Current_Product_Count())
				{
					InsertListInfo("To obtain the number of current orders has failed",2);
					return;
				}
				InsertListInfo("Obtain the number of current orders has been successful",0);
				this->UpdateCount(this->m_csCurrentCount);
			}			
		}
		break;
	default:
		break;
	}
}


bool CPACKView::IMEITYPE_0_CheckImei1()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	//检测长度是否为15位
	if(m_csIMEI1.GetLength()!=15)
	{
		InsertListInfo("IMEI1 Error length...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI1.Left(14)>m_imei1_end || m_csIMEI1.Left(14)<m_imei1_start)
		{
			InsertListInfo("IMEI1 is out of range...",2);
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
			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
			((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI1 is not in IMEI form...",2);
				return FALSE;
			}

			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
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
		InsertListInfo("Illegal IMEI, check bit error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//检测SQL内是否重复 
	try
	{
		sql.Format("select * from %s_%s_CartonBox where IMEI1='%s' and Status<>0",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1);  //@@ Call storage course @@
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("carton_name"); 
			temp=VariantToCString(var);
			temp2.Format("IMEI %s Already in the No.:",m_csIMEI1);
			InsertListInfo(temp2,2);
			temp2.Format("%s%s",this->m_csCartonNameStatic,temp);
			this->m_ctrIMEI1.SetSel(0,-1);
			InsertListInfo(temp2,2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//判断彩盒是否有记录
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s'",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			this->m_ctrIMEI1.SetSel(0,-1);
			InsertListInfo("No color data, check box is missing...",2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//根据IMEI从ColorBox中获取其他信息
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s'",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1);  //@@ Call storage course @@
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BarCode");
			m_csSN.Empty();
			m_csSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ProductSN");
			m_csPSN.Empty();
			m_csPSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BTAddress");
			m_csBT.Empty();	
			m_csBT=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("WIFIMAC");
			m_csMAC.Empty();	
			m_csMAC=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("Weigh");
			m_csWeigh.Empty();	
			m_csWeigh=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("NetCode");
			m_csNet.Empty();	
			m_csNet=VariantToCString(var);
			
			//判断重量是否为空
			if(m_csWeigh=="")
			{
				this->m_ctrIMEI1.SetSel(0,-1);
				InsertListInfo("No weight information, check whether or not to leak...",2);
				return FALSE;
			}
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}


bool CPACKView::IMEITYPE_0_CheckImei1_Ex()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	//检测长度是否为15位
	if(m_csIMEI1.GetLength()!=15)
	{
		InsertListInfo("IMEI1 Error length...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI1.Left(14)>m_imei1_end || m_csIMEI1.Left(14)<m_imei1_start)
		{
			InsertListInfo("IMEI1 is out of range...",2);
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
			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
			((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI1 is not in IMEI form...",2);
				return FALSE;
			}

			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
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
		InsertListInfo("Illegal IMEI, check bit error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//检测该IMEI是否已经包装
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and CartonName IS NOT NULL and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("CartonName");
			temp=VariantToCString(var);
			temp2.Format("IMEI %s Already in the No.:",m_csIMEI1);
			InsertListInfo(temp2,2);
			temp2.Format("%s%s",this->m_csCartonNameStatic,temp);
			this->m_ctrIMEI1.SetSel(0,-1);
			InsertListInfo(temp2,2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	//判断上一工位——彩盒称重是否有记录
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and ColorBoxWeigh IS NOT NULL and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			this->m_ctrIMEI1.SetSel(0,-1);
			temp2.Format("IMEI %s No box weighing data, check whether the missing box weighing station...",m_csIMEI1);
			InsertListInfo(temp2,2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}
	
	//根据IMEI从ColorBox中获取其他信息
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI2");
			m_csIMEI2_Ex.Empty();
			m_csIMEI2_Ex=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI3");
			m_csIMEI3_Ex.Empty();
			m_csIMEI3_Ex=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI4");
			m_csIMEI4_Ex.Empty();
			m_csIMEI4_Ex=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BarCode");
			m_csSN.Empty();
			m_csSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ProductSN");
			m_csPSN.Empty();
			m_csPSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BTAddress");
			m_csBT.Empty();	
			m_csBT=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("WIFIMAC");
			m_csMAC.Empty();	
			m_csMAC=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ColorBoxWeigh");
			m_csWeigh.Empty();	
			m_csWeigh=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("NetCode");
			m_csNet.Empty();	
			m_csNet=VariantToCString(var);
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}


bool CPACKView::IMEITYPE_1_CheckImei2()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	//检测长度是否为15位
	if(m_csIMEI2.GetLength()!=15)
	{
		InsertListInfo("IMEI2 Error length...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI2.Left(14)>m_imei2_end || m_csIMEI2.Left(14)<m_imei2_start)
		{
			InsertListInfo("IMEI2 is out of range...",2);
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
			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
			((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI2 is not in IMEI form...",2);
				return FALSE;
			}

			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}

	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI2.Left(14),calculate_digit);

	if(calculate_digit!=m_csIMEI2.Right(1))
	{
		InsertListInfo("Illegal IMEI, check bit error...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	//检测该IMEI是否已经包装
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and IMEI2='%s' and CartonName IS NOT NULL and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1,this->m_csIMEI2);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("CartonName"); 
			temp=VariantToCString(var);
			InsertListInfo("The IMEI already exists in the No.:",2);
			temp2.Format("%s%s",this->m_csCartonNameStatic,temp);
			this->m_ctrIMEI2.SetSel(0,-1);
			InsertListInfo(temp2,2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	//判断上一工位——彩盒称重是否有记录
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and IMEI2='%s' and ColorBoxWeigh IS NOT NULL and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1,this->m_csIMEI2);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			this->m_ctrIMEI2.SetSel(0,-1);
			InsertListInfo("No box weighing data, check whether the missing box weighing station...",2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	//根据IMEI从ColorBox中获取其他信息
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and IMEI2='%s' and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1,this->m_csIMEI2);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BarCode");
			m_csSN.Empty();
			m_csSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ProductSN");
			m_csPSN.Empty();
			m_csPSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BTAddress");
			m_csBT.Empty();	
			m_csBT=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("WIFIMAC");
			m_csMAC.Empty();	
			m_csMAC=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ColorBoxWeigh");
			m_csWeigh.Empty();	
			m_csWeigh=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("NetCode");
			m_csNet.Empty();	
			m_csNet=VariantToCString(var);
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}


bool CPACKView::IMEITYPE_2_CheckImei3()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	//检测长度是否为15位
	if(m_csIMEI3.GetLength()!=15)
	{
		InsertListInfo("IMEI3 Error length...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI3.Left(14)>m_imei3_end || m_csIMEI3.Left(14)<m_imei3_start)
		{
			InsertListInfo("IMEI3 is out of range...",2);
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
			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
			((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI3 is not in IMEI form...",2);
				return FALSE;
			}

			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}

	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI3.Left(14),calculate_digit);

	if(calculate_digit!=m_csIMEI3.Right(1))
	{
		InsertListInfo("Illegal IMEI, check bit error...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	//检测该IMEI是否已经包装
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and IMEI2='%s' and IMEI3='%s' and CartonName IS NOT NULL and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1,this->m_csIMEI2,this->m_csIMEI3);  //@@ Call storage course @@
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("carton_name"); 
			temp=VariantToCString(var);
			InsertListInfo("The IMEI already exists in the No.:",2);
			temp2.Format("%s%s",this->m_csCartonNameStatic,temp);
			this->m_ctrIMEI3.SetSel(0,-1);
			InsertListInfo(temp2,2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}
	
	//判断上一工位——彩盒称重是否有记录
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and IMEI2='%s' and IMEI3='%s' and ColorBoxWeigh IS NOT NULL and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1,this->m_csIMEI2,this->m_csIMEI3);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			this->m_ctrIMEI3.SetSel(0,-1);
			InsertListInfo("No box weighing data, check whether the missing box weighing station...",2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	//根据IMEI从ColorBox中获取其他信息
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and IMEI2='%s' and IMEI3='%s' and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1,this->m_csIMEI2,this->m_csIMEI3);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BarCode");
			m_csSN.Empty();
			m_csSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ProductSN");
			m_csPSN.Empty();
			m_csPSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BTAddress");
			m_csBT.Empty();	
			m_csBT=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("WIFIMAC");
			m_csMAC.Empty();	
			m_csMAC=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ColorBoxWeigh");
			m_csWeigh.Empty();	
			m_csWeigh=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("NetCode");
			m_csNet.Empty();	
			m_csNet=VariantToCString(var);
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}


bool CPACKView::IMEITYPE_3_CheckImei4()
{
	CString temp,temp2;
	_variant_t var;
	CString sql;

	//检测长度是否为15位
	if(m_csIMEI4.GetLength()!=15)
	{
		InsertListInfo("IMEI4 Error length...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}

	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI4.Left(14)>m_imei4_end || m_csIMEI4.Left(14)<m_imei4_start)
		{
			InsertListInfo("IMEI4 is out of range...",2);
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
			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
			((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI4 is not in IMEI form...",2);
				return FALSE;
			}

			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}

	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI4.Left(14),calculate_digit);

	if(calculate_digit!=m_csIMEI4.Right(1))
	{
		InsertListInfo("Illegal IMEI, check bit error...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}

	//检测该IMEI是否已经包装
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and IMEI2='%s' and IMEI3='%s'and IMEI4='%s' and CartonName IS NOT NULL and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1,this->m_csIMEI2,this->m_csIMEI3,this->m_csIMEI4);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("carton_name"); 
			temp=VariantToCString(var);
			InsertListInfo("The IMEI already exists in the No.:",2);
			this->m_ctrIMEI4.SetSel(0,-1);
			temp2.Format("%s%s",this->m_csCartonNameStatic,temp);
			InsertListInfo(temp2,2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}
	
	//判断上一工位——彩盒称重是否有记录
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and IMEI2='%s' and IMEI3='%s' and IMEI4='%s' and ColorBoxWeigh IS NOT NULL and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1,this->m_csIMEI2,this->m_csIMEI3,this->m_csIMEI4);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			this->m_ctrIMEI4.SetSel(0,-1);
			InsertListInfo("No box weighing data, check whether the missing box weighing station...",2);
			return FALSE;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}

	//根据IMEI从ColorBox中获取其他信息
	try
	{
		sql.Format("select * from %s_%s_ColorBox where IMEI1='%s' and IMEI2='%s' and IMEI3='%s' and IMEI4='%s' and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csIMEI1,this->m_csIMEI2,this->m_csIMEI3,this->m_csIMEI4);
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BarCode");
			m_csSN.Empty();
			m_csSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ProductSN");
			m_csPSN.Empty();
			m_csPSN=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("BTAddress");
			m_csBT.Empty();	
			m_csBT=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("WIFIMAC");
			m_csMAC.Empty();	
			m_csMAC=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ColorBoxWeigh");
			m_csWeigh.Empty();	
			m_csWeigh=VariantToCString(var);

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("NetCode");
			m_csNet.Empty();	
			m_csNet=VariantToCString(var);
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		this->m_ctrIMEI4.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}


bool CPACKView::InsertDataToSQL()
{
	_variant_t var;
	CString sql;

	//抢箱号start->如果是列表中的第一个数据那么重新获取一下数据库中的NEXT箱号是否和之前获取的箱号相同
	if(m_cur_row==1)
	{
		this->ReGet_Current_Carton_Name();
		if(this->m_csReGet_Current_Carton != this->m_csCurrent_Carton)
		{
			CString temp;
			CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
			this->m_csCurrent_Carton = this->m_csReGet_Current_Carton;
			temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
			pFrame->m_wndStatusBar.SetPaneText(1,temp);
			InsertListInfo(temp,0);//调整显示位置
		}
		
		//将卡通箱号插入到CartonBox
		try
		{
			sql.Format("INSERT INTO %s_%s_CartonBox (CartonName,LabelPrintCount,Status) VALUES ('%s',%d, %d)",this->m_csCurrentProduct,this->m_csCurrentOrder,m_csCurrent_Carton,0,1);
			((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		}
		catch(_com_error e)
		{
			InsertListInfo("Save the record to the carton box database failed...",2);
			return FALSE;
		} 

		InsertListInfo("Save the record to the carton box database successfully...",0);
	}
	//抢箱号end

	//将卡通箱号插入到ColorBox
	try
	{
		sql.Format("UPDATE %s_%s_ColorBox SET CartonName='%s' where IMEI1='%s' and Enable=1",this->m_csCurrentProduct,this->m_csCurrentOrder,m_csCurrent_Carton,m_csIMEI1);
		((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
	}
	catch(_com_error e)
	{
		InsertListInfo("To save the record box database failed...",2);
		return FALSE;
	}

	return TRUE;
}


bool CPACKView::UpdateStatusTo2()
{
	CString temp;
	_variant_t var;
	CString sql;
	
	try
	{
		sql.Format("UPDATE %s_%s_CartonBox SET Status=2 where CartonName='%s'",this->m_csCurrentProduct,this->m_csCurrentOrder,this->m_csCurrent_Carton);
		((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
	}
	catch(_com_error e)
	{
		InsertListInfo("Unknown error...",2);
		return FALSE;
	}

	return TRUE;
}


BOOL CPACKView::IMEITYPE_1_CheckImei1()
{
	//检测长度是否为15位
	if(m_csIMEI1.GetLength()!=15)
	{
		InsertListInfo("IMEI1 Error length...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI1.Left(14)>this->m_imei1_end || m_csIMEI1.Left(14)<this->m_imei1_start)
		{
			InsertListInfo("IMEI1 is out of range...",2);
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
			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
			((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI1 is not in IMEI form...",2);
				return FALSE;
			}

			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
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
		InsertListInfo("Illegal IMEI1, check bit error...",2);
		this->m_ctrIMEI1.SetSel(0,-1);
		return FALSE;
	}
	
	return TRUE;
}


BOOL CPACKView::IMEITYPE_2_CheckImei2()
{
	//检测长度是否为15位
	if(m_csIMEI2.GetLength()!=15)
	{
		InsertListInfo("IMEI2 Error length...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI2.Left(14)>this->m_imei2_end || m_csIMEI2.Left(14)<this->m_imei2_start)
		{
			InsertListInfo("IMEI2 is out of range...",2);
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
			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
			((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI2 is not in IMEI form...",2);
				return FALSE;
			}

			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI2.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI2.Right(1))	
	{
		InsertListInfo("Illegal IMEI2, check bit error...",2);
		this->m_ctrIMEI2.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}


BOOL CPACKView::IMEITYPE_3_CheckImei3()
{
	//检测长度是否为15位
	if(m_csIMEI3.GetLength()!=15)
	{
		InsertListInfo("IMEI3 Error length...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}
	
	//检测号码是否再合法范围内
	if (m_iIMEIRangeType==0)
	{
		if(m_csIMEI3.Left(14)>this->m_imei3_end || m_csIMEI3.Left(14)<this->m_imei3_start)
		{
			InsertListInfo("IMEI3 is out of range...",2);
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
			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
			((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);

			if(((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
			{
				InsertListInfo("IMEI3 is not in IMEI form...",2);
				return FALSE;
			}

			if(((CPACKApp *)AfxGetApp())->m_pRst->State)
				((CPACKApp *)AfxGetApp())->m_pRst->Close();
		}
		catch(_com_error e)
		{
			return FALSE;
		}
	}
	
	//检测IMEI校验位是否正确
	char calculate_digit;
	calculate_15_digit_imei((char*)(LPCTSTR)m_csIMEI3.Left(14),calculate_digit);
	
	if(calculate_digit!=m_csIMEI3.Right(1))	
	{
		InsertListInfo("Illegal IMEI3, check bit error...",2);
		this->m_ctrIMEI3.SetSel(0,-1);
		return FALSE;
	}

	return TRUE;
}


void CPACKView::OnBtnPrint()
{
	UpdateData();

	CCartonBoxPrint CPDlg;

	CPDlg.m_csCurrentProduct   =  this->m_csCurrentProduct;
	CPDlg.m_csCurrentOrder     =  this->m_csCurrentOrder;
	CPDlg.m_csCurrentColor     =  this->m_csCurrentColor;
	CPDlg.m_csStaticCartonName =  this->m_csCartonNameStatic;
	CPDlg.m_replace_R1         =  this->m_csCurrent_Carton;
	CPDlg.csCurrentLine        =  this->m_csCurrentLine;
	CPDlg.csLocalPathtemp      =  this->m_csLocalPath;
	CPDlg.m_iMaxCount          =  this->m_iMaxCount;

	CPDlg.csPSNStatic          =  this->m_csPSNStatic;
	CPDlg.bPSNStatic           =  this->m_bPSNStatic;

	CPDlg.DoModal();
}


int CPACKView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CSplashWnd::ShowSplashScreen(this);
	
	return 0;
}


void CPACKView::OnScanAll() 
{
	CPassword psw;
	psw.ilevel = 1; //需普超级限验证
	if(IDOK!=psw.DoModal())
		return;

	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_ONE, MF_BYCOMMAND | MF_UNCHECKED );
	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_ALL, MF_BYCOMMAND | MF_CHECKED ); 
	this->m_Config.m_iScanMode=2;
	this->m_Config.SaveConfig();
}


void CPACKView::OnScanOne() 
{
	CPassword psw;
	psw.ilevel = 1; //需普超级限验证
	if(IDOK!=psw.DoModal())
		return;

	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_ALL, MF_BYCOMMAND | MF_UNCHECKED );
	AfxGetMainWnd()->GetMenu()->GetSubMenu(0)->CheckMenuItem(IDM_SCAN_ONE, MF_BYCOMMAND | MF_CHECKED ); 
	this->m_Config.m_iScanMode=1;
	this->m_Config.SaveConfig();	
}


bool CPACKView::GetProductOrderInfo()
{
	CString temp;
	_variant_t var;
	CString sql;
	
	try
	{
		sql.Format("select * from ProductList where Enable=1 and OrderName='%s'",m_csCurrentOrder);
		
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("ProductName");
			temp=VariantToCString(var);
			this->m_csCurrentProduct= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEIType");
			this->m_iIMEIType=var.intVal;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEIRangeType");
			this->m_iIMEIRangeType=var.intVal;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI1_STR");
			temp=VariantToCString(var);
			this->m_imei1_start= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI1_END");
			temp=VariantToCString(var);
			this->m_imei1_end= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI2_STR");
			temp=VariantToCString(var);
			this->m_imei2_start= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI2_END");
			temp=VariantToCString(var);
			this->m_imei2_end= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI3_STR");
			temp=VariantToCString(var);
			this->m_imei3_start= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI3_END");
			temp=VariantToCString(var);
			this->m_imei3_end= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI4_STR");
			temp=VariantToCString(var);
			this->m_imei4_start= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("IMEI4_END");
			temp=VariantToCString(var);
			this->m_imei4_end= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("CartonNameStatic");
			temp=VariantToCString(var);
			this->m_csCartonNameStatic= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("CartonNameBegin");//初始箱号
			temp=VariantToCString(var);
			this->m_csCartonNameBegin= temp;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("CartonNameSNLength");
			this->m_iCartonNameLength=var.intVal;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("CartonMaxCount");
			this->m_iMaxCount=var.intVal;

			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("Color");
			temp=VariantToCString(var);
			this->m_csCurrentColor= temp;
		}
		else
		{
			return FALSE;
		}
		
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}

	try
	{
		sql.Format("select * from SettingList where Enable=1 and OrderName='%s'",m_csCurrentOrder);
		
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("PSNCheckBit");
			temp=VariantToCString(var);
			m_csPSNStatic = temp;
		}
		else
		{
			return FALSE;
		}
		
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}
	
	return TRUE;
}


bool CPACKView::Load_Current_Product_Count()
{
	CString temp,states;
	_variant_t var;
	CString sql;

	sql.Format("select count(*) as COUNT from %s_%s_CartonBox WHERE Status = 2",(char*)(LPCTSTR)this->m_csCurrentProduct,this->m_csCurrentOrder);

	try
	{
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
		((CPACKApp *)AfxGetApp())->m_pRst=((CPACKApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		
		if(!((CPACKApp *)AfxGetApp())->m_pRst->adoEOF)
		{			
			var = ((CPACKApp *)AfxGetApp())->m_pRst->GetCollect("COUNT");
			temp=VariantToCString(var);
			this->m_csCurrentCount= temp;
		}
		if(((CPACKApp *)AfxGetApp())->m_pRst->State)
			((CPACKApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}

	return TRUE;
}


void CPACKView::UpdateCount(CString strCount)
{
	CString temp;   
	temp="Quantity:"+strCount;
	this->m_cCurrentCount.SetWindowText(temp);
	UpdateData(FALSE);
}


void CPACKView::PopMessageBox_Ex()
{
	CPopMessageBox dlg;
	dlg.DoModal();
}


void CPACKView::PopMainCarton_Ex(CString strMainCarton)
{
	CPopMainCarton dlg;
	dlg.m_csMainCarton=strMainCarton;
	dlg.DoModal();
}

void CPACKView::OnBtnSetting() 
{
	CPassword dlg;
	dlg.ilevel = 1; //需超级权限验证
	if(dlg.DoModal()!=IDOK)
		return;

	CPrintSetting Pdlg;
	Pdlg.csLocalPath = this->m_csLocalPath;
	Pdlg.DoModal();
}

void CPACKView::OnCheckPsnStatic() 
{
	UpdateData();
	bool flag;

	flag = m_bPSNStatic;

	CPassword dlg;
	dlg.ilevel = 1; //需超级权限验证
	if(dlg.DoModal()!=IDOK)
	{
		m_ctrPSNStatic.SetCheck(!flag);
		return;
	}
}

void CPACKView::OnCheckAddZ() 
{
	UpdateData();
	bool flag;
	CString temp;

	flag = m_bAddZ;

	CPassword dlg;
	dlg.ilevel = 1; //需超级权限验证
	if(dlg.DoModal()!=IDOK)
	{
		m_ctrAddZ.SetCheck(!flag);
		return;
	}

	if (flag == TRUE)
	{
		m_ctrAdd.EnableWindow(FALSE);
	}
	else
	{
		m_ctrAdd.EnableWindow(TRUE);
	}
	
	CMainFrame *pFrame=(CMainFrame*)GetParentFrame();
	//获取当前箱号
	if(!Get_Current_Carton_Name())
	{
		InsertListInfo("Get the current project order number failed...",2);
		return;
	}
	else
	{
		InsertListInfo("Get the current project order number success...",0);
		temp.Format("The current No.: %s%s",this->m_csCartonNameStatic,this->m_csCurrent_Carton);
		pFrame->m_wndStatusBar.SetPaneText(1,temp);
		InsertListInfo(temp,0);//调整显示位置
	}
}
