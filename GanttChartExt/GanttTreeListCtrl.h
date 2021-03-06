// GanttTreeList.h: interface for the CGanttTreeList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GANTTTREELISTCTRL_H__016B94F3_1D28_4532_97EF_95F1D9D5CE55__INCLUDED_)
#define AFX_GANTTTREELISTCTRL_H__016B94F3_1D28_4532_97EF_95F1D9D5CE55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Ganttstruct.h"
#include "GanttTreeCtrl.h"

#include "..\shared\TreeListSyncer.h"
#include "..\shared\enheaderctrl.h"
#include "..\shared\graphicsmisc.h"
#include "..\shared\treectrlhelper.h"
#include "..\shared\fontcache.h"
#include "..\shared\mapex.h"

#include "..\Interfaces\itasklist.h"
#include "..\Interfaces\iuiextension.h"

/////////////////////////////////////////////////////////////////////////////

#define TVN_KEYUP (TVN_FIRST-16)

/////////////////////////////////////////////////////////////////////////////

class CGanttDependencyEditor
{
public:
	virtual BOOL SetFromTask(DWORD dwFromTaskID) = 0;
	virtual BOOL SetFromDependency(DWORD dwFromTaskID, DWORD dwCurToTaskID) = 0;
	virtual BOOL SetToTask(DWORD dwToTaskID) = 0;
	
	virtual DWORD GetFromTask() const = 0;
	virtual DWORD GetFromDependency(DWORD& dwCurToTaskID) const = 0;
	virtual DWORD GetToTask() const = 0;
	
	virtual BOOL IsPicking() const = 0;
	virtual BOOL IsPickingFromTask() const = 0;
	virtual BOOL IsPickingFromDependency() const = 0;
	virtual BOOL IsPickingToTask() const = 0;
	virtual BOOL IsPickingCancelled() const = 0;
	virtual BOOL IsPickingCompleted() const = 0;

	virtual BOOL IsAdding() const = 0;
	virtual BOOL IsEditing() const = 0;
	virtual BOOL IsDeleting() const = 0;

	virtual void Cancel() = 0;
};

/////////////////////////////////////////////////////////////////////////////

class CThemed;
class CGanttCreateDependsDlg;

/////////////////////////////////////////////////////////////////////////////

class CGanttTreeListCtrl : protected CTreeListSyncer  
{
	friend class CGanttLockUpdates;

public:
	CGanttTreeListCtrl(CGanttTreeCtrl& tree, CListCtrl& list);
	virtual ~CGanttTreeListCtrl();

	BOOL Initialize(UINT nIDTreeHeader);
	void Release();

	BOOL SaveToImage(CBitmap& bmImage);
	BOOL SetFont(HFONT hFont, BOOL bRedraw = TRUE);

	void UpdateTasks(const ITaskList* pTasks, IUI_UPDATETYPE nUpdate, const CSet<IUI_ATTRIBUTE>& attrib);
	bool PrepareNewTask(ITaskList* pTask) const;

	DWORD GetSelectedTaskID() const;
	BOOL SelectTask(DWORD dwTaskID);
	HTREEITEM GetSelectedItem() const;
	BOOL GetSelectedTaskDates(COleDateTime& dtStart, COleDateTime& dtDue) const;
	DWORD GetNextTask(DWORD dwTaskID, IUI_APPCOMMAND nCmd) const;

	BOOL GetSelectedTaskDependencies(CStringArray& aDepends) const;
	BOOL SetSelectedTaskDependencies(const CStringArray& aDepends);

	DWORD HitTestTask(const CPoint& ptScreen) const;
	BOOL PointInHeader(const CPoint& ptScreen) const;
	void GetWindowRect(CRect& rWindow, BOOL bWithHeader = TRUE) const;

	void ExpandAll(BOOL bExpand = TRUE);
	void ExpandItem(HTREEITEM hti, BOOL bExpand = TRUE, BOOL bAndChildren = FALSE);
	BOOL CanExpandItem(HTREEITEM hti, BOOL bExpand = TRUE) const;

	void Resize(const CRect& rect);
	void ResizeColumnsToFit();

