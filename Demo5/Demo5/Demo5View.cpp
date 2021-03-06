﻿
// Demo5View.cpp: CDemo5View 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Demo5.h"
#endif

#include "Demo5Doc.h"
#include "Demo5View.h"

#include <iostream>
#include <vector>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDemo5View

IMPLEMENT_DYNCREATE(CDemo5View, CView)

BEGIN_MESSAGE_MAP(CDemo5View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CDemo5View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_HERMITE, &CDemo5View::OnHermite)
	ON_COMMAND(ID_BEZIER, &CDemo5View::OnBezier)
	ON_COMMAND(ID_SPLINE, &CDemo5View::OnSpline)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_BEZIER_SURFACE, &CDemo5View::OnBezierSurface)
//	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CDemo5View 构造/析构

CDemo5View::CDemo5View() noexcept
{
	// TODO: 在此处添加构造代码

}

CDemo5View::~CDemo5View()
{
}

BOOL CDemo5View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}




// CDemo5View 打印


void CDemo5View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CDemo5View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CDemo5View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CDemo5View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CDemo5View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
#if 0
	ClientToScreen(&point);
	OnContextMenu(this, point);
#endif
}

void CDemo5View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CDemo5View 诊断

#ifdef _DEBUG
void CDemo5View::AssertValid() const
{
	CView::AssertValid();
}

void CDemo5View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDemo5Doc* CDemo5View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDemo5Doc)));
	return (CDemo5Doc*)m_pDocument;
}
#endif //_DEBUG


// CDemo5View 消息处理程序

enum DrawMode{Hermite = 0,Bezier,BSpline,BezierSurface};
DrawMode drawMode = Hermite;

vector<CPoint> hermiteCtls;// 0: first point, 1: first tangent, 2: second point, 3: second tangent
vector<CPoint> bezierCtls;
vector<CPoint> splineCtls;// accept any number of ctl pts >= 4
vector<vector<CPoint> > bezierSurfaceCtls;// bezier surface control points. 4 by 4


void CDemo5View::OnHermite()
{
	drawMode = Hermite;

	hermiteCtls.clear();

	Invalidate();
}


void CDemo5View::OnBezier()
{
	// TODO: 在此添加命令处理程序代码
	drawMode = Bezier;

	bezierCtls.clear();

	Invalidate();
}


void CDemo5View::OnSpline()
{
	// TODO: 在此添加命令处理程序代码
	drawMode = BSpline;

	splineCtls.clear();

	Invalidate();
}

void CDemo5View::OnBezierSurface()
{
	drawMode = BezierSurface;

	bezierSurfaceCtls.clear();

	Invalidate();
}


bool modifyCtl = false;// whether in modify control points mode

void CDemo5View::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!modifyCtl)
	{
		switch (drawMode)
		{
		case Hermite:
			if (hermiteCtls.size() < 4)
				hermiteCtls.push_back(point);
			break;
		case Bezier:
			if (bezierCtls.size() < 4)
				bezierCtls.push_back(point);
			break;
		case BSpline:
			splineCtls.push_back(point);
			break;
		case BezierSurface:
			
			break;
		default:
			break;
		}

	}
	else
	{
		switch (drawMode)
		{
		case Hermite:
			
			break;
		case Bezier:

			break;
		case BSpline:

			break;
		case BezierSurface:

			break;
		default:
			break;
		}
	}

	Invalidate();

	CView::OnLButtonDown(nFlags, point);
}


void CDemo5View::OnMouseMove(UINT nFlags, CPoint point)
{
	if (modifyCtl)
	{



		Invalidate();
	}


	

	CView::OnMouseMove(nFlags, point);
}


void CDemo5View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_CONTROL)
		modifyCtl = true;

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CDemo5View::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_CONTROL)
		modifyCtl = false;

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}



// CDemo5View 绘图
// vec: 4, matrix: 4 by 4
// memory allocated beforehand
void vectorMatrixProduct(const vector<float>& vec, const vector<vector<float> > & mat, vector<float>& rst)
{
	for (int k = 0; k < 4; ++k)
	{
		rst[k] = 0;
		for (int m = 0; m < 4; ++m)
			rst[k] += vec[m] * mat[m][k];
	}
}

