// MFCTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCTest.h"
#include "MFCTestDlg.h"
#include "StringConvert.h"
#include <MMSystem.h>	/// for timeGetTime()
#include "PsVisualAlgorithm.h"
#include "DataCenter.h"
#include "FT_ET99_API.h"

ET_HANDLE g_hToken = NULL;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMFCTestDlg 对话框




CMFCTestDlg::CMFCTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFCTestDlg::IDD, pParent)
	, mIsDrag(false)
	, mIsHitLT(false)
	, mIsHitRT(false)
	, mIsHitLB(false)
	, mIsHitRB(false)
	, mFinishCorrection(false)
	, mCaptureBK(false)
	, mBMPServer(NULL)
{
	DATACENTER.LoadVideoSize();

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	mCubeWidth = 30;
	mLTCubeRect = CRect(0, 0, 0+mCubeWidth, 0+mCubeWidth);
	mLBCubeRect = CRect(0, DATACENTER.mVideoHeight-mCubeWidth, mCubeWidth, DATACENTER.mVideoHeight);
	mRTCubeRect = CRect(DATACENTER.mVideoWidth-mCubeWidth, 0, DATACENTER.mVideoWidth, mCubeWidth);
	mRBCubeRect = CRect(DATACENTER.mVideoWidth-mCubeWidth, DATACENTER.mVideoHeight-mCubeWidth, DATACENTER.mVideoWidth, DATACENTER.mVideoHeight);

	int fix = 0;
	src_point[0].x=fix;             
	src_point[0].y=fix;
	src_point[1].x=DATACENTER.mVideoWidth-fix;                    
	src_point[1].y=fix;
	src_point[2].x=fix; 
	src_point[2].y=DATACENTER.mVideoHeight-fix;
	src_point[3].x=DATACENTER.mVideoWidth-fix; 
	src_point[3].y=DATACENTER.mVideoHeight-fix;

	mPolygon[0].x = src_point[0].x;
	mPolygon[0].y = src_point[0].y;
	mPolygon[1].x = src_point[2].x;
	mPolygon[1].y = src_point[2].y;
	mPolygon[2].x = src_point[3].x;
	mPolygon[2].y = src_point[3].y;
	mPolygon[3].x = src_point[1].x;
	mPolygon[3].y = src_point[1].y;

}

void CMFCTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH1, mFlashCtrl);
	DDX_Control(pDX, IDC_SLIDER1, mScollerThreshold1);
	DDX_Control(pDX, IDC_SLIDER2, mScollerThreshold2);
	DDX_Control(pDX, IDC_EDIT1, mTxtThreshold1);
	DDX_Control(pDX, IDC_EDIT2, mTxtThreshold2);
	DDX_Control(pDX, IDC_EDIT3, mTxtDensity);
	DDX_Control(pDX, IDC_EDIT4, mTxtFPS);
}

