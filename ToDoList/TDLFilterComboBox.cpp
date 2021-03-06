// TDLFilterComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "TDLFilterComboBox.h"
#include "tdcstatic.h"

#include "..\shared\enstring.h"
#include "..\shared\dialoghelper.h"
#include "..\shared\misc.h"
#include "..\shared\holdredraw.h"
#include "..\shared\dialoghelper.h"
#include "..\shared\localizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLFilterComboBox

CTDLFilterComboBox::CTDLFilterComboBox() : m_bShowDefaultFilters(TRUE)
{
}

CTDLFilterComboBox::~CTDLFilterComboBox()
{
}


BEGIN_MESSAGE_MAP(CTDLFilterComboBox, CTabbedComboBox)
	//{{AFX_MSG_MAP(CTDLFilterComboBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLFilterComboBox message handlers

void CTDLFilterComboBox::PreSubclassWindow() 
{
	CTabbedComboBox::PreSubclassWindow();

	FillCombo();
}

int CTDLFilterComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTabbedComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	FillCombo();
	
	return 0;
}

int CTDLFilterComboBox::AddDefaultFilterItem(int nItem)
{
	ASSERT(nItem >= 0 && nItem < NUM_SHOWFILTER);
	
	CEnString sFilter(SHOW_FILTERS[nItem][0]);
	TCHAR cLetter = (TCHAR)('A' + nItem);
	
	CString sItem;

	if (cLetter > 'Z')
		sItem.Format(_T("\t%s"), sFilter); 
	else
		sItem.Format(_T("%c)\t%s"), cLetter, sFilter); 

	UINT nFilter = SHOW_FILTERS[nItem][1];
	
	return CDialogHelper::AddString(*this, sItem, nFilter);
}

void CTDLFilterComboBox::FillCombo()
{
	ASSERT(GetSafeHwnd());
	
	if (GetCount())
		return; // already called

	CLocalizer::EnableTranslation(*this, FALSE);
	
	if (m_bShowDefaultFilters)
	{
		for (int nItem = 0; nItem < NUM_SHOWFILTER; nItem++)
			AddDefaultFilterItem(nItem);
	}
	else // always show 'All Tasks' filter
	{
		AddDefaultFilterItem(0);
	}
	
	// custom filters
	for (int nItem = 0; nItem < m_aAdvancedFilterNames.GetSize(); nItem++)
	{
		CString sFilter = FormatAdvancedFilterDisplayString(nItem, m_aAdvancedFilterNames[nItem]);	
		CDialogHelper::AddString(*this, sFilter, (DWORD)FS_ADVANCED);
	}

	// resize to fit widest item
	CDialogHelper::RefreshMaxDropWidth(*this, NULL, TABSTOPS);
}

void CTDLFilterComboBox::RefillCombo(LPCTSTR szCustomSel)
{
	if (GetSafeHwnd())
	{
		CHoldRedraw hr(*this);
		
		// save selection
		CString sCustom;
		FILTER_SHOW nSelFilter = GetSelectedFilter(sCustom);
		
		ResetContent();
		FillCombo();
		
		// restore selection
		RestoreSelection(nSelFilter, (szCustomSel ? szCustomSel : sCustom));
	}
}

FILTER_SHOW CTDLFilterComboBox::GetSelectedFilter() const
{
	return (FILTER_SHOW)CDialogHelper::GetSelectedItemData(*this);
}

FILTER_SHOW CTDLFilterComboBox::GetSelectedFilter(CString& sCustom) const
{
	FILTER_SHOW nSelFilter = GetSelectedFilter();

	if (nSelFilter == FS_ADVANCED)
	{
		CString sFilter = CDialogHelper::GetSelectedItem(*this);
		sCustom = ExtractAdvancedFilterName(sFilter);
	}
	else
		sCustom.Empty();

	return nSelFilter;
}

BOOL CTDLFilterComboBox::SelectAdvancedFilter(FILTER_SHOW nFilter)
{
	return CDialogHelper::SelectItemByData(*this, (DWORD)nFilter);
}