void DrawHermite(const vector<CPoint>& hermiteCtls,CDC* pDC)
{
	if (hermiteCtls.empty())
		return;

	CBrush brush(RGB(128, 0, 0));
	CBrush* oldBrush = pDC->SelectObject(&brush);
	CPen penPoly(PS_SOLID, 2, RGB(10, 10, 128));
	CPen* oldPen = nullptr;
	// draw poly
	if (hermiteCtls.size() > 0)
	{
		pDC->Ellipse(hermiteCtls[0].x - 3, hermiteCtls[0].y - 3, hermiteCtls[0].x + 3, hermiteCtls[0].y + 3);
	}
	if (hermiteCtls.size() > 1)
	{
		pDC->Ellipse(hermiteCtls[1].x - 3, hermiteCtls[1].y - 3, hermiteCtls[1].x + 3, hermiteCtls[1].y + 3);

		pDC->MoveTo(hermiteCtls[0]);
		pDC->LineTo(hermiteCtls[1]);
	}
	if (hermiteCtls.size() > 2)
	{
		pDC->Ellipse(hermiteCtls[2].x - 3, hermiteCtls[2].y - 3, hermiteCtls[2].x + 3, hermiteCtls[2].y + 3);
	}
	if (hermiteCtls.size() > 3)
	{
		pDC->Ellipse(hermiteCtls[3].x - 3, hermiteCtls[3].y - 3, hermiteCtls[3].x + 3, hermiteCtls[3].y + 3);

		pDC->MoveTo(hermiteCtls[2]);
		pDC->LineTo(hermiteCtls[3]);
	}

	pDC->SelectObject(oldPen);

	if (hermiteCtls.size() != 4)
		return;
		
	vector<vector<float> > mat(4);
	vector<float> mat0(4);
	mat0[0] = 2; mat0[1] = -2; mat0[2] = 1; mat0[3] = 1;
	mat[0] = mat0;

	vector<float> mat1(4);
	mat1[0] = -3; mat1[1] = 3; mat1[2] = -2; mat1[3] = -1;
	mat[1] = mat1;

	vector<float> mat2(4);
	mat2[0] = 0; mat2[1] = 0; mat2[2] = 1; mat2[3] = 0;
	mat[2] = mat2;

	vector<float> mat3(4);
	mat3[0] = 1; mat3[1] = 0; mat3[2] = 0; mat3[3] = 0;
	mat[3] = mat3;

	CPen penCurve(PS_SOLID, 2, RGB(128, 10, 10));
	oldPen = pDC->SelectObject(&penCurve);
	// draw curve
	const int numSeg = 100;
	vector<float> uvec(4);
	float u = 0;
	vector<float> umat(4);
	CPoint curPt;
	// to get more beautiful effect, use other method for computing the points on the curve !!
	for (int k = 0; k <= numSeg; ++k)
	{
		u = 1. / numSeg * k;
		uvec[0] = u * u * u;
		uvec[1] = u * u;
		uvec[2] = u;
		uvec[3] = 1.;

		vectorMatrixProduct(uvec, mat, umat);

		curPt.x = 0;
		curPt.y = 0;
		
		curPt.x += hermiteCtls[0].x * umat[0];
		curPt.x += hermiteCtls[2].x * umat[1];
		curPt.x += hermiteCtls[1].x * umat[2];
		curPt.x += hermiteCtls[3].x * umat[3];

		curPt.y += hermiteCtls[0].y * umat[0];
		curPt.y += hermiteCtls[2].y * umat[1];
		curPt.y += hermiteCtls[1].y * umat[2];
		curPt.y += hermiteCtls[3].y * umat[3];
		
		if (k == 0)
			pDC->MoveTo(curPt);
		else
			pDC->LineTo(curPt);
	}



	pDC->SelectObject(oldPen);
}

void DrawBezierCurve(const vector<CPoint>& bezierCtls, CDC* pDC)
{
	if (bezierCtls.empty())
		return;

	CBrush brush(RGB(128, 0, 0));
	CBrush* oldBrush = pDC->SelectObject(&brush);
	CPen penPoly(PS_SOLID, 2, RGB(10, 10, 128));
	CPen* oldPen = nullptr;
	// draw poly
	for (int k = 0; k < bezierCtls.size(); ++k)
	{
		pDC->Ellipse(bezierCtls[k].x - 3, bezierCtls[k].y - 3, bezierCtls[k].x + 3, bezierCtls[k].y + 3);
	}
	pDC->MoveTo(bezierCtls[0]);
	for (int k = 1; k < bezierCtls.size(); ++k)
		pDC->LineTo(bezierCtls[k]);

	pDC->SelectObject(oldPen);

	if (bezierCtls.size() != 4)
		return;

	vector<vector<float> > mat(4);
	vector<float> mat0(4);
	mat0[0] = -1; mat0[1] = 3; mat0[2] = -3; mat0[3] = 1;
	mat[0] = mat0;

	vector<float> mat1(4);
	mat1[0] = 3; mat1[1] = -6; mat1[2] = 3; mat1[3] = 0;
	mat[1] = mat1;

	vector<float> mat2(4);
	mat2[0] = -3; mat2[1] = 3; mat2[2] = 0; mat2[3] = 0;
	mat[2] = mat2;

	vector<float> mat3(4);
	mat3[0] = 1; mat3[1] = 0; mat3[2] = 0; mat3[3] = 0;
	mat[3] = mat3;

	CPen penCurve(PS_SOLID, 2, RGB(128, 10, 10));
	oldPen = pDC->SelectObject(&penCurve);
	// draw curve
	const int numSeg = 100;
	vector<float> uvec(4);
	float u = 0;
	vector<float> umat(4);
	CPoint curPt;
	// to get more beautiful effect, use other method for computing the points on the curve !!
	for (int k = 0; k <= numSeg; ++k)
	{
		u = 1. / numSeg * k;
		uvec[0] = u * u * u;
		uvec[1] = u * u;
		uvec[2] = u;
		uvec[3] = 1.;

		vectorMatrixProduct(uvec, mat, umat);

		curPt.x = 0;
		curPt.y = 0;

		for (int m = 0; m < 4; ++m)
		{
			curPt.x += bezierCtls[m].x * umat[m];
			curPt.y += bezierCtls[m].y * umat[m];
		}

		if (k == 0)
			pDC->MoveTo(curPt);
		else
			pDC->LineTo(curPt);
	}



	pDC->SelectObject(oldPen);
}