BEGIN_MESSAGE_MAP(CMFCTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	//ON_EN_CHANGE(IDC_MSG_BOX, &CMFCTestDlg::OnEnChangeMsgBox)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCTestDlg::OnBnClickedButton1)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCTestDlg::OnBnClickedButton2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CMFCTestDlg::OnNMCustomdrawSlider2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCTestDlg::OnBnClickedButton3)
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_EN_CHANGE(IDC_EDIT4, &CMFCTestDlg::OnEnChangeEdit4)
END_MESSAGE_MAP()


// CMFCTestDlg 消息处理程序

BOOL CMFCTestDlg::OnInitDialog()
{
	if ( !checkET99() )
	{
		PostQuitMessage(0);
		return false;
	}

	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	DATACENTER.LoadConfig();
	
	// 启动时save一次，如果load的时候没有config文件，那就用默认值自动创建一个
	DATACENTER.SaveConfig();

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// init scroll bars
	mScollerThreshold1.SetRange(0, 200);
	mScollerThreshold1.SetPos(DATACENTER.mThreshold1);
	mScollerThreshold2.SetRange(0, 500);
	mScollerThreshold2.SetPos(DATACENTER.mThreshold2);

	// init cedit
	mTxtThreshold1.SetWindowTextW(StringConvert::IntToCString(DATACENTER.mThreshold1));
	mTxtThreshold2.SetWindowTextW(StringConvert::IntToCString(DATACENTER.mThreshold2));
	mTxtDensity.SetWindowTextW(StringConvert::IntToCString(DATACENTER.mPixelStep));

	char curPath[512];
	GetCurrentDirectoryA(512, curPath);
	strcat(curPath, "\\test.swf");
	for ( int i = 0; i<strlen(curPath); i++ )
	{
		if ( curPath[i] == 92 )	// check '\'
			curPath[i] = '/';
	}
	
	CString tempStr(curPath);
	//mFlashCtrl.LoadMovie(0, tempStr);
	//mFlashCtrl.Play();
	//mFlashCtrl.

	initCamera();

	SetTimer(0, 50, NULL);	// limit at 20FPS
	SetTimer(1, 10, NULL);	// draw png image

	AddLog(L"Initialized...");

	mBMPServer = new BMPServer;

	runFlashExe();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//void CMFCTestDlg::OnBnClickedButton1()		// Up
//{
//	// TODO: Add your control notification handler code here
//	CString y = mFlashCtrl.GetVariable(L"_root.btn._y");
//	int iY = StringConvert::CStringToInt(y);
//	
//	iY-=10;
//	CString fixedY = StringConvert::IntToCString(iY);
//	mFlashCtrl.SetVariable(L"_root.btn._y", fixedY);
//	return;
//}
//
//void CMFCTestDlg::OnBnClickedButton4()		// Right
//{
//	// TODO: Add your control notification handler code here
//	//mFlashCtrl.SetVariable(L"_root.a",L"3.6");
//	//CString res = mFlashCtrl.CallFunction(L"<invoke name=\"mfc\" returntype=\"xml\">\
//   //         <arguments><string>From mfc</string></arguments>\
//    //        </invoke>");
//
//	mFlashCtrl.CallFunction(L"<invoke name=\"drawDetector\">\
//            <arguments><number>100</number><number>200</number></arguments>\
//            </invoke>");
//
//
//	return;
//}
//
//void CMFCTestDlg::OnBnClickedButton2()		// Left
//{
//	// TODO: Add your control notification handler code here
//}
//
//void CMFCTestDlg::OnBnClickedButton3()		// Down
//{
//	// TODO: Add your control notification handler code here
//}
//#include "highgui.h"

//#define USE_STATIC_IMAGE
IplImage *rawinput2 = NULL;
void CMFCTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 0:
		OnTimer_Detect();
		break;
	case 1:
		OnTimer_DrawPNG();
		break;
	default:
		break;
	}
}

void CMFCTestDlg::drawOnHDC( int _resourceID, IplImage* _image )
{
	// 把OpenCV的画面draw到MFC上
	CRect rect;
	GetDlgItem(_resourceID)->GetClientRect(&rect);
	CvvImage cvvImage2;
	cvvImage2.CopyOf(_image);
	cvvImage2.DrawToHDC(GetDlgItem(_resourceID)->GetDC()->GetSafeHdc(), rect);

}

void CMFCTestDlg::drawDetectorOnFlash(int _x, int _y)
{
	char command[1024]={0};
	sprintf(command, "<invoke name=\"drawDetector\">\
            <arguments><number>%d</number><number>%d</number></arguments>\
            </invoke>", _x, _y);
	mFlashCtrl.CallFunction(CString(command));

}

void CMFCTestDlg::drawBMPOnFlash( const IplImage* _image )
{
	//char command[204800]={0};
	//int imgSizeByte = _image->imageSize;
	//sprintf(command, "<invoke name=\"ReceiveBitmap\"><arguments><string>%s</string></arguments></invoke>"
	//	, _image->imageData);
	//mFlashCtrl.CallFunction(CString(command));

	//IplImage* imageRGBA = cvCreateImage(cvGetSize(_image),IPL_DEPTH_8U, 4);
	//IplImage* imageRGB = cvCreateImage(cvGetSize(_image),IPL_DEPTH_8U, 3);
	//cvCvtColor(_image, imageRGB, CV_BGR2RGB);

	//char buf[256]={0};
	//sprintf_s(buf, 128, "BufferSize:[%d]", mBMPServer->GetBufferSize());
	//CString log(buf);
	//AddLog(log);

	//把Gray的图转换为BGR的图
	if ( strcmp(_image->channelSeq, "GRAY") == 0 )
	{
		unsigned char* bgr = new unsigned char[_image->imageSize*3];
		memset(bgr, 0, sizeof(bgr));

		int j = 0;
		for ( int i = 0; i<_image->imageSize*3; i++ )
		{
			if ( i!=0 && i%3 == 0 )
				j++;

			unsigned char dd =  _image->imageData[j];
			bgr[i] = _image->imageData[j];
			//bgr[i] = 128;
			
			continue;
		}
		
		mBMPServer->SendData((char*)bgr, _image->imageSize*3);

		delete []bgr;
		bgr = NULL;
	}
	else
	{
		mBMPServer->SendData(_image->imageData, _image->imageSize);
	}

	

	//cvSaveImage("bmpoutput.bmp", _image);

	//BYTE *bmp = NULL;
	//DWORD size;
	//GetBmpFromIpl(_image, bmp, size);
	
	//FILE* fp = fopen("tttt.bmp", "wb");
	//if ( fp )
	//{
	//	fwrite(bmp, size, 1, fp);
	//}
	//fclose(fp);
	//mBMPServer->SendData(bmp, size);

	//delete bmp;
	//bmp = NULL;
}

