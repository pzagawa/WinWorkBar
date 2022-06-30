// CDlgImportExport.cpp : implementation file
//

#include "DlgImportExport.h"
#include "WinWorkBarDlg.h"

extern CWinWorkBarDlg*	lpMain;

// CDlgImportExport dialog

IMPLEMENT_DYNAMIC(CDlgImportExport, CDialog)
CDlgImportExport::CDlgImportExport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportExport::IDD, pParent)
{
	bImport = true;
}

CDlgImportExport::~CDlgImportExport()
{
}

void CDlgImportExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAB_IE_TITLE, labTitle);
	DDX_Control(pDX, IDC_PROGRESS_IE, cProgress);
	DDX_Control(pDX, IDOK, btnOk);
	DDX_Control(pDX, IDCANCEL, btnCancel);
	DDX_Control(pDX, IDC_LIST_IE_DATA, listData);
}


BEGIN_MESSAGE_MAP(CDlgImportExport, CDialog)
	ON_CBN_SELCHANGE(IDC_LIST_IE_DATA, OnCbnSelchangeListIeData)
END_MESSAGE_MAP()


// CDlgImportExport message handlers

BOOL CDlgImportExport::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::string DlgTitle = ProgramName + " - " + lpMain->ML.Text((bImport)?178:179);
	SetWindowText(DlgTitle.c_str());

	labTitle.SetWindowText(lpMain->ML.Text(181));
	cProgress.SetRange(0, 100);
	cProgress.SetPos(0);

	btnOk.SetWindowText(lpMain->ML.Text((bImport)?182:27));
	btnCancel.SetWindowText(lpMain->ML.Text(65));

	listData.ResetContent();
	listData.InsertString(0, lpMain->ML.Text(16));
	listData.InsertString(1, lpMain->ML.Text(17));
	listData.InsertString(2, lpMain->ML.Text(25));
	listData.EnableWindow(!bImport);

	btnOk.EnableWindow(bImport);

	return TRUE;
}

void CDlgImportExport::ExportDataXml(std::string sFileName)
{
	int nIndex = listData.GetCurSel();
	if (nIndex != CB_ERR)
	{
		switch (nIndex)
		{
			case 0:	lpMain->tbTasks.SaveTableToFile(sFileName);	break;
			case 1:	lpMain->tbNotes.SaveTableToFile(sFileName);	break;
			case 2:	lpMain->tbCategories.SaveTableToFile(sFileName);	break;
		};
	}
}

void CDlgImportExport::ImportDataXml(std::string sFileName)
{
	TiXmlDocument	Doc;
	if (Doc.LoadFile(sFileName.c_str()))
	{
		PSXML::PSTinyXML* TB = NULL;
		int iListIdx = -1;
		TiXmlElement* lRoot = NULL;
		TiXmlElement* lInfo = NULL;
		TiXmlElement* lAppName = NULL;
		std::string		sAppName;

		if ((!lRoot) && (lRoot = Doc.FirstChildElement("Tasks")))
		{
			iListIdx = 0;
			TB = &lpMain->tbTasks;
		}

		if ((!lRoot) && (lRoot = Doc.FirstChildElement("Notes")))
		{
			iListIdx = 1;
			TB = &lpMain->tbNotes;
		}

		if ((!lRoot) && (lRoot = Doc.FirstChildElement("Categories")))
		{
			iListIdx = 2;
			TB = &lpMain->tbCategories;
		}

		if (lRoot)
		{
			if (lInfo = lRoot->FirstChildElement("Information"))
			{
				if (lAppName = lInfo->FirstChildElement("Name"))
				{					
					if (TiXmlNode* lValue = lAppName->FirstChild())
						sAppName = lValue->Value();
				}
			}
		}

		if (sAppName == ProgramName)
		{
			listData.SetCurSel(iListIdx);
			Utils::ImportData(*TB, sFileName, &cProgress);

			if (iListIdx == 0)
			{
				lpMain->tbTasks.SaveTable();
				lpMain->dlgTasks.LoadListItems();
			}
			if (iListIdx == 1)
			{
				lpMain->tbNotes.SaveTable();
				lpMain->dlgNotes.LoadListItems();
			}
			if (iListIdx == 2)
			{
				lpMain->tbCategories.SaveTable();
				lpMain->dlgTasks.LoadListItems();
				lpMain->dlgNotes.LoadListItems();
			}

			lpMain->dlgToday.LoadListItems();
		} else {
			MessageBox(lpMain->ML.Text(183), NULL, MB_OK | MB_ICONERROR);
		}
	}
}

void CDlgImportExport::OnOK()
{
	const char szFilterTemplate[] = "%s (*.xml)|*.xml||";
	char szFileFilter[128];
	wsprintf(szFileFilter, szFilterTemplate, lpMain->ML.Text(180));

	std::string InitFolder = Misc::GetFolder("desktop");

	TCHAR IniFileName[MAX_PATH];
	CString S;
	listData.GetWindowText(S);
	std::string sFileName = "WinWorkBar ";
	sFileName += S;
	lstrcpy(IniFileName, sFileName.c_str());

	cProgress.SetPos(0);

	if (bImport)
	{
		CFileDialog	FileDialog(true, "xml", NULL, OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_HIDEREADONLY, szFileFilter);
		FileDialog.GetOFN().lpstrInitialDir = InitFolder.c_str();
		if (FileDialog.DoModal() == IDOK)
		{
			std::string sFileName = FileDialog.GetPathName();
			ImportDataXml(sFileName);
		}
	} else {
		CFileDialog	FileDialog(false, "xml", NULL, OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_HIDEREADONLY, szFileFilter);
		FileDialog.GetOFN().lpstrInitialDir = InitFolder.c_str();
		FileDialog.GetOFN().lpstrFile = IniFileName;
		if (FileDialog.DoModal() == IDOK)
		{
			std::string sFileName = FileDialog.GetPathName();
			cProgress.SetPos(100);
			ExportDataXml(sFileName);
		}
	}
	cProgress.SetPos(0);
	listData.SetCurSel(-1);
	OnCbnSelchangeListIeData();
	//CDialog::OnOK();
}

void CDlgImportExport::OnCancel()
{

	CDialog::OnCancel();
}

void CDlgImportExport::OnCbnSelchangeListIeData()
{
	CString S;
	listData.GetWindowText(S);
	if (!bImport)
		btnOk.EnableWindow(!S.IsEmpty());
}
