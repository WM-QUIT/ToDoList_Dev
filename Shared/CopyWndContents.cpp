#include "StdAfx.h"

#include "CopyWndContents.h"
#include "treectrlhelper.h"

/////////////////////////////////////////////////////////////////////////////////////

CCopyWndContents::CCopyWndContents(CWnd& wnd) : m_wnd(wnd)
{
}

CCopyWndContents::~CCopyWndContents()
{
}

BOOL CCopyWndContents::DoCopy(CBitmap& bmp, const CRect& rFromTo)
{
	ASSERT(bmp.GetSafeHandle() == NULL);

	m_sizeContent = CalcContentsSize();
	m_sizePage = CalcPageSize();

	ASSERT((m_sizeContent.cx > 0) && (m_sizeContent.cy > 0));
	ASSERT((m_sizePage.cx > 0) && (m_sizePage.cy > 0));

	int nNumHorzPages = CalcPageCount(FALSE);
	int nNumVertPages = CalcPageCount(TRUE);

	// Adjust content size to match passed-in rect
	if (rFromTo.right != -1)
		m_sizeContent.cx = rFromTo.Width();
	else
		m_sizeContent.cx -= rFromTo.left;
	
	if (rFromTo.bottom != -1)
		m_sizeContent.cy = rFromTo.Height();
	else
		m_sizeContent.cy -= rFromTo.top;

	ASSERT((m_sizeContent.cx > 0) && (m_sizeContent.cy > 0));

	// create a temp dc to paint on
	CDC* pDC = m_wnd.GetDC();
	CDC dcContent, dcPage;
	CBitmap bmpPage;

	if (dcContent.CreateCompatibleDC(pDC) && dcPage.CreateCompatibleDC(pDC))
	{
		if (bmp.CreateCompatibleBitmap(pDC, m_sizeContent.cx, m_sizeContent.cy) &&
			bmpPage.CreateCompatibleBitmap(pDC, m_sizePage.cx, m_sizePage.cy))
		{
			// Move to origin
			m_wnd.SendMessage(WM_HSCROLL, SB_LEFT);
			m_wnd.SendMessage(WM_VSCROLL, SB_TOP);

			// prepare bitmaps
			CBitmap* pOldContent = dcContent.SelectObject(&bmp);
			dcContent.FillSolidRect(0, 0, m_sizeContent.cx, m_sizeContent.cy, GetSysColor(COLOR_WINDOW));

			CBitmap* pOldPage = dcPage.SelectObject(&bmpPage);
			CPoint ptPagePos(0, 0);

			for (int nVert = 0; nVert < nNumVertPages; nVert++)
			{
				// Only draw pages within passed-in rect
				if ((rFromTo.bottom != -1) && (ptPagePos.y >= rFromTo.bottom))
					break;

				if ((ptPagePos.y + m_sizePage.cy) > rFromTo.top)
				{
					for (int nHorz = 0; nHorz < nNumHorzPages; nHorz++)
					{
						// Only draw pages within passed-in rect
						if ((rFromTo.right != -1) && (ptPagePos.x >= rFromTo.right))
							break;

						if ((ptPagePos.x + m_sizePage.cx) > rFromTo.left)
						{
							// Copy the current page
							dcPage.FillSolidRect(0, 0, m_sizePage.cx, m_sizePage.cy, GetSysColor(COLOR_WINDOW));
							DoPrint(dcPage, ptPagePos.x, ptPagePos.y);

							// GetWindowOrg contains the point from which
							// we copy the page bitmap to the content bitmap
							CPoint ptDCOrg = dcPage.GetWindowOrg();
							dcPage.SetWindowOrg(0, 0);

							dcContent.BitBlt((ptPagePos.x - rFromTo.left), 
											(ptPagePos.y - rFromTo.top), 
											m_sizePage.cx, 
											m_sizePage.cy, 
											&dcPage, 
											ptDCOrg.x, 
											ptDCOrg.y, 
											SRCCOPY);
						}

						// Scroll one page to right
						ptPagePos.x = PageRight(ptPagePos.x);
					}

					// Reset horizontal scrollbar
					m_wnd.SendMessage(WM_HSCROLL, SB_LEFT);
					ptPagePos.x = 0;
				}

				// Scroll one page down
				ptPagePos.y = PageDown(ptPagePos.y);
			}

			// cleanup
			dcContent.SelectObject(pOldContent);
			dcPage.SelectObject(pOldPage);
		}
	}

	m_wnd.ReleaseDC(pDC);

	return TRUE;
}