	BOOL ZoomIn(BOOL bIn = TRUE);
	BOOL ZoomBy(int nAmount);

	void SetFocus();
	BOOL HasFocus() const { return CTreeListSyncer::HasFocus(); }

	void Sort(GTLC_COLUMN nBy, BOOL bAllowToggle, BOOL bAscending = -1);
	GTLC_COLUMN GetSortColumn() const { return m_nSortBy; }
	BOOL GetSortAscending() const { return m_bSortAscending; }

	GTLC_MONTH_DISPLAY GetMonthDisplay() const { return m_nMonthDisplay; }
	BOOL SetMonthDisplay(GTLC_MONTH_DISPLAY nNewDisplay);
	BOOL CanSetMonthDisplay(GTLC_MONTH_DISPLAY nDisplay) const;
	BOOL ValidateMonthDisplay(GTLC_MONTH_DISPLAY& nDisplay) const;
	void ValidateMonthDisplay();

	void ScrollToToday();
	void ScrollToSelectedTask();
	void ScrollToTask(DWORD dwTaskID);

	void SetOption(DWORD dwOption, BOOL bSet = TRUE);
	BOOL HasOption(DWORD dwOption) const { return (m_dwOptions & dwOption); }

	BOOL HandleEraseBkgnd(CDC* pDC) { return CTreeListSyncer::HandleEraseBkgnd(pDC); }
	void SetAlternateLineColor(COLORREF crAltLine);
	void SetGridLineColor(COLORREF crGridLine);
	void SetTodayColor(COLORREF crToday);
	void SetWeekendColor(COLORREF crWeekend);
	void SetDefaultColor(COLORREF crDefault);
	void SetParentColoring(GTLC_PARENTCOLORING nOption, COLORREF color);
	void SetMilestoneTag(const CString& sTag);

	BOOL CancelOperation();
	void SetReadOnly(bool bReadOnly) { m_bReadOnly = bReadOnly; }
	BOOL GetLabelEditRect(LPRECT pEdit) const;
	CString GetItemTip(CPoint ptScreen) const;
	HTREEITEM GetItem(CPoint ptScreen) const;

	void SetSnapMode(GTLC_SNAPMODE nSnap) { m_nSnapMode = nSnap; }
	GTLC_SNAPMODE GetSnapMode() const;

	BOOL BeginDependencyEdit(CGanttDependencyEditor* pDependEdit);
	void OnEndDepedencyEdit();

	int GetColumnOrder(CIntArray& aTreeOrder) const;
	BOOL SetColumnOrder(const CIntArray& aTreeOrder);
	void GetColumnWidths(CIntArray& aTreeWidths, CIntArray& aListWidths) const;
	BOOL SetColumnWidths(const CIntArray& aTreeWidths, const CIntArray& aListWidths);
	BOOL SetTrackedColumns(const CIntArray& aTreeTracked, const CIntArray& aListTracked);
	void GetTrackedColumns(CIntArray& aTreeTracked, CIntArray& aListTracked) const;

	static BOOL WantEditUpdate(IUI_ATTRIBUTE nAttribute);
	static BOOL WantSortUpdate(IUI_ATTRIBUTE nAttribute);
	static IUI_ATTRIBUTE MapColumnToAttrib(GTLC_COLUMN nCol);

protected:
	BOOL m_bReadOnly;
	BOOL m_bSortAscending;

	CEnHeaderCtrl m_treeHeader, m_listHeader;
	CGanttDependencyEditor* m_pDependEdit;
	CMap<GTLC_MONTH_DISPLAY, GTLC_MONTH_DISPLAY, int, int> m_mapMinMonthWidths;
	CIntArray m_aPrevColWidths, m_aPrevTrackedCols;
	COLORREF m_crAltLine, m_crGridLine, m_crToday, m_crWeekend, m_crParent, m_crDefault;
	COleDateTime m_dtDragMin;
	GANTTDATERANGE m_dateRange;
	CPoint m_ptDragStart, m_ptLastDependPick;
	DWORD m_dwOptions;
	DWORD m_dwMaxTaskID;
	GANTTITEM m_giPreDrag;
	GTLC_COLUMN m_nSortBy;
	GTLC_MONTH_DISPLAY m_nMonthDisplay;
	GTLC_PARENTCOLORING m_nParentColoring;
	int m_nMonthWidth;
	CString m_sMilestoneTag;
	GTLC_DRAG m_nDragging;