BOOL CTDLFilterComboBox::SelectAdvancedFilter(const CString& sAdvFilter)
{
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		if (FS_ADVANCED == GetItemData(nItem))
		{
			CString sFilter;
			GetLBText(nItem, sFilter);

			// exact test
			if (sFilter == sAdvFilter)
			{
				SetCurSel(nItem);
				return TRUE;
			}
			// partial test
			else if (sFilter.Find(sAdvFilter) != -1)
			{
				// then full test
				int nFilter = nItem - (m_bShowDefaultFilters ? NUM_SHOWFILTER : 0);
				CString sFull = FormatAdvancedFilterDisplayString(nFilter, sAdvFilter);	

				if (sFilter == sFull)
				{
					SetCurSel(nItem);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void CTDLFilterComboBox::AddAdvancedFilters(const CStringArray& aFilters, LPCTSTR szCustomSel)
{
	m_aAdvancedFilterNames.Copy(aFilters);

	if (GetSafeHwnd())
		RefillCombo(szCustomSel);
}

int CTDLFilterComboBox::GetAdvancedFilters(CStringArray& aFilters) const
{
	aFilters.Copy(m_aAdvancedFilterNames);
	return aFilters.GetSize();
}

void CTDLFilterComboBox::RemoveAdvancedFilters()
{
	m_aAdvancedFilterNames.RemoveAll();

	if (GetSafeHwnd())
		RefillCombo();
}

void CTDLFilterComboBox::ShowDefaultFilters(BOOL bShow)
{
	if (m_bShowDefaultFilters == bShow)
		return;

	m_bShowDefaultFilters = bShow;

	if (GetSafeHwnd())
		RefillCombo();
}

void CTDLFilterComboBox::RestoreSelection(FILTER_SHOW nFilter, LPCTSTR szCustom)
{
	if (nFilter == FS_ADVANCED)
	{
		if (SelectAdvancedFilter(szCustom))
			return;
	}
	else if (SelectAdvancedFilter(nFilter))
	{
		return;
	}

	// else
	SetCurSel(0);

	// notify parent of selection change
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CBN_SELCHANGE), (LPARAM)GetSafeHwnd());
}

CString CTDLFilterComboBox::FormatAdvancedFilterDisplayString(int nFilter, const CString& sFilter, BOOL bIncCustomLabel)
{
	// if it starts with a tab then it's already done
	if (!sFilter.IsEmpty() && sFilter[0] == '\t')
		return sFilter;

	CEnString sAdvanced(IDS_ADVANCEDFILTER);
	CString sDisplay, sNumeral(Misc::Format(nFilter));	

	if (sFilter.IsEmpty())
	{
		sDisplay.Format(_T("%s)\t%s"), sNumeral, sAdvanced);
	}
	else if (bIncCustomLabel)
	{
		sDisplay.Format(_T("%s)\t%s (%s)"), sNumeral, sFilter, sAdvanced);
	}
	else // filter only
	{
		sDisplay.Format(_T("%s)\t%s"), sNumeral, sFilter);
	}

	return sDisplay;
}

CString CTDLFilterComboBox::ExtractAdvancedFilterName(const CString& sDisplay)
{
	// if it doesn't have a tab then it's already done
	int nTab = sDisplay.Find('\t');

	if (nTab == -1)
		return sDisplay;

	// check for custom filter string
	CEnString sAdvanced(IDS_ADVANCEDFILTER);
	ASSERT(sDisplay.Find(sAdvanced) != -1);

	int nEnd = sDisplay.Find(_T(" ("));

	if (nEnd == -1)
		return _T("");

	// else
	sAdvanced = sDisplay.Mid(nTab + 1, nEnd - nTab - 1);
	return sAdvanced;
}

void CTDLFilterComboBox::DrawItemText(CDC& dc, const CRect& rect, int nItem, UINT nItemState,
										DWORD dwItemData, const CString& sItem, BOOL bList)
{
	CString sText(sItem);

	// if it's a custom filter and the non-list item
	// remove the (Find Tasks Filter) bit
	if ((FS_ADVANCED == dwItemData) && !bList)
	{
		CString sFilter = ExtractAdvancedFilterName(sItem);

		if (!sFilter.IsEmpty())
		{
			int nFilter = nItem;
		
			if (m_bShowDefaultFilters)
				nFilter -= NUM_SHOWFILTER;

			sText = FormatAdvancedFilterDisplayString(nFilter, sFilter, FALSE);
		}
	}

	// default drawing
	CTabbedComboBox::DrawItemText(dc, rect, nItem, nItemState, dwItemData, sText, bList);
}
