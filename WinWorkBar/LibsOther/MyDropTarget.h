//////////////////////////////////////////////////////////////////////
//
// MyDropTarget.h: interface for the CMyDropTarget class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../default.h"
#include <afxole.h>
#include "afxcmn.h"
#include "afxwin.h"
#include "shlobj.h"
#include "shlguid.h"
#include <vector>

//////////////////////////////////////////////////////////////////////
// CMyDropTarget implements a drop target for the MultiFiler dialog.
// I was also messing around with the IDropTargetHelper stuff in 
// Win2K, which lets the shell draw the nifty shaded drag image when
// you drag into the dialog.  If you're using 9x or NT 4, that stuff
// is disabled.
 
struct IDropTargetHelper;   // forward reference, in case the latest PSDK isn't installed.

class CMyDropTarget : public COleDropTarget  
{
public:
	AddFilesToMainList	fnAddFiles;

	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);

	BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

	void OnDragLeave( CWnd* pWnd );

  CMyDropTarget();
	virtual ~CMyDropTarget();
protected:
    IDropTargetHelper* m_piDropHelper;
    bool               m_bUseDnDHelper;
    BOOL ReadHdropData ( COleDataObject* pDataObject );
};