	CGanttTreeCtrl& m_tree;
	CListCtrl& m_list;

	CGanttItemMap m_data;

private:
	mutable CTreeCtrlHelper* m_pTCH;
	mutable GTLC_SNAPMODE m_nSnapMode;

protected:
	LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	LRESULT ScWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

protected:
	// base-class virtual message handlers
	LRESULT OnTreeCustomDraw(NMTVCUSTOMDRAW* pTVCD);
	LRESULT OnListCustomDraw(NMLVCUSTOMDRAW* pLVCD);
	LRESULT OnHeaderCustomDraw(NMCUSTOMDRAW* pNMCD);

	// pseudo-message handler
	void OnHeaderDividerDblClk(NMHEADER* HDN);

	BOOL DrawListItemColumn(CDC* pDC, int nItem, int nCol, DWORD dwTaskID, BOOL bSelected);
	void PostDrawListItem(CDC* pDC, int nItem, DWORD dwTaskID);
	void DrawTreeItem(CDC* pDC, HTREEITEM hti, int nCol, const GANTTITEM& gi, BOOL bSelected, COLORREF crBack = CLR_NONE);
	void DrawListHeaderItem(CDC* pDC, int nCol);
	void DrawListHeaderRect(CDC* pDC, const CRect& rItem, const CString& sItem, CThemed* pTheme);
	GM_ITEMSTATE GetItemState(int nItem) const;
	GM_ITEMSTATE GetItemState(HTREEITEM hti) const;
	
	void RedrawList(BOOL bErase = FALSE);
	void RedrawTree(BOOL bErase = FALSE);

	void DrawListItemYears(CDC* pDC, const CRect& rItem, int nYear, int nNumYears,
							const GANTTITEM& gi, BOOL bSelected, BOOL& bToday);
	void DrawListItemYear(CDC* pDC, const CRect& rYear, int nYear, 
							const GANTTITEM& gi, BOOL bSelected, BOOL& bToday);
	void DrawListItemMonths(CDC* pDC, const CRect& rItem, int nMonth, int nNumMonths, int nYear, 
							const GANTTITEM& gi, BOOL bSelected, BOOL& bToday);
	void DrawListItemMonth(CDC* pDC, const CRect& rMonth, int nMonth, int nYear, 
							const GANTTITEM& gi, BOOL bSelected, BOOL& bToday);
	void DrawListItemWeeks(CDC* pDC, const CRect& rMonth, int nMonth, int nYear, 
							const GANTTITEM& gi, BOOL bSelected, BOOL& bToday);
	void DrawListItemDays(CDC* pDC, const CRect& rMonth, int nMonth, int nYear, 
							const GANTTITEM& gi, BOOL bSelected, BOOL& bToday, BOOL bDrawHours);

	void DrawGanttBar(CDC* pDC, const CRect& rMonth, int nMonth, int nYear, const GANTTITEM& gi);
	void DrawGanttDone(CDC* pDC, const CRect& rMonth, int nMonth, int nYear, const GANTTITEM& gi);
	void DrawGanttMilestone(CDC* pDC, const CRect& rMonth, int nMonth, int nYear, const GANTTITEM& gi);

	void DrawWeekend(CDC* pDC, const COleDateTime& dtDay, const CRect& rDay);
	BOOL DrawToday(CDC* pDC, const CRect& rMonth, int nMonth, int nYear, BOOL bSelected);
	void DrawGanttParentEnds(CDC* pDC, const GANTTITEM& gi, const CRect& rBar, 
							 const COleDateTime& dtMonthStart, const COleDateTime& dtMonthEnd);

	enum DIV_TYPE { DIV_NONE = -1, DIV_VERT_LIGHT, DIV_VERT_MID, DIV_VERT_DARK, DIV_HORZ };