void CCopyWndContents::DoPrint(CDC& dc, int /*nHPage*/, int /*nVPage*/)
{
	DoPrint(m_wnd, dc);
}

void CCopyWndContents::DoPrint(HWND hwnd, HDC hdc, DWORD dwFlags)
{
	::SendMessage(hwnd, WM_PRINT, (WPARAM)hdc, (LPARAM)dwFlags);
}

int CCopyWndContents::PageDown(int nCurVertPos)
{
	DoPageDown();

	int nScrollPos = GetContentScrollPos(TRUE);

	if (nScrollPos > 0)
		return nScrollPos;

	int nNewVertPos = (nCurVertPos + m_sizePage.cy);
	nNewVertPos = min(nNewVertPos, m_sizeContent.cy - m_sizePage.cy);

	return nNewVertPos;
}

int CCopyWndContents::PageRight(int nCurHorzPos)
{
	DoPageRight();

	int nScrollPos = GetContentScrollPos(FALSE);

	if (nScrollPos > 0)
		return nScrollPos;

	// else
	int nNewHorzPos = (nCurHorzPos + m_sizePage.cx);
	nNewHorzPos = min(nNewHorzPos, (m_sizeContent.cx - m_sizePage.cx));

	return nNewHorzPos;
}

void CCopyWndContents::DoPageDown()
{
	m_wnd.SendMessage(WM_VSCROLL, SB_PAGEDOWN);
}

void CCopyWndContents::DoPageRight()
{
	m_wnd.SendMessage(WM_HSCROLL, SB_PAGERIGHT);
}

CSize CCopyWndContents::CalcContentsSize() const
{
	CRect rClient;
	m_wnd.GetClientRect(rClient);

	CSize sizeContents(rClient.Width(), rClient.Height());

	if (m_wnd.GetStyle() & WS_HSCROLL)
	{
		SCROLLINFO siHorz = { 0 };
		m_wnd.GetScrollInfo(SB_HORZ, &siHorz);

		sizeContents.cx = (siHorz.nMax - siHorz.nMin + 1);
	}

	if (m_wnd.GetStyle() & WS_VSCROLL)
	{
		SCROLLINFO siVert = { 0 };
		m_wnd.GetScrollInfo(SB_VERT, &siVert);

		sizeContents.cy = (siVert.nMax - siVert.nMin + 1);
	}

	return sizeContents;
}

CSize CCopyWndContents::CalcPageSize() const
{
	CRect rClient;
	m_wnd.GetClientRect(rClient);

	return rClient.Size();
}

int CCopyWndContents::CalcPageCount(BOOL bVert) const
{
	CSize sizeContent = CalcContentsSize();
	CSize sizePage = CalcPageSize();

	int nCount = 0;

	if (bVert)
	{
		nCount = (sizeContent.cy / sizePage.cy);

		if (sizeContent.cy % sizePage.cy)
			nCount++;
	}
	else
	{
		nCount = (sizeContent.cx / sizePage.cx);

		if (sizeContent.cx % sizePage.cx)
			nCount++;
	}

	return nCount;
}

