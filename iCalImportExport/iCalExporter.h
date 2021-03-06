// iCalExporter.h: interface for the CiCalExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICALEXPORTER_H__5CDCA505_6C5C_4342_8F80_67368C7EE6A5__INCLUDED_)
#define AFX_ICALEXPORTER_H__5CDCA505_6C5C_4342_8F80_67368C7EE6A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\shared\Icon.h"

#include "..\Interfaces\IImportExport.h"
#include "..\Interfaces\ITasklist.h"

class CStdioFileEx;

class CiCalExporter : public IExportTasklist  
{
public:
	CiCalExporter();
	virtual ~CiCalExporter();

	// interface implementation
	void Release() { delete this; }
	void SetLocalizer(ITransText* pTT);

	// caller must copy only
	LPCTSTR GetMenuText() const { return _T("iCalendar"); }
	LPCTSTR GetFileFilter() const { return _T("iCalendar Files (*.ics)|*.ics||"); }
	LPCTSTR GetFileExtension() const { return _T("ics"); }
	HICON GetIcon() const { return m_icon; }

	bool Export(const ITaskList* pSrcTaskFile, LPCTSTR szDestFilePath, bool bSilent, IPreferences* pPrefs, LPCTSTR szKey);
	bool Export(const IMultiTaskList* pSrcTaskFile, LPCTSTR szDestFilePath, bool bSilent, IPreferences* pPrefs, LPCTSTR szKey);

protected:
	CIcon m_icon;
	BOOL EXPORTASVTASK;

protected:
	void ExportTask(const ITASKLISTBASE* pTasks, HTASKITEM hTask, const CString& sParentUID, 
							CStdioFile& fileOut, BOOL bAndSiblings);
	bool InitConsts(BOOL bSilent, IPreferences* pPrefs, LPCTSTR szKey);

	static void __cdecl WriteString(CStdioFile& fileOut, LPCTSTR lpszFormat, ...);
	static CString FormatDateTime(LPCTSTR szType, const COleDateTime& date, BOOL bStartOfDay);
	static CString FormatRecurrence(int nRegularity, DWORD dwSpecific1, DWORD dwSpecific2);
	static CString FormatDayOfMonth(DWORD dwDOM);
	static void WriteHeader(CStdioFileEx& fileOut);
	static CString FormatUID(LPCTSTR szFileName, DWORD dwTaskID);
	static BOOL GetTaskDates(const ITASKLISTBASE* pTasks, HTASKITEM hTask, time64_t& tStart, time64_t& tEnd, COleDateTime& dtDue);
};

#endif // !defined(AFX_ICALEXPORTER_H__5CDCA505_6C5C_4342_8F80_67368C7EE6A5__INCLUDED_)