	void DrawItemDivider(CDC* pDC, const CRect& rItem, DIV_TYPE nType, BOOL bSelected);
	DIV_TYPE GetVerticalDivider(int nMonth, int nYear) const;

	void ClearDependencyPickLine(CDC* pDC = NULL);
	BOOL DrawDependencyPickLine(const CPoint& ptClient);
	void EndDependencyEdit();

	void BuildListColumns();
	void UpdateListColumns(int nWidth = -1);
	void RecalcListColumnWidths(int nFromWidth, int nToWidth);
	void UpdateColumnsWidthAndText(int nWidth = -1);
	CString FormatColumnHeaderText(GTLC_MONTH_DISPLAY nDisplay, int nMonth = 0, int nYear = 0) const;

	int GetListItem(HTREEITEM hti) const;
	void ExpandList(HTREEITEM hti, int& nNextIndex);
	void CollapseList(HTREEITEM hti);
	void ExpandList();
	void GetTreeItemRect(HTREEITEM hti, int nCol, CRect& rItem, BOOL bText = FALSE) const;
	HFONT GetTreeItemFont(HTREEITEM hti, const GANTTITEM& gi, int nCol);
	BOOL IsTreeItemLineOdd(HTREEITEM hti) const;
	BOOL IsListItemLineOdd(int nItem) const;
	BOOL GetListColumnDate(int nCol, int& nMonth, int& nYear) const;
	void CalcMinMonthWidths();
	int GetMinMonthWidth(GTLC_MONTH_DISPLAY nDisplay) const;
	void BuildTreeColumns();
	GTLC_MONTH_DISPLAY GetColumnDisplay(int nColWidth);
	int GetColumnWidth() const;
	int GetColumnWidth(GTLC_MONTH_DISPLAY nDisplay) const;
	double GetMonthWidth(int nColWidth) const;
	int GetRequiredColumnCount() const;
	int GetRequiredColumnCount(GTLC_MONTH_DISPLAY nDisplay) const;
	BOOL ZoomTo(GTLC_MONTH_DISPLAY nNewDisplay, int nNewMonthWidth);
	void DeleteTreeItem(HTREEITEM hti);
	void RemoveDeletedTasks(HTREEITEM hti, const ITASKLISTBASE* pTasks, const CSet<DWORD>& mapIDs);
	int FindColumn(int nScrollPos) const;
	int FindColumn(int nMonth, int nYear) const;
	int FindColumn(const COleDateTime& date) const;
	int GetDateInMonths(int nMonth, int nYear) const;
	BOOL GetDateFromScrollPos(int nScrollPos, COleDateTime& date) const;
	int GetScrollPosFromDate(const COleDateTime& date) const;
	int GetDrawPosFromDate(const COleDateTime& date) const;
	BOOL GetListColumnRect(int nCol, CRect& rect, BOOL bScrolled = TRUE) const;
	void ScrollTo(const COleDateTime& date);
	void InitItemHeights();
	int CalcTreeWidth() const;
	int GetStartYear(GTLC_MONTH_DISPLAY nDisplay) const;
	int GetEndYear(GTLC_MONTH_DISPLAY nDisplay) const;
	COleDateTime GetStartDate(GTLC_MONTH_DISPLAY nDisplay) const;
	COleDateTime GetEndDate(GTLC_MONTH_DISPLAY nDisplay) const;
	int GetNumMonths(GTLC_MONTH_DISPLAY nDisplay) const;
	void Resize();
	BOOL ValidateMonthDisplay(GTLC_MONTH_DISPLAY& nDisplay, int& nWidth) const;
	BOOL CanSetMonthDisplay(GTLC_MONTH_DISPLAY nDisplay, int nWidth) const;
	void RecalcDateRange();
   BOOL GetListItemRect(int nItem, CRect& rItem) const;
	void IncrementItemPositions(HTREEITEM htiParent, int nFromPos);

	inline BOOL HasGridlines() const { return (m_crGridLine != CLR_NONE); }

	GANTTITEM* GetGanttItem(DWORD dwTaskID, BOOL bCopyRefID = TRUE) const;
	BOOL RestoreGanttItem(const GANTTITEM& giPrev);