void DrawBSpline(const vector<CPoint>& splineCtls, CDC* pDC)
{
	if (splineCtls.empty())
		return;

	CBrush brush(RGB(128, 0, 0));
	CBrush* oldBrush = pDC->SelectObject(&brush);
	CPen penPoly(PS_SOLID, 2, RGB(10, 10, 128));
	CPen* oldPen = nullptr;
	// draw poly
	for (int k = 0; k < splineCtls.size(); ++k)
	{
		pDC->Ellipse(splineCtls[k].x - 3, splineCtls[k].y - 3, splineCtls[k].x + 3, splineCtls[k].y + 3);
	}
	pDC->MoveTo(splineCtls[0]);
	for (int k = 1; k < splineCtls.size(); ++k)
		pDC->LineTo(splineCtls[k]);

	pDC->SelectObject(oldPen);

	if (splineCtls.size() < 4)
		return;

	vector<vector<float> > mat(4);
	vector<float> mat0(4);
	mat0[0] = -1; mat0[1] = 3; mat0[2] = -3; mat0[3] = 1;
	mat[0] = mat0;

	vector<float> mat1(4);
	mat1[0] = 3; mat1[1] = -6; mat1[2] = 3; mat1[3] = 0;
	mat[1] = mat1;

	vector<float> mat2(4);
	mat2[0] = -3; mat2[1] = 0; mat2[2] = 3; mat2[3] = 0;
	mat[2] = mat2;

	vector<float> mat3(4);
	mat3[0] = 1; mat3[1] = 4; mat3[2] = 1; mat3[3] = 0;
	mat[3] = mat3;
	for (int k = 0; k < 4; ++k)
		for (int m = 0; m < 4; ++m)
			mat[k][m] /= 6.;

	CPen penCurve(PS_SOLID, 2, RGB(128, 10, 10));
	oldPen = pDC->SelectObject(&penCurve);
	// draw curve
	
	// to get more beautiful effect, use other method for computing the points on the curve !!
	for (int kc = 0; kc < splineCtls.size() - 4 + 1; ++kc)
	{
		const int numSeg = 100;
		vector<float> uvec(4);
		float u = 0;
		vector<float> umat(4);
		CPoint curPt;

		vector<CPoint> curCtls(4);
		for (int k = 0; k < 4; ++k)
			curCtls[k] = splineCtls[kc + k];

		for (int k = 0; k <= numSeg; ++k)
		{
			u = 1. / numSeg * k;
			uvec[0] = u * u * u;
			uvec[1] = u * u;
			uvec[2] = u;
			uvec[3] = 1.;

			vectorMatrixProduct(uvec, mat, umat);

			curPt.x = 0;
			curPt.y = 0;

			for (int m = 0; m < 4; ++m)
			{
				curPt.x += curCtls[m].x * umat[m];
				curPt.y += curCtls[m].y * umat[m];
			}

			if (k == 0)
				pDC->MoveTo(curPt);
			else
				pDC->LineTo(curPt);
		}
	}



	pDC->SelectObject(oldPen);
}

void DrawBezierSurface(const vector<vector<CPoint> >& bezierSurfaceCtls, CDC* pDC)
{

}

void CDemo5View::OnDraw(CDC* pDC)
{
	CDemo5Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	switch (drawMode)
	{
	case Hermite:
		DrawHermite(hermiteCtls,pDC);
		break;
	case Bezier:
		DrawBezierCurve(bezierCtls, pDC);
		break;
	case BSpline:
		DrawBSpline(splineCtls, pDC);
		break;
	case BezierSurface:
		DrawBezierSurface(bezierSurfaceCtls, pDC);
		break;
	default:

		break;

	}

	// TODO: 在此处为本机数据添加绘制代码
}




//void CDemo5View::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CView::OnLButtonUp(nFlags, point);
//}