int CCopyWndContents::GetContentScrollPos(BOOL bVert) const
{
	if (m_wnd.GetStyle() & (bVert ? WS_VSCROLL : WS_HSCROLL))
	{
		SCROLLINFO si = { 0 };
		m_wnd.GetScrollInfo((bVert ? SB_VERT : SB_HORZ), &si);

		return si.nPos;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////

CCopyTreeCtrlContents::CCopyTreeCtrlContents(CTreeCtrl& tree) : CCopyWndContents(tree), m_tree(tree)
{
	HTREEITEM hti = (HTREEITEM)m_wnd.SendMessage(TVM_GETNEXTITEM, NULL, TVGN_CHILD);
	ASSERT(hti);

	m_nItemHeight = TreeView_GetItemHeight(m_wnd);
}

CCopyTreeCtrlContents::~CCopyTreeCtrlContents()
{

}

void CCopyTreeCtrlContents::DoPageDown()
{
	int nLine = (GetPageSize().cy / m_nItemHeight);

	while (nLine--)
		m_wnd.SendMessage(WM_VSCROLL, SB_LINEDOWN);
}

CSize CCopyTreeCtrlContents::CalcContentsSize() const
{
	CSize sizeContent(CCopyWndContents::CalcContentsSize());

	sizeContent.cx++;

	CTreeCtrlHelper tch(m_tree);
	int nTotalVisible = tch.BuildVisibleIndexMap();
	sizeContent.cy = (nTotalVisible * m_nItemHeight);

	return sizeContent;
}

CSize CCopyTreeCtrlContents::CalcPageSize() const
{
	CSize sizePage(CCopyWndContents::CalcPageSize());

	int nPageVisible = m_tree.GetVisibleCount();
	sizePage.cy = (nPageVisible * m_nItemHeight);

	return sizePage;
}

int CCopyTreeCtrlContents::GetContentScrollPos(BOOL bVert) const
{
	int nPos = CCopyWndContents::GetContentScrollPos(bVert);

	if (bVert)
	{
		if (nPos != 0)
		{
			nPos *= m_nItemHeight;
		}
		else
		{
			HTREEITEM hti = m_tree.GetFirstVisibleItem();
			CRect rItem;
			
			if (hti && m_tree.GetItemRect(hti, rItem, FALSE))
				nPos = rItem.top;
		}
	}

	return nPos;
}

/////////////////////////////////////////////////////////////////////////////////////

CCopyHeaderCtrlContents::CCopyHeaderCtrlContents(CHeaderCtrl& hdr) : CCopyWndContents(hdr)
{
}

CCopyHeaderCtrlContents::~CCopyHeaderCtrlContents()
{
}

/////////////////////////////////////////////////////////////////////////////////////

CCopyListCtrlContents::CCopyListCtrlContents(CListCtrl& list) : CCopyWndContents(list), m_list(list)
{
	CRect rect;
	
	if (m_list.GetItemRect(0, &rect, LVIR_BOUNDS))
		m_nItemHeight = rect.Height();
	else
		m_nItemHeight = 10;
}

CCopyListCtrlContents::~CCopyListCtrlContents()
{

}

int CCopyListCtrlContents::PageDown(int nCurVertPos)
{
	int nNewVPos = CCopyWndContents::PageDown(nCurVertPos);

	return (nNewVPos + CalcHeaderHeight());
}

int CCopyListCtrlContents::CalcPageCount(BOOL bVert) const
{
	if (!bVert)
		return CCopyWndContents::CalcPageCount(FALSE);

	// else
	int nHeaderHeight = CalcHeaderHeight();
	int nSizeContent = CalcContentsSize().cy - nHeaderHeight;
	int nSizePage = CalcPageSize().cy - nHeaderHeight;

	if (nSizePage == 0)
		return 1;

	// else
	int nCount = (nSizeContent / nSizePage);

	if (nSizeContent % nSizePage)
		nCount++;

	return max(nCount, 1);
}

void CCopyListCtrlContents::DoPageDown()
{
	// Avoid animations by not using WM_VSCROLL
	CSize amount(0, (m_list.GetCountPerPage() * m_nItemHeight));

	m_list.Scroll(amount);
}

void CCopyListCtrlContents::DoPageRight()
{
	// Avoid animations by not using WM_VHCROLL
	CRect rClient;
	m_list.GetClientRect(rClient);

	CSize amount(rClient.Width(), 0);
	
	m_list.Scroll(amount);
}

CSize CCopyListCtrlContents::CalcContentsSize() const
{
	CSize sizeContent(CCopyWndContents::CalcContentsSize());

	// for whole lines
	int nTotalVisible = m_list.GetItemCount();
	sizeContent.cy = (nTotalVisible * m_nItemHeight);

	// add height of header
	sizeContent.cy += CalcHeaderHeight();

	// Restrict width to total width of columns
	HWND hwndHdr = ListView_GetHeader(m_wnd);
	
	if (hwndHdr)
	{
		int nCol = Header_GetItemCount(hwndHdr);
		int nWidth = 0;

		while (nCol--)
			nWidth += m_list.GetColumnWidth(nCol);

		sizeContent.cx = min(sizeContent.cx, nWidth);
	}

	return sizeContent;
}

CSize CCopyListCtrlContents::CalcPageSize() const
{
	CSize sizePage(CCopyWndContents::CalcPageSize());

 	// adjust for header and whole lines
	int nHeaderHeight = CalcHeaderHeight();

	if (m_list.GetCountPerPage() > 0)
	{
		sizePage.cy = ((m_list.GetCountPerPage() * m_nItemHeight) + nHeaderHeight);
	}
	else // item height > page size
	{
		sizePage.cy += nHeaderHeight;
	}

	return sizePage;
}

int CCopyListCtrlContents::CalcHeaderHeight() const
{
	HWND hwndHdr = ListView_GetHeader(m_wnd);

	if (hwndHdr && ::IsWindowVisible(hwndHdr))
	{
		CRect rHeader;
		VERIFY(::GetWindowRect(hwndHdr, rHeader));

		return rHeader.Height();
	}

	// else
	return 0;
}

void CCopyListCtrlContents::DoPrint(CDC& dc, int /*nHPage*/, int nVPage)
{
	CPoint ptOrg = dc.GetWindowOrg();
	CCopyWndContents::DoPrint(m_wnd, dc, PRF_CLIENT | PRF_CHILDREN);

	if (nVPage != 0)
		dc.SetWindowOrg(ptOrg.x, CalcHeaderHeight());
}

int CCopyListCtrlContents::GetContentScrollPos(BOOL bVert) const
{
	int nPos = CCopyWndContents::GetContentScrollPos(bVert);

	if (bVert)
	{
		if (nPos != 0)
		{
			nPos *= m_nItemHeight;
		}
		else
		{
			int nItem = m_list.GetTopIndex();
			CRect rItem;
			
			if ((nItem != -1) && m_list.GetItemRect(nItem, rItem, LVIR_BOUNDS))
				nPos = rItem.top;
		}
	}

	return nPos;
}

/////////////////////////////////////////////////////////////////////////////////////

CCopyEditContents::CCopyEditContents(CEdit& edit) 
	: 
	CCopyWndContents(edit), 
	m_nLineHeight(-1), 
	m_nLineCount(-1)
{
	int nFirstCharLine0 = edit.LineIndex(0);
	int nFirstCharLine1 = edit.LineIndex(1);

	CPoint ptTopLeftLine0 = edit.PosFromChar(nFirstCharLine0);
	CPoint ptTopLeftLine1 = edit.PosFromChar(nFirstCharLine1);

	m_nLineHeight = (ptTopLeftLine1 - ptTopLeftLine0).cy;
	m_nLineCount = edit.GetLineCount();
}

CCopyEditContents::~CCopyEditContents()
{

}

void CCopyEditContents::DoPageDown()
{
	int nLine = (GetPageSize().cy / m_nLineHeight);

	while (nLine--)
		m_wnd.SendMessage(WM_VSCROLL, SB_LINEDOWN);
}

CSize CCopyEditContents::CalcContentsSize() const
{
	CSize sizeContent(CCopyWndContents::CalcContentsSize());

	sizeContent.cx++;
	sizeContent.cy = (m_nLineCount * m_nLineHeight) + 2;

	return sizeContent;
}

CSize CCopyEditContents::CalcPageSize() const
{
	CRect rect;
	m_wnd.SendMessage(EM_GETRECT, 0, (LPARAM)&rect);

	return CSize(rect.Width(), ((rect.Height() / m_nLineHeight) * m_nLineHeight));
}

/////////////////////////////////////////////////////////////////////////////////////