	HTREEITEM TreeHitTestItem(const CPoint& point, BOOL bScreen) const;
	DWORD TreeHitTestTask(const CPoint& point, BOOL bScreen) const;
	int ListHitTestItem(const CPoint& point, BOOL bScreen, int& nCol) const;
	DWORD ListHitTestTask(const CPoint& point, BOOL bScreen, GTLC_HITTEST& nHit, BOOL bDragging) const;
	DWORD ListDependsHitTest(const CPoint& ptClient, DWORD& dwToTaskID);
	DWORD HitTestTask(const CPoint& point, BOOL bScreen, int& nItem) const;
	int FindListItem(DWORD dwTaskID, const CHTIMap& mapItems) const;
	int FindListItem(DWORD dwTaskID) const;

	DWORD GetTaskID(HTREEITEM hti) const;
	DWORD GetTaskID(int nItem) const;
	DWORD GetListTaskID(DWORD nItemData) const;

	BOOL StartDragging(const CPoint& ptCursor);
	BOOL EndDragging(const CPoint& ptCursor);
	BOOL UpdateDragging(const CPoint& ptCursor);
	BOOL ValidateDragPoint(CPoint& ptDrag) const;
	BOOL IsValidDragPoint(const CPoint& ptDrag) const;
	void CancelDrag(BOOL bReleaseCapture);
	BOOL IsDragging() const;
	void GetDragLimits(CRect& rLimits) const;
	BOOL GetValidDragDate(const CPoint& ptCursor, COleDateTime& dtDrag) const;
	BOOL GetDateFromPoint(const CPoint& ptCursor, COleDateTime& date) const;
	COleDateTime GetNearestDate(const COleDateTime& date) const;
	COleDateTime CalcMinDragDate(const GANTTITEM& gi) const;
	double CalcMinDragDuration() const;
	BOOL CanDragTask(DWORD dwTaskID, GTLC_DRAG nDrag = GTLCD_ANY) const;

	BOOL NotifyParentDateChange(GTLC_DRAG nDrag);
	void NotifyParentDragChange();

	BOOL RecalcTreeColumns(BOOL bResize = TRUE);
	void RecalcTreeColumnWidth(int nCol, CDC* pDC = NULL);
	CString GetLongestVisibleAllocTo(HTREEITEM hti);
	CString GetTreeItemColumnText(const GANTTITEM& gi, int nCol);
	BOOL IsMilestone(const GANTTITEM& gi) const;
	int CalcWidestItemTitle(HTREEITEM htiParent, CDC* pDC) const;
	void RefreshItemBoldState(HTREEITEM hti = NULL, BOOL bAndChildren = TRUE);
	BOOL CalcMilestoneRect(const GANTTITEM& gi, const CRect& rMonth, CRect& rMilestone) const;
	int GetBestTextPos(const GANTTITEM& gi, const CRect& rMonth) const;

	BOOL HasAltLineColor() const { return (m_crAltLine != CLR_NONE); }
	void GetGanttBarColors(const GANTTITEM& gi, COLORREF& crBorder, COLORREF& crFill) const;
 	COLORREF GetTreeTextColor(const GANTTITEM& gi, BOOL bSelected, BOOL bLighter = FALSE) const;
	COLORREF GetTreeTextBkColor(const GANTTITEM& gi, BOOL bSelected, BOOL bAlternate) const;
	void SetColor(COLORREF& color, COLORREF crNew);

	void RebuildTree(const ITASKLISTBASE* pTasks);
	void BuildTreeItem(const ITASKLISTBASE* pTasks, HTASKITEM hTask, CTreeCtrl& tree, HTREEITEM htiParent, BOOL bAndSiblings);
	BOOL UpdateTask(const ITASKLISTBASE* pTasks, HTASKITEM hTask, IUI_UPDATETYPE nUpdate, const CSet<IUI_ATTRIBUTE>& attrib, BOOL bAndSiblings);
	void RecalcParentDates();
	void RecalcParentDates(HTREEITEM htiParent, GANTTITEM*& pGI);
	BOOL GetTaskStartDueDates(const GANTTITEM& gi, COleDateTime& dtStart, COleDateTime& dtDue) const;
	BOOL HasDisplayDates(const GANTTITEM& gi) const;
	BOOL HasDoneDate(const GANTTITEM& gi) const;