/** 从IPLIMAGE对象中生成BMP的图像文件流
*/
bool CMFCTestDlg::GetBmpFromIpl(const IplImage* pIpl, BYTE* &pBmp, DWORD& size)
{
	BITMAPFILEHEADER bfh = {0};
	DWORD dwImageSize = 0;
	DWORD dwBytesRead = 0;

	int w = pIpl->width;
	int l = ((w * 24 +31) & ~31) /8;
	int h = pIpl->height;
	dwImageSize    = l * h;

	bfh.bfType        = (WORD)'M' << 8 | 'B';            //定义文件类
	bfh.bfOffBits    = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);    //定义文件头大小
	bfh.bfSize        = bfh.bfOffBits + dwImageSize;        //文件大小

	BITMAPINFOHEADER  bih = {0};
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = pIpl->width;
	bih.biHeight = pIpl->height;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage    = 0;
	bih.biXPelsPerMeter    = 0;
	bih.biYPelsPerMeter    = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	size = bfh.bfSize;
	pBmp = new BYTE[bfh.bfSize+1];

	memset(pBmp, 0, bfh.bfSize + 1);
	memcpy(pBmp, &bfh, sizeof(BITMAPFILEHEADER));
	memcpy(pBmp+sizeof(BITMAPFILEHEADER), &bih, sizeof(BITMAPINFOHEADER));
	BYTE* p = pBmp+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	memcpy(p, (BYTE*)pIpl->imageData, dwImageSize);

	//BYTE* dd = new BYTE[bfh.bfSize+1];
	//memset(dd, 0, bfh.bfSize + 1);
	//int ss = sizeof(pBmp);
	//memcpy(dd, pBmp, size);
	//BYTE* pp = dd+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

	return true;
}

void CMFCTestDlg::setFlashFullScreen( bool _enable )
{
	char command[1024]={0};
	sprintf(command, "<invoke name=\"SetFullScreen\">\
					 <arguments><number>%d</number></arguments>\
					 </invoke>", _enable);
	mFlashCtrl.CallFunction(CString(command));

}

void CMFCTestDlg::cleanDetectorOnFlash()
{
	char command[1024]={0};
	sprintf(command, "<invoke name=\"cleanDetector\"></invoke>");
	mFlashCtrl.CallFunction(CString(command));
}


void CMFCTestDlg::initCamera()
{
	mCapture = cv::VideoCapture(0);
	if(!mCapture.isOpened()) // check if we succeeded
		return;

	return;
}
void CMFCTestDlg::OnEnChangeMsgBox()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CMFCTestDlg::AddLog( CString _msg )
{
	CListBox* listbox = ((CListBox*)GetDlgItem(IDC_LIST1));
	listbox->AddString(_msg);
	int count = listbox->GetCount();
	listbox->SetCurSel(count-1);
}BEGIN_EVENTSINK_MAP(CMFCTestDlg, CDialog)
ON_EVENT(CMFCTestDlg, IDC_SHOCKWAVEFLASH1, 150, CMFCTestDlg::FSCommandShockwaveflash1, VTS_BSTR VTS_BSTR)
END_EVENTSINK_MAP()

void CMFCTestDlg::FSCommandShockwaveflash1(LPCTSTR command, LPCTSTR args)
{
	// 从flash接收消息

}

void CMFCTestDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if( (CWnd*)pScrollBar == (CWnd*)(&mScollerThreshold1) )
	{
		int pos = mScollerThreshold1.GetPos();
		DATACENTER.mThreshold1 = pos;
	}
	else if( (CWnd*)pScrollBar == (CWnd*)(&mScollerThreshold2 ))
	{
		int pos = mScollerThreshold2.GetPos();
		DATACENTER.mThreshold2 = pos;
	}
	else
	{
		int i = 0;
	}

	mTxtThreshold1.SetWindowTextW(StringConvert::IntToCString(DATACENTER.mThreshold1));
	mTxtThreshold1.Invalidate();
	mTxtThreshold2.SetWindowTextW(StringConvert::IntToCString(DATACENTER.mThreshold2));
	mTxtThreshold2.Invalidate();


	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMFCTestDlg::OnBnClickedButton1()
{
	// set density of detector for flash
	CString value;
	mTxtDensity.GetWindowTextW(value);
	int valueInt = StringConvert::CStringToInt(value);

	DATACENTER.mPixelStep = valueInt;

}

//void CMFCTestDlg::on_mouse(int event, int x, int y, int flags, void* param)
//{
//	((CMFCTestDlg*)param)->on_mouse(event, x, y, flags);
//}
//
//void CMFCTestDlg::on_mouse(int event, int x, int y, int flags)
//{
//
//
//	return;
//}
void CMFCTestDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	static CPoint lastPos = CPoint(0,0);
	CPoint offsetPos = point-lastPos;

	if(mIsDrag)
	{
		CRect r, oriRegion;
		GetDlgItem(IDC_CAMERA_AREA2)->GetWindowRect(&r);
		ScreenToClient(&r);
		GetDlgItem(IDC_CAMERA_AREA2)->GetClientRect(&oriRegion);

		//POINT p;
		//p.x = point.x-r.left;
		//p.y = point.y-r.top;
		//char buf[1024];
		//sprintf(buf, "MouseX[%d] MouseY[%d]", p.x, p.y);
		//AddLog(CString(buf));
		//if (PointInFences(p, mPolygon, 4))
		//{
		//	AddLog(CString("In Rectangle"));
		//}

		if( point.x >= r.left && point.x <= r.right		// 判断鼠标是否在视频区域内
			&& point.y >= r.top && point.y <= r.bottom )
		{
			float rateW = (float)r.Width()/(float)DATACENTER.mVideoWidth;
			float rateH = (float)r.Height()/(float)DATACENTER.mVideoHeight;
			if( (point.x-r.left) >= mLTCubeRect.left && (point.x-r.left) <= mLTCubeRect.right		// 判断鼠标是否在左上矩形区域内
				&& (point.y-r.top) >= mLTCubeRect.top && (point.y-r.top) <= mLTCubeRect.bottom )
			{
				mIsHitLT = true;
			}
			else if ( (point.x-r.left) >= mRTCubeRect.left && (point.x-r.left) <= mRTCubeRect.right		// 判断鼠标是否在右上矩形区域内
				&& (point.y-r.top) >= mRTCubeRect.top && (point.y-r.top) <= mRTCubeRect.bottom )
			{
				mIsHitRT = true;
			}
			else if ( (point.x-r.left) >= mLBCubeRect.left && (point.x-r.left) <= mLBCubeRect.right		// 判断鼠标是否在左下矩形区域内
				&& (point.y-r.top) >= mLBCubeRect.top && (point.y-r.top) <= mLBCubeRect.bottom )
			{
				mIsHitLB = true;
			}
			else if ( (point.x-r.left) >= mRBCubeRect.left && (point.x-r.left) <= mRBCubeRect.right		// 判断鼠标是否在右下矩形区域内
				&& (point.y-r.top) >= mRBCubeRect.top && (point.y-r.top) <= mRBCubeRect.bottom )
			{
				mIsHitRB = true;
			}


			if ( mIsHitLT )
			{
				mLTCubeRect.left += (int)((float)offsetPos.x*rateW);
				mLTCubeRect.top += (int)((float)offsetPos.y*rateH);
				mLTCubeRect.right += (int)((float)offsetPos.x*rateW);
				mLTCubeRect.bottom += (int)((float)offsetPos.y*rateH);
			}
			else if ( mIsHitRT )
			{
				mRTCubeRect.left += (int)((float)offsetPos.x*rateW);
				mRTCubeRect.top += (int)((float)offsetPos.y*rateH);
				mRTCubeRect.right += (int)((float)offsetPos.x*rateW);
				mRTCubeRect.bottom += (int)((float)offsetPos.y*rateH);
			}
			else if ( mIsHitLB )
			{
				mLBCubeRect.left += (int)((float)offsetPos.x*rateW);
				mLBCubeRect.top += (int)((float)offsetPos.y*rateH);
				mLBCubeRect.right += (int)((float)offsetPos.x*rateW);
				mLBCubeRect.bottom += (int)((float)offsetPos.y*rateH);
			}
			else if ( mIsHitRB )
			{
				mRBCubeRect.left += (int)((float)offsetPos.x*rateW);
				mRBCubeRect.top += (int)((float)offsetPos.y*rateH);
				mRBCubeRect.right += (int)((float)offsetPos.x*rateW);
				mRBCubeRect.bottom += (int)((float)offsetPos.y*rateH);
			}


			//char buf[1024];

			//sprintf(buf, "HitLT[%d]HitRT[%d]HitLB[%d]HitRB[%d]",mIsHitLT, mIsHitRT, mIsHitLB, mIsHitRB);
			//CString log(buf);
			//AddLog(log);
		}	
	}
	lastPos = point;
	CDialog::OnMouseMove(nFlags, point);
}

void CMFCTestDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	mIsDrag = true;
	CDialog::OnLButtonDown(nFlags, point);
}

void CMFCTestDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	mFinishCorrection = !mFinishCorrection;

	if ( mFinishCorrection )
	{
		src_point[0].x=mLTCubeRect.left;             
		src_point[0].y=mLTCubeRect.top;
		src_point[1].x=mRTCubeRect.right;                    
		src_point[1].y=mRTCubeRect.top;
		src_point[2].x=mLBCubeRect.left; 
		src_point[2].y=mLBCubeRect.bottom;
		src_point[3].x=mRBCubeRect.right; 
		src_point[3].y=mRBCubeRect.bottom;
	}
	else
	{
		int fix = 0;
		src_point[0].x=fix;             
		src_point[0].y=fix;
		src_point[1].x=DATACENTER.mVideoWidth-fix;                    
		src_point[1].y=fix;
		src_point[2].x=fix; 
		src_point[2].y=DATACENTER.mVideoHeight-fix;
		src_point[3].x=DATACENTER.mVideoWidth-fix; 
		src_point[3].y=DATACENTER.mVideoHeight-fix;
	}

	// TODO: Add your message handler code here and/or call default
	//int fix = 150;		// 测试代码，自动往中心缩进100像素
	//src_point[0].x=fix;             
	//src_point[0].y=fix;
	//src_point[1].x=DATACENTER.mVideoWidth-fix;                    
	//src_point[1].y=fix;
	//src_point[2].x=fix; 
	//src_point[2].y=DATACENTER.mVideoHeight-fix;
	//src_point[3].x=DATACENTER.mVideoWidth-fix; 
	//src_point[3].y=DATACENTER.mVideoHeight-fix;

	// mPolygon=  0-左上 1-左下  2-右下  3-右上
	mPolygon[0].x = src_point[0].x;
	mPolygon[0].y = src_point[0].y;
	mPolygon[1].x = src_point[2].x;
	mPolygon[1].y = src_point[2].y;
	mPolygon[2].x = src_point[3].x;
	mPolygon[2].y = src_point[3].y;
	mPolygon[3].x = src_point[1].x;
	mPolygon[3].y = src_point[1].y;


	CDialog::OnLButtonDblClk(nFlags, point);
}

void CMFCTestDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	mIsDrag = false;
	mIsHitLT = false;
	mIsHitRT = false;
	mIsHitLB = false;
	mIsHitRB = false;
	CDialog::OnLButtonUp(nFlags, point);
}

void CMFCTestDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	DATACENTER.SaveConfig();

	CDialog::OnClose();
}

void CMFCTestDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}

void CMFCTestDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	DATACENTER.SaveConfig();

	MessageBoxA(this->m_hWnd, "保存设置成功！", "保存设置", MB_OK);
}

void CMFCTestDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CMFCTestDlg::OnBnClickedButton3()
{
	mCaptureBK = true;
}

CMFCTestDlg::~CMFCTestDlg()
{
	if ( mBMPServer )
	{
		delete mBMPServer;
		mBMPServer = NULL;
	}
}

