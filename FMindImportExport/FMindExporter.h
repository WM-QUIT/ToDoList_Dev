// FMindExporter.h: interface for the CFMindExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FMINDEXPORTER_H__49174123_010C_4E73_B1BF_5440AE766D46__INCLUDED_)
#define AFX_FMINDEXPORTER_H__49174123_010C_4E73_B1BF_5440AE766D46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\shared\Icon.h"

#include "..\Interfaces\IImportExport.h"
#include "..\Interfaces\ITasklist.h"

class CXmlItem;
class CXmlFile;
class ITransText;

class CFMindExporter : public IExportTasklist  
{
public:
	CFMindExporter();
	virtual ~CFMindExporter();
	
	// interface implementation
	void Release() { delete this; }
	void SetLocalizer(ITransText* pTT);

	// caller must copy only
	LPCTSTR GetMenuText() const { return _T("FreeMind"); }
	LPCTSTR GetFileFilter() const { return _T("FreeMind Files (*.mm)|*.mm||"); }
	LPCTSTR GetFileExtension() const { return _T("mm"); }
	HICON GetIcon() const { return m_icon; }
	
	bool Export(const ITaskList* pSrcTaskFile, LPCTSTR szDestFilePath, bool bSilent, IPreferences* pPrefs, LPCTSTR szKey);
	bool Export(const IMultiTaskList* pSrcTaskFile, LPCTSTR szDestFilePath, bool bSilent, IPreferences* pPrefs, LPCTSTR szKey);

protected:
	CIcon m_icon;
	
protected:
	void ExportTask(const ITASKLISTBASE* pSrcTaskFile, HTASKITEM hTask, 
					CXmlItem* pXIDestParent, int LEVEL, BOOL bAndSiblings);
	CString FormatDate(time_t tDate);

	static CString Translate(LPCTSTR szText);
	static CString Export(const CXmlFile& file);

};

#endif // !defined(AFX_FMINDEXPORTER_H__49174123_010C_4E73_B1BF_5440AE766D46__INCLUDED_)
