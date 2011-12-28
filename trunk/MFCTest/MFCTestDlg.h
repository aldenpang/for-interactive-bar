// MFCTestDlg.h : 头文件
//

#pragma once
#include "atlimage.h"
#include "shockwaveflash1.h"
#include "cv.h"
#include "highgui.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "BMPServer.h"

struct MyRect 
{
	int x;
	int y;
	MyRect()
	{
		x = 0;
		y = 0;
	}
};


// CMFCTestDlg 对话框
class CMFCTestDlg : public CDialog
{
// 构造
public:
	CMFCTestDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CMFCTestDlg();

// 对话框数据
	enum { IDD = IDD_MFCTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void initCamera();
	cv::VideoCapture mCapture;

	void drawDetectorOnFlash(int _x, int _y);
	void cleanDetectorOnFlash();

	void drawOnHDC(int _resourceID, IplImage* _image);

	void drawBMPOnFlash( const IplImage* _image );
	//static void on_mouseWrapper(int event, int x, int y, int flags, void* param);
	//void on_mouse(int event, int x, int y, int flags);

	void setFlashFullScreen(bool _enable);

	void correctScreen(IplImage* _image);

		/** 从IPLIMAGE对象中生成BMP的图像文件流
	*/
	bool GetBmpFromIpl(const IplImage* pIpl, BYTE* &pBmp, DWORD& size);

	void runFlashExe();

	bool checkET99();

	void setFPS(int _fps);

	bool		mIsDrag;

	bool		mIsHitLT;
	bool		mIsHitRT;
	bool		mIsHitLB;
	bool		mIsHitRB;

	int			mCubeWidth;
	CRect		mLTCubeRect;
	CRect		mRTCubeRect;
	CRect		mLBCubeRect;
	CRect		mRBCubeRect;

	bool		mFinishCorrection;

	bool		mCaptureBK;

	CvPoint2D32f src_point[4];
	CvPoint2D32f dec_point[4];

	POINT		mPolygon[4];

	BMPServer*	mBMPServer;

	bool PointInFences(POINT pnt1, POINT* fencePnts, int count)
	{
		int j=0, cnt = 0;
		for (int i = 0; i < count; i++)
		{
			j = (i == count - 1) ? 0 : j + 1;
			if ((fencePnts[i].y!=fencePnts[j].y)&&(((pnt1.y >= fencePnts[i].y) && (pnt1.y < fencePnts[j].y)) ||
				((pnt1.y >= fencePnts[j].y) && (pnt1.y < fencePnts[i].y))) && 
				(pnt1.x < (fencePnts[j].x - fencePnts[i].x) * (pnt1.y - fencePnts[i].y) / (fencePnts[j].y - fencePnts[i].y) + fencePnts[i].x)) 
				cnt++;
		}
		return (cnt%2>0)?true:false;
	}

	void OnTimer_Detect();
	void OnTimer_DrawPNG();



public:
	CShockwaveflash1 mFlashCtrl;
	//afx_msg void OnBnClickedButton1();
	//afx_msg void OnBnClickedButton4();
	//afx_msg void OnBnClickedButton2();
	//afx_msg void OnBnClickedButton3();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeMsgBox();

	void AddLog(CString _msg);
	DECLARE_EVENTSINK_MAP()
	void FSCommandShockwaveflash1(LPCTSTR command, LPCTSTR args);
	CSliderCtrl mScollerThreshold1;
	CSliderCtrl mScollerThreshold2;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CEdit mTxtThreshold1;
	CEdit mTxtThreshold2;
	CEdit mTxtDensity;
	CEdit mTxtFPS;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton3();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL PreTranslateMessage(MSG *pMsg);
	afx_msg void OnEnChangeEdit4();
};
