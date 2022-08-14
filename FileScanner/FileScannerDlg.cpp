
// FileScannerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "FileScanner.h"
#include "FileScannerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileScannerDlg dialog



CFileScannerDlg::CFileScannerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FILESCANNER_DIALOG, pParent)
	, FolderPath(_T(""))
	, Name2Search(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileScannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FOLDER, FolderPath);
	DDX_Text(pDX, IDC_EDIT_FILENAME, Name2Search);
	DDX_Control(pDX, IDC_LIST1, FileList);
}

BEGIN_MESSAGE_MAP(CFileScannerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CFileScannerDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CFileScannerDlg::OnBnClickedButtonSearch)
END_MESSAGE_MAP()


// CFileScannerDlg message handlers

BOOL CFileScannerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	FileList.InsertColumn(0, _T("Ten file"), LVCFMT_LEFT, 250);
	FileList.InsertColumn(1, _T("Kich thuoc (bytes)"), LVCFMT_LEFT, 150);
	FileList.InsertColumn(2, _T("Duong dan"), LVCFMT_LEFT, 300);
	FileList.InsertColumn(3, _T("Ngay sua"), LVCFMT_LEFT, 205);
	FileList.InsertColumn(4, _T("Ngay tao"), LVCFMT_LEFT, 205);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileScannerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileScannerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileScannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFileScannerDlg::OnBnClickedButtonBrowse()
{
	CFolderPickerDialog cFolder;
	if (cFolder.DoModal())
	{
		FolderPath = cFolder.GetFolderPath();
	}
	else
	{
		MessageBox(_T("Unknown error has occured"), _T("Error"), MB_OK);
	}
	UpdateData(FALSE);
}

void CFileScannerDlg::OnBnClickedButtonSearch()
{
	UpdateData();
	if (FolderPath == _T("") || Name2Search == _T(""))
		return;

	// check if folder path is legit
	if (!PathFileExists(FolderPath))
	{
		MessageBox(_T("Folder path does not exist"), _T("Error"), MB_OK);
		return;
	}

	// Reset Content cho CListCtr
	FileList.DeleteAllItems();

	LockControl(IDC_EDIT_FILENAME);
	LockControl(IDC_EDIT_FOLDER);
	LockControl(IDC_BUTTON_BROWSE);
	LockControl(IDC_BUTTON_SEARCH);
	
	ScanFile(FolderPath);

	UnlockControl(IDC_EDIT_FILENAME);
	UnlockControl(IDC_EDIT_FOLDER);
	UnlockControl(IDC_BUTTON_BROWSE);
	UnlockControl(IDC_BUTTON_SEARCH);
}

void CFileScannerDlg::ScanFile(CString Path)
{
	CString Search = Path + _T("\\*.*");
	CString FilePath = _T("");
	CFileFind Finder;
	BOOL bWorking = Finder.FindFile(Search);
	if (!bWorking)
	{
		MessageBox(_T("Unknown error has occured"), _T("Error"), MB_OK);
		return;
	}
	while (bWorking)
	{
		bWorking = Finder.FindNextFileW();
		FilePath = Finder.GetFilePath();
		if (Finder.IsDots())
		{
			continue;
		}
		if (Finder.IsDirectory())
		{
			ScanFile(FilePath);
		}
		else
		{
			CString FileTitle = Finder.GetFileTitle();
			if (FileTitle.Find(Name2Search) != -1)
			{
				CString FileName = Finder.GetFileName();
				ULONGLONG FileSize = Finder.GetLength();
				CString FileSizeString;
				FileSizeString.Format(_T("%I64u"), FileSize);
				CTime FileCreateTime;
				CTime FileModifyTime;
				Finder.GetCreationTime(FileCreateTime);
				Finder.GetLastWriteTime(FileModifyTime);
				CString FileCreateTimeString = FileCreateTime.Format(_T("%A, %B %d, %Y, %r"));
				CString FileModifyTimeString = FileModifyTime.Format(_T("%A, %B %d, %Y, %r"));
				int nItem;

				nItem = FileList.InsertItem(0, FileName);
				FileList.SetItemText(nItem, 1, FileSizeString);
				FileList.SetItemText(nItem, 2, FilePath);
				FileList.SetItemText(nItem, 3, FileModifyTimeString);
				FileList.SetItemText(nItem, 4, FileCreateTimeString);

			}
		}
	}
	Finder.Close();
	return;
}

void CFileScannerDlg::LockControl(int ID)
{
	auto pLock = GetDlgItem(ID);
	pLock->EnableWindow(FALSE);
}

void CFileScannerDlg::UnlockControl(int ID)
{
	auto pUnlock = GetDlgItem(ID);
	pUnlock->EnableWindow();
}