	int GetExpandedState(CDWordArray& aExpanded, HTREEITEM hti = NULL) const;
	void SetExpandedState(const CDWordArray& aExpanded);

	CTreeCtrlHelper& TCH() { return m_tree.TCH(); }
	const CTreeCtrlHelper& TCH() const { return m_tree.TCH(); }

	BOOL CalcDependencyEndPos(DWORD dwTaskID, const CHTIMap& mapItems, GANTTDEPENDENCY& depend, BOOL bTo, LPPOINT lpp = NULL) const;
	BOOL CalcDependencyEndPos(DWORD dwTaskID, GANTTDEPENDENCY& depend, BOOL bTo, LPPOINT lpp = NULL) const;
	BOOL BuildDependency(DWORD dwFromTaskID, DWORD, const CHTIMap& mapItems, GANTTDEPENDENCY& depend) const;
	int BuildVisibleDependencyList(CGanttDependArray& aDepends) const;
	BOOL IsDependencyPickLinePosValid() const;
	void ResetDependencyPickLinePos();

	BOOL IsDependencyEditing() const;
	BOOL IsPickingDependencyFromTask() const;
	BOOL IsPickingFromDependency() const;
	BOOL IsPickingDependencyToTask() const;
	BOOL IsDependencyEditingCancelled() const;
	BOOL IsDependencyEditingComplete() const;

	static int CALLBACK SortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int GetColumnWidth(GTLC_MONTH_DISPLAY nDisplay, int nMonthWidth);
	static COleDateTime GetDate(time64_t tDate, BOOL bEndOfDay);
	static COLORREF GetColor(COLORREF crBase, double dLighter, BOOL bSelected);
	static BOOL CalcDateRect(const CRect& rMonth, int nMonth, int nYear, 
							const COleDateTime& dtFrom, const COleDateTime& dtTo, CRect& rDate);
	static BOOL CalcDateRect(const CRect& rMonth, int nDaysInMonth, 
							const COleDateTime& dtMonthStart, const COleDateTime& dtMonthEnd, 
							const COleDateTime& dtFrom, const COleDateTime& dtTo, CRect& rDate);
	static BOOL GetMonthDates(int nMonth, int nYear, COleDateTime& dtStart, COleDateTime& dtEnd);
	static CString GetTaskAllocTo(const ITASKLISTBASE* pTasks, HTASKITEM hTask);
	static int Compare(const CString& sText1, const CString& sText2);
	static BOOL CalcMinDragDuration(GTLC_SNAPMODE nMode, double& dMin);
	static void BuildTaskMap(const ITASKLISTBASE* pTasks, HTASKITEM hTask, CSet<DWORD>& mapIDs, BOOL bAndSiblings);
	static BOOL DragDatesDiffer(const GANTTITEM& gi1, const GANTTITEM& gi2);
	static void OffsetMonth(int& nMonth, int& nYear, int nNumMonths);
	static double GetMonthWidth(GTLC_MONTH_DISPLAY nDisplay, int nColWidth);
	static BOOL GetDateFromScrollPos(int nScrollPos, GTLC_MONTH_DISPLAY nDisplay, int nMonth, int nYear, const CRect& rColumn, COleDateTime& date);
	static int GetNumMonthsPerColumn(GTLC_MONTH_DISPLAY nDisplay);
	static BOOL IsVerticalDivider(DIV_TYPE nType);
	static BOOL IsDragging(GTLC_DRAG nDrag);
	static BOOL IsDraggingEnds(GTLC_DRAG nDrag);

private:
	void PreFixVScrollSyncBug();
	BOOL CalcDependencyEndPos(DWORD dwTaskID, int nItem, GANTTDEPENDENCY& depend, BOOL bTo, LPPOINT lpp) const;

};

#endif // !defined(AFX_GANTTTREELIST_H__016B94F3_1D28_4532_97EF_95F1D9D5CE55__INCLUDED_)