void CMFCTestDlg::correctScreen( IplImage* _image )
{

	// 如果用户双击屏幕，那么就是用四个矩形标定的外侧坐标作为定位，将四边形做透视变换
	if ( mFinishCorrection )
	{
		IplImage* Img_old = cvCloneImage( _image );
		//IplImage* Img_old2 = cvCloneImage( _image );

		CvSize size = cvGetSize(_image);
		dec_point[0].x = 0;
		dec_point[0].y = 0;
		dec_point[1].x = size.width;
		dec_point[1].y = 0;
		dec_point[2].x = 0;
		dec_point[2].y = size.height;
		dec_point[3].x = size.width; 
		dec_point[3].y = size.height;

		float newm[9];
		CvMat newM = cvMat( 3, 3, CV_32F, newm );

		//获得透视转换矩阵
		cvGetPerspectiveTransform(src_point,dec_point,&newM);
		cvWarpPerspective(Img_old,_image,&newM,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );
		//cvWarpPerspective(Img_old2,input,&newM,CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );

		cvReleaseImage(&Img_old);
		//cvReleaseImage(&Img_old2);
	}
	else
	{
		cvRectangle(_image, cvPoint(mLTCubeRect.left, mLTCubeRect.top), cvPoint(mLTCubeRect.right, mLTCubeRect.bottom), 
			cvScalar(0xff,0x00,0x00), CV_FILLED);
		cvRectangle(_image, cvPoint(mRTCubeRect.left, mRTCubeRect.top), cvPoint(mRTCubeRect.right, mRTCubeRect.bottom), 
			cvScalar(0xff,0x00,0x00), CV_FILLED);
		cvRectangle(_image, cvPoint(mLBCubeRect.left, mLBCubeRect.top), cvPoint(mLBCubeRect.right, mLBCubeRect.bottom), 
			cvScalar(0xff,0x00,0x00), CV_FILLED);
		cvRectangle(_image, cvPoint(mRBCubeRect.left, mRBCubeRect.top), cvPoint(mRBCubeRect.right, mRBCubeRect.bottom), 
			cvScalar(0xff,0x00,0x00), CV_FILLED);
	}
}

