// WeighSetting.cpp : implementation file
//

#include "stdafx.h"
#include "weightool.h"
#include "WeighSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWeighSetting dialog


CWeighSetting::CWeighSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CWeighSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWeighSetting)
	m_dHigh = 0.0;
	m_dLow = 0.0;
	//}}AFX_DATA_INIT
}


void CWeighSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWeighSetting)
	DDX_Text(pDX, IDC_EDIT_HIGH, m_dHigh);
	DDX_Text(pDX, IDC_EDIT_LOW, m_dLow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWeighSetting, CDialog)
	//{{AFX_MSG_MAP(CWeighSetting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeighSetting message handlers

BOOL CWeighSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (!GetWeighRange())
	{
		MessageBox("Get the cartoon box weight range failed!","ERROR",MB_OK);
		return FALSE;
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWeighSetting::OnOK() 
{
	// TODO: Add extra validation here
	_variant_t var;
	CString sql;

	UpdateData();

	try
	{
		sql.Format("UPDATE SettingList SET CartonBoxLowWeigh=%f, CartonBoxHighWeigh=%f WHERE Enable=1 and ProductName='%s' and OrderName='%s'", \
			m_dLow,m_dHigh,m_csProduct,m_csOrder);
		if(((CWeighToolApp *)AfxGetApp())->m_pRst->State)
			((CWeighToolApp *)AfxGetApp())->m_pRst->Close();
		((CWeighToolApp *)AfxGetApp())->m_pRst=((CWeighToolApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		if(((CWeighToolApp *)AfxGetApp())->m_pRst->State)
			((CWeighToolApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		MessageBox("Reset the weight of the cartoon box!","ERROR",MB_OK);
		return;
	}
	
	CDialog::OnOK();
}

bool CWeighSetting::GetWeighRange()
{
	_variant_t var;
	CString sql;
	
	try
	{
		sql.Format("select * from SettingList where Enable=1 and ProductName='%s' and OrderName='%s'",this->m_csProduct,this->m_csOrder);
		if(((CWeighToolApp *)AfxGetApp())->m_pRst->State)
			((CWeighToolApp *)AfxGetApp())->m_pRst->Close();
		((CWeighToolApp *)AfxGetApp())->m_pRst=((CWeighToolApp *)AfxGetApp())->m_pConnection->Execute((_bstr_t)sql,NULL,adCmdText);
		
		if(!((CWeighToolApp *)AfxGetApp())->m_pRst->adoEOF)
		{
			var = ((CWeighToolApp *)AfxGetApp())->m_pRst->GetCollect("CartonBoxLowWeigh");
			m_dLow=atof((_bstr_t)var);

			var = ((CWeighToolApp *)AfxGetApp())->m_pRst->GetCollect("CartonBoxHighWeigh");
			m_dHigh=atof((_bstr_t)var);
		}
		else
		{
			return FALSE;
		}
		if(((CWeighToolApp *)AfxGetApp())->m_pRst->State)
			((CWeighToolApp *)AfxGetApp())->m_pRst->Close();
	}
	catch(_com_error e)
	{
		return FALSE;
	}

	UpdateData(FALSE);
	return TRUE;
}
