// MyDropTarget.cpp: implementation of the CMyDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "MyDropTarget.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern UINT g_uCustomClipbrdFormat;
extern bool g_bNT;

//////////////////////////////////////////////////////////////////////
// Construction/destruction

CMyDropTarget::CMyDropTarget()
	: m_piDropHelper(NULL), m_bUseDnDHelper(false)
{
	// Create an instance of the shell DnD helper object.
	if ( SUCCEEDED( CoCreateInstance ( CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER,IID_IDropTargetHelper, (void**) &m_piDropHelper)))
	{
		m_bUseDnDHelper = true;
	}
}

CMyDropTarget::~CMyDropTarget()
{
	if (NULL != m_piDropHelper)
    m_piDropHelper->Release();
}

// IDropTarget methods
DROPEFFECT CMyDropTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

  // Check for our own custom clipboard format in the data object.  If it's
  // present, then the DnD was initiated from our own window, and we won't
  // accept the drop.
  // If it's not present, then we check for CF_HDROP data in the data object.
	if (NULL == pDataObject->GetGlobalData(g_uCustomClipbrdFormat))
	{
		// Look for CF_HDROP data in the data object, and accept the drop if it's there.
		if (NULL != pDataObject->GetGlobalData(CF_HDROP))
			dwEffect = DROPEFFECT_COPY;
	}

	// Call the DnD helper.
	if (m_bUseDnDHelper)
	{
		// The DnD helper needs an IDataObject interface, so get one from
		// the COleDataObject.  Note that the FALSE param means that
		// GetIDataObject will not AddRef() the returned interface, so we do not Release() it.
		IDataObject* piDataObj = pDataObject->GetIDataObject(FALSE); 
		m_piDropHelper->DragEnter ( pWnd->GetSafeHwnd(), piDataObj, &point, dwEffect);
	}
	return dwEffect;
}

DROPEFFECT CMyDropTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;

  // Check for our own custom clipboard format in the data object.  If it's
  // present, then the DnD was initiated from our own window, and we won't
  // accept the drop.
  // If it's not present, then we check for CF_HDROP data in the data object.
	if ( NULL == pDataObject->GetGlobalData(g_uCustomClipbrdFormat))
	{
		// Look for CF_HDROP data in the data object, and accept the drop if it's there.
		if (NULL != pDataObject->GetGlobalData(CF_HDROP))
      dwEffect = DROPEFFECT_COPY;
	}
	
	// Call the DnD helper.
	if ( m_bUseDnDHelper )
	{
		m_piDropHelper->DragOver(&point, dwEffect);
	}
	return dwEffect;
}

BOOL CMyDropTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	BOOL bRet;

  // Read the CF_HDROP data and put the files in the main window's list.
	bRet = ReadHdropData(pDataObject);

	// Call the DnD helper.
	if (m_bUseDnDHelper)
	{
		// The DnD helper needs an IDataObject interface, so get one from
		// the COleDataObject.  Note that the FALSE param means that
		// GetIDataObject will not AddRef() the returned interface, so we do not Release() it.
		IDataObject* piDataObj = pDataObject->GetIDataObject(FALSE);
		m_piDropHelper->Drop(piDataObj, &point, dropEffect);
	}
	return bRet;
}

void CMyDropTarget::OnDragLeave(CWnd* pWnd)
{
	if (m_bUseDnDHelper)
	{
		m_piDropHelper->DragLeave();
	}
}

// ReadHdropData() reads CF_HDROP data from the passed-in data object, and 
// puts all dropped files/folders into the main window's list control.
BOOL CMyDropTarget::ReadHdropData(COleDataObject* pDataObject)
{
	HGLOBAL     hg;
	HDROP       hdrop;
	UINT        uNumFiles;
	TCHAR       szNextFile[MAX_PATH];
	std::vector<std::string>	FileList;

	// Get the HDROP data from the data object.
	hg = pDataObject->GetGlobalData(CF_HDROP);
	if (!hg)
		return FALSE;

	hdrop = (HDROP)GlobalLock(hg);
	if (!hdrop)
	{
		GlobalUnlock (hg);
		return FALSE;
	}

	// Get the # of files being dropped.
	uNumFiles = DragQueryFile(hdrop, -1, NULL, 0);

	for (UINT uFile = 0; uFile < uNumFiles; uFile++)
	{
		// Get the next filename from the HDROP info.
		if (DragQueryFile(hdrop, uFile, szNextFile, MAX_PATH ) > 0)
		{
			FileList.push_back(szNextFile);
		}
	} // end for

	GlobalUnlock(hg);

	if (fnAddFiles)
		if (FileList.size() > 0)
			fnAddFiles(FileList);

	return TRUE;
}