void CMFCTestDlg::runFlashExe()
{
	STARTUPINFO si; //一些必备参数设置
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pi; //必备参数设置结束
	if(!CreateProcess(L"test.exe",NULL,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
	{
		MessageBoxA(0, "Can not find flash exe file!", "Error!", MB_OK);
	}

	return;
}
void CMFCTestDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CMFCTestDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL CMFCTestDlg::PreTranslateMessage( MSG *pMsg )
{
	if ( WM_KEYFIRST<= pMsg->message && pMsg->message <= WM_KEYLAST )
	{
		if ( pMsg->wParam == VK_F1 )
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

#define PID "7B748C33"
#define SOPIN "9F0E70F60E2CC8EA"
bool CMFCTestDlg::checkET99()
{
	char iBuf[80] = {0};
	sprintf_s(iBuf, PID);
	int count = 0;
	int res = et_FindToken((unsigned char*)iBuf, &count);
	if ( count == 0 )
	{
		MessageBoxA(0, "未找到加密硬件", "Warning!", MB_OK);
		return false;
	}

	res = et_OpenToken(&g_hToken, (unsigned char*)iBuf, 1);
	if ( res != ET_SUCCESS )
	{
		char errBuff[64]={0};
		sprintf_s(errBuff, "打开加密硬件发生错误，错误代码[%d]", res);
		MessageBoxA(0, errBuff, "错误", MB_OK);
		return false;
	}

	res = et_Verify(g_hToken, ET_VERIFY_SOPIN, (unsigned char*)SOPIN);
	if ( res != ET_SUCCESS )
	{
		char errBuff[64]={0};
		sprintf_s(errBuff, "认证加密硬件发生错误，错误代码[%d]", res);
		MessageBoxA(0, errBuff, "错误", MB_OK);
		return false;
	}

	res = et_CloseToken(g_hToken);

	return true;
}
void CMFCTestDlg::OnEnChangeEdit4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CMFCTestDlg::setFPS(int _fps)
{
	CString fps = StringConvert::IntToCString(_fps);
	mTxtFPS.SetWindowText(fps);
}

// 主绘制流程
void CMFCTestDlg::OnTimer_Detect()
{
	// TODO: Add your message handler code here and/or call default
	int beginRenderTime = timeGetTime();
	CPaintDC dc(this); // 用于绘制的设备上下文

	cv::Mat frame;
	mCapture >> frame; // get a new frame from camera

	int stepTime1 = timeGetTime();

#ifdef USE_STATIC_IMAGE
	IplImage *rawinput = cvLoadImage( "real.jpg", 1);
	IplImage *rawinput2 = cvLoadImage( "realbk.jpg", 1);
#else
	IplImage *rawinput = &IplImage(frame);
	//IplImage *rawinput2 = NULL;// = &IplImage(frame);	// initially, they are same.
#endif
	if ( mCaptureBK )		// 判断是否已经确定静态背景，如果确定，则使用静态背景作为参照物
	{
		rawinput2 = cvCloneImage(rawinput);
		mCaptureBK = false;
	}

	CvSize dst_cvsize;
	// resize
	dst_cvsize.width = DATACENTER.mVideoWidth;
	dst_cvsize.height = DATACENTER.mVideoHeight;
	IplImage* input = cvCreateImage(dst_cvsize,rawinput->depth, rawinput->nChannels);
	cvResize(rawinput, input, CV_INTER_LINEAR);
	IplImage* input2 = cvCreateImage(dst_cvsize,rawinput->depth, rawinput->nChannels);
	if ( rawinput2 != NULL )
		cvResize(rawinput2, input2, CV_INTER_LINEAR);

	IplImage* rawinputGRAY = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U, 1);
	IplImage* rawinput2GRAY = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U, 1);
	IplImage* result = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U, 1);
	IplImage* result2 = NULL;

	cvCvtColor(input, rawinputGRAY, CV_BGR2GRAY);
	if ( rawinput2 != NULL )
		cvCvtColor(input2, rawinput2GRAY, CV_BGR2GRAY);

	if ( rawinput2 != NULL )
	{
		result2 = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U, 1);
		cvAbsDiff(rawinput2GRAY, rawinputGRAY, result);		// 比较图像
		cvThreshold(result, result2, DATACENTER.mThreshold1, 255.0, CV_THRESH_BINARY); //阀值处理
	}
	else
	{
		result2 = cvCloneImage(input);
	}

	// 对结果图像进行梯形校正
	correctScreen(result2);

	int timer1 = timeGetTime();
	drawBMPOnFlash(result2);	//将结果发送给flash
	int timer2 = timeGetTime();

	// result camera window
	drawOnHDC(IDC_CAMERA_AREA, rawinputGRAY);
	drawOnHDC(IDC_CAMERA_AREA2, result2);

	::ReleaseDC(this->m_hWnd, dc.GetSafeHdc());
	cvReleaseImage(&input);
	cvReleaseImage(&input2);
	cvReleaseImage(&result);
	cvReleaseImage(&result2);
	cvReleaseImage(&rawinputGRAY);
	cvReleaseImage(&rawinput2GRAY);


	//	cvReleaseImage(&rawinput);		// need release if load static image

	int endRenderTime = timeGetTime();
	////////////////////////////  LOG  ///////////////////////////////////
	//char buf[256]={0};
	//sprintf_s(buf, 128, "CaptureCam:%dms, Canny:%dms, DetectPixel:%dms, DrawHDC:%dms, Total:%dms", 
	//	stepTime1-beginRenderTime, stepTime4-stepTime3, stepTime5-stepTime4, stepTime6-stepTime5, 
	//	endRenderTime-beginRenderTime);

	//sprintf_s(buf, 128, "每帧刷新时间:%d毫秒, 每秒帧数:%2f", endRenderTime-beginRenderTime, 1000.0/(endRenderTime-beginRenderTime));
	//CString log(buf);
	//AddLog(log);

	setFPS(1000.0/(endRenderTime-beginRenderTime));
}

void CMFCTestDlg::OnTimer_DrawPNG()
{
	ATL::CImage image;
	HRESULT hr = image.Load(_T("logo.png"));
	if (SUCCEEDED(hr))
	{
		for(int i = 0; i < image.GetWidth(); i++)
		{
			for(int j = 0; j < image.GetHeight(); j++)
			{
				unsigned char* pucColor = reinterpret_cast<unsigned char *>(image.GetPixelAddress(i , j));
				pucColor[0] = pucColor[0] * pucColor[3] / 255;
				pucColor[1] = pucColor[1] * pucColor[3] / 255;
				pucColor[2] = pucColor[2] * pucColor[3] / 255;
			}
		}
		image.Draw(GetDlgItem(IDC_LOGO)->GetDC()->GetSafeHdc(), 0, 0);
	}
}