/****************************************************************************
 Modul:   		NEUROROB.C

 Aufgabe:    	Hauptteil des Programmes. Dieses Modul ｜ernimmt die Anmeldung
							von NeuroRob bei WINDOWS. Sowie die Steuerung des Hauptfensters
							und des Hauptmenues.

 Funktionen:  WinMain
							InitApplication
							InitInstance
							MainWndProc
****************************************************************************/

#include <windows.h>
#include "neurorob.h"
#include "brainmen.h"
#include "neuromen.h"
#include <stdio.h>

DCB		 		CommDCB;

HANDLE 		nCid;
HANDLE		nCid2;
HANDLE		hInst;

char			dummy[100];
char			dummy1[100];
char			dummy2[100];

HMENU		  hMainMenu;
HMENU			hBrainMenu;

BOOL      bMoveMasterInit			= FALSE;
BOOL			bVideomatInit				= FALSE;
BOOL			bBrain							= FALSE;
BOOL			bBefehlsliste 			= FALSE;
BOOL			bSteuerung 					= FALSE;
BOOL			bVideoSteuerung			= FALSE;
BOOL			bRepeatmodus 				= FALSE;
BOOL			bEinzelschrittmodus	= FALSE;
BOOL			bVisualisierung			= FALSE;
BOOL			bNetzdefine					= FALSE;
BOOL			bSpeichernAbfrage		= TRUE;
BOOL					bSpeichern			= FALSE;  //MenueItem Speichern erlaubt ?

HWND			hWndBrain;
HWND			hWndNeurorob;
HWND			hDlgBefehlsliste;
HWND			hDlgSteuerung;
HWND			hDlgVisualisierung;
HWND			hDlgVideoSteuerung;

FARPROC		lpBefehlsliste;
FARPROC		lpSteuerung;
FARPROC		lpProcLaden;
FARPROC		lpVisualisierung;
FARPROC   lpVideoSteuerung;
LPSTR			szFileCaption;

HBITMAP		hRobBitmap;
HBITMAP		hOldBitmap;
HBITMAP		hAllBitmap;
HBITMAP   hFaceBitmap;
HBITMAP	  hGehirnBitmap;

HPEN 			hRotPen1;
HPEN  		hBlauPen1;
HPEN			hLilaPen1;
HPEN			hSchwarzPen1;
HPEN			hGelbPen1;
HPEN			hHellblauPen1;
HPEN			hGruenPen1;
HPEN			hGrauPen1;

HBRUSH 		hRotBrush1;
HBRUSH 		hBlauBrush1;
HBRUSH 		hLilaBrush1;
HBRUSH 		hGruenBrush1;
HBRUSH 		hSchwarzBrush1;
HBRUSH 		hGelbBrush1;
HBRUSH 		hHellblauBrush1;
HBRUSH 		hGrauBrush1;

HCURSOR		hHourGlass;
HCURSOR		hMaster;
HCURSOR		hSivips;


int				roboterspeed = 20;
int				roboterstep	 = 20;
int				robotergrid  = 63;
int 			roboterdegree= 10;
int 			ndummy;

double		xCent=394;
double		yCent=25;
double		xUrsprung;
double		yUrsprung;
double		xPart;
double		yPart;

char 			FileName[128];
char 			PathName[128];
char 			OpenName[128];
char 			DefSpec[13]="*.*";
char 			DefExt[]=".txt";
char 			str[255];

/****************************************************************************
 Funktion:     WinMain

 Aufgabe:      Hauptfunktion

 Parameter:
	HANDLE hInstance  	:Aktuelle Instance
	HANDLE hPrevInstance:Vorherige Instance
	LPSTR  lpCmdLine    :Zeiger auf String der bei Aufruf hinter dem Programm-
											 namen stand
	int		 nCmdShpw     :

 R…kgabewert:
	int					 Der wParam der zuletzt empfangenen Meldung
****************************************************************************/

int PASCAL WinMain(	HANDLE 	hInstance,
										HANDLE	hPrevInstance,
										LPSTR		lpCmdLine,
										int			nCmdShow)
{
MSG 		msg;
HCURSOR	hSaveCursor;

	if (!hPrevInstance)
	{
		if (!InitApplication(hInstance))
			return (FALSE);
	}
	else
	{
		MessageBox(GetFocus(),(LPSTR)"NeuroRob ist bereits gstartet worden",NULL,
			MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		return FALSE;
	}
	if (!InitInstance(hInstance, nCmdShow))
		return (FALSE);
	while (GetMessage(&msg,NULL,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (msg.wParam);
}//WinMain

/****************************************************************************
 Funktion:     InitApplication

 Aufgabe:      Grundinitialisierung

 Parameter:    HANDLE hInstance : Handle der zu initialisierenden Instance

 R…kgabewert: BOOL 		wenn TRUE dann war Initialisierung erfolgreich
****************************************************************************/

BOOL InitApplication(HANDLE hInstance)
{
WNDCLASS  wc;
WNDCLASS  wc2;

	wc.style 					= NULL;
	wc.lpfnWndProc		= MainWndProc;
	wc.cbClsExtra 		= 0;
	wc.cbWndExtra 		= 0;
	wc.hInstance 			= hInstance;
	wc.hIcon 					= LoadIcon(hInstance, "ROB");
	wc.hCursor 				= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground 	= GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  	= "NeuroRobMenu";
	wc.lpszClassName 	= "NeurorobWClass";
	if(RegisterClass(&wc)==FALSE)
		return(FALSE);
	wc2.style = CS_VREDRAW | CS_HREDRAW | CS_CLASSDC | CS_OWNDC | CS_DBLCLKS;
	wc2.lpfnWndProc 	= Brain;
	wc2.cbClsExtra 		= 0;
	wc2.cbWndExtra 		= 0;
	wc2.hInstance 		= hInstance;
	wc2.hIcon 				= LoadIcon(hInstance, "GEHIRN");
	wc2.hCursor 			= LoadCursor(NULL, IDC_ARROW);
	wc2.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc2.lpszMenuName  =  "BRAINMENU";
	wc2.lpszClassName = "BrainWClass";
	return(RegisterClass(&wc2));
}//InitApplication

/****************************************************************************
 Funktion:  		InitInstance

 Aufgabe:     	Instancespecifische Initialisierung

 Parameter:
	HANDLE hInstance : Handle der zu initialisierenden Instance
	int    nCmdShow  : Darstellungsart des Fensters

 R…kgabewert: BOOL 	TRUE wenn Initialisierung erfolgreich war
****************************************************************************/

BOOL InitInstance(HANDLE 	hInstance,
									int			nCmdShow)
{
HWND hWnd;

	hInst = hInstance;
	hWnd = CreateWindow("NeurorobWClass","NeuroRob",WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,                       /* Use the window class menu.         */
		hInstance,NULL);
	hWndNeurorob=hWnd;
	hWndBrain = CreateWindow((LPSTR)"BrainWClass",(LPSTR)"Brain",
		WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		CW_USEDEFAULT,NULL,NULL,hInst,NULL);
	if (!hWnd || !hWndBrain)
		return (FALSE);
	hMainMenu = GetMenu(hWnd);
	hBrainMenu= GetMenu(hWndBrain);
	CheckMenuItem(hBrainMenu,IDM9_MITTEL,MF_CHECKED);
	CheckMenuItem(hBrainMenu,IDM9_PRIOMIDDLE,MF_CHECKED);
	EnableMenuItem(hBrainMenu,IDM9_START,MF_GRAYED);
	EnableMenuItem(hBrainMenu,IDM9_SPEICHERN,MF_GRAYED);
	EnableMenuItem(hBrainMenu,IDM9_ALS,MF_GRAYED);
	EnableMenuItem(hBrainMenu,IDM9_ROBOTEREINSATZ,MF_GRAYED);
	EnableMenuItem(hBrainMenu,IDM9_ERKENNUNG,MF_GRAYED);
	EnableMenuItem(hBrainMenu,IDM9_TEST,MF_GRAYED);
	EnableMenuItem(hBrainMenu,IDM9_RESET,MF_GRAYED);
	EnableMenuItem(hMainMenu,IDM_STEUERUNG,MF_GRAYED);
	EnableMenuItem(hMainMenu,IDM_BEFEHLSLISTE,MF_GRAYED);
	EnableMenuItem(hMainMenu,IDM_SENDEFILE,MF_GRAYED);
	EnableMenuItem(hMainMenu,IDM_POSITIONSANA,MF_GRAYED);
	EnableMenuItem(hMainMenu,IDM_GELENKDEMO,MF_GRAYED);
	EnableMenuItem(hMainMenu,IDM_VISUALISIERUNG,MF_GRAYED);
//	EnableMenuItem(hMainMenu,IDM_NEUINIT,MF_GRAYED);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return (TRUE);
}//InitInstance

/****************************************************************************
 Funktion:    	MainWndProc

 Aufgabe:      	HauptCallBackFunktion von Windows

 Parameter:
	HWND 			hWnd   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD      wParam : Nachrichtenspezifische Informationen
	LONG			lParam :          "                 "

 R…kgabewert:
	long		0L wenn Nachricht bearbeitet sonst Defaultwert von DefWindowProc
****************************************************************************/

long FAR PASCAL MainWndProc(HWND 			hWnd,
														unsigned	message,
														WORD			wParam,
														LONG 			lParam)
{
FILE				*FilePointer;
FARPROC 		lpProcAbout;
FARPROC 		lpProcRobInit;
FARPROC			lpProcVideoinit;
FARPROC			lpProcPosition;
HDC					hDC;
HDC					hMemoryDC;
PAINTSTRUCT	ps;
char				string[6];
int 				Return;
HCURSOR			hSaveCursor;
char				where[30];
RECT				*lpRect;

	switch (message) {
		case WM_COMMAND:
				 switch (wParam) {
					 case IDM_INFO:
								lpProcAbout = MakeProcInstance(About, hInst);
								DialogBox(hInst,"AboutBox",hWnd,lpProcAbout);
								FreeProcInstance(lpProcAbout);
								break;
					 case IDM_SENDEFILE:
								szFileCaption = (LPSTR)"Textfile laden";
								lpProcLaden = MakeProcInstance(DateiLaden, hInst);
								if(!DialogBox(hInst,"FILEOPERATION",hWnd,lpProcLaden))
									break;
								FreeProcInstance(lpProcLaden) ;
								FilePointer=fopen(FileName,"rt");
								if(!FilePointer)
								{
									MessageBox(GetFocus(),"Datei konnte nicht geoeffnet werden",
										"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									return (FALSE);
								}
								ndummy=0;
								do
								{
									if (fgets(dummy, 90 , FilePointer) == NULL)
										break;
									dummy[strlen(dummy)-1] = '\0'; // Linefeed entfernen
									++ndummy;
									lstrcat((LPSTR)dummy,(LPSTR)"\n");
									SendeComm1(nCid,dummy);
								} while (TRUE);
								fclose(FilePointer);
								break;
					 case IDM_STEUERUNG:
								if(!bSteuerung)
								{
									lpSteuerung = MakeProcInstance(Steuerung,hInst);
									hDlgSteuerung = CreateDialog(hInst,"STEUERUNG",
										hWnd,lpSteuerung);
									bSteuerung = TRUE;
									CheckMenuItem(hMainMenu,IDM_STEUERUNG,MF_CHECKED);
								}
								else
								{
									DestroyWindow(hDlgSteuerung);
									FreeProcInstance(lpSteuerung);
									bSteuerung = FALSE;
									hDlgSteuerung = NULL;
									CheckMenuItem(hMainMenu,IDM_STEUERUNG,MF_UNCHECKED);
								}
								break;
					 case IDM_ROBINIT:
								lpProcRobInit = MakeProcInstance(RobInit, hInst);
								DialogBox(hInst,"ROBINIT",hWnd,lpProcRobInit);
								FreeProcInstance(lpProcRobInit);
								break;
					 case IDM_BEFEHLSLISTE:
								if(!bBefehlsliste)
								{
									lpBefehlsliste = MakeProcInstance(Befehlsliste,hInst);
									hDlgBefehlsliste = CreateDialog(hInst,"BEFEHLSLISTE",
										hWnd,lpBefehlsliste);
									bBefehlsliste = TRUE;
									CheckMenuItem(hMainMenu,IDM_BEFEHLSLISTE,MF_CHECKED);
								}
								else
								{
									DestroyWindow(hDlgBefehlsliste);
									FreeProcInstance(lpBefehlsliste);
									bBefehlsliste = FALSE;
									hDlgBefehlsliste = NULL;
									CheckMenuItem(hMainMenu,IDM_BEFEHLSLISTE,MF_UNCHECKED);
								}
								break;
					 case IDM_VISUALISIERUNG:
								if(!bVisualisierung)
								{
									lpVisualisierung = MakeProcInstance(Visualisierung,hInst);
									hDlgVisualisierung = CreateDialog(hInst,"VISUALISIERUNG",
										hWnd,lpVisualisierung);
									bVisualisierung = TRUE;
									CheckMenuItem(hMainMenu,IDM_VISUALISIERUNG,MF_CHECKED);
								}
								else
								{
									DestroyWindow(hDlgVisualisierung);
									FreeProcInstance(lpVisualisierung);
									bVisualisierung = FALSE;
									hDlgVisualisierung = NULL;
									CheckMenuItem(hMainMenu,IDM_VISUALISIERUNG,MF_UNCHECKED);
								}
								break;
					 case IDM_VIDEOINIT:
								hSaveCursor = SetCursor(hSivips);
								if(!VideoInit())
								{
									SetCursor(hSaveCursor);
									MessageBox(GetFocus(),
										(LPSTR)"COM 2 konnte nicht initialisiert werden",NULL,
										MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									break;
								}
								SetCursor(hSaveCursor);
								bVideomatInit=TRUE;
								MessageBox(GetFocus(),(LPSTR)
									"Bitte legen Sie den Abgleichgegenstand\n auf den Ursprung",
									(LPSTR)" ",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
								HolePartSchwerpunkt();
								xUrsprung=xCent+xPart;
								yUrsprung=yCent+yPart;
								SendeComm2(nCid2,"H");
								SendeComm2(nCid2,"B");
								break;
					 case IDM_VIDEOSTEU:
								if(!bVideoSteuerung)
								{
									lpVideoSteuerung = MakeProcInstance(VideoSteuerung,hInst);
									hDlgVideoSteuerung = CreateDialog(hInst,"VIDEOSTEUERUNG",
										hWnd,lpVideoSteuerung);
									bVideoSteuerung = TRUE;
									CheckMenuItem(hMainMenu,IDM_VIDEOSTEU,MF_CHECKED);
								}
								else
								{
									DestroyWindow(hDlgVideoSteuerung);
									FreeProcInstance(lpVideoSteuerung);
									bVideoSteuerung = FALSE;
									hDlgVideoSteuerung = NULL;
									CheckMenuItem(hMainMenu,IDM_VIDEOSTEU,MF_UNCHECKED);
								}
								break;
					 case IDM_NEUINIT:
								if(!bBrain)
								{
									ShowWindow(hWndBrain,SW_SHOWNORMAL);
									UpdateWindow(hWndBrain);
									bBrain = TRUE;
									CheckMenuItem(hMainMenu,IDM_NEUINIT,MF_CHECKED);
								}
								else
								{
									ShowWindow(hWndBrain,SW_HIDE);
									bBrain = FALSE;
									CheckMenuItem(hMainMenu,IDM_NEUINIT,MF_UNCHECKED);
								}
								break;
					 case IDM_GELENKDEMO:
								MessageBox(GetFocus(),(LPSTR)"Wir starten mit der DEMO",
								(LPSTR)"DEMO",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
								Gelenkdemo();
								MessageBox(GetFocus(),(LPSTR)"PUUHH.. Fertig",(LPSTR)"DEMO",
									MB_OK|MB_SYSTEMMODAL);
								break;
					 case IDM_BEENDEN:
								PostMessage(hWnd, WM_DESTROY,0,0);
					 case IDM_POSITIONSANA:
								lpProcPosition = MakeProcInstance(Positionsana,hInst);
								DialogBox(hInst,"POSITIONSANA",hWnd,lpProcPosition);
								FreeProcInstance(lpProcPosition);
								break;
					 default:
								return (DefWindowProc(hWnd, message, wParam, lParam));
				 }
		case WM_INITMENU:
				 EnableMenuItem(hMainMenu,IDM_ROBINIT,				MF_GRAYED);
				 EnableMenuItem(hMainMenu,IDM_STEUERUNG,			MF_GRAYED);
				 EnableMenuItem(hMainMenu,IDM_BEFEHLSLISTE,		MF_GRAYED);
				 EnableMenuItem(hMainMenu,IDM_SENDEFILE,			MF_GRAYED);
				 EnableMenuItem(hMainMenu,IDM_POSITIONSANA,		MF_GRAYED);
				 EnableMenuItem(hMainMenu,IDM_GELENKDEMO,			MF_GRAYED);
				 EnableMenuItem(hMainMenu,IDM_VISUALISIERUNG,	MF_GRAYED);
				 EnableMenuItem(hMainMenu,IDM_VIDEOSTEU,			MF_GRAYED);
				 EnableMenuItem(hMainMenu,IDM_VIDEOINIT,			MF_GRAYED);
				 if(bMoveMasterInit)
				 {
					 EnableMenuItem(hMainMenu,IDM_STEUERUNG,			MF_ENABLED);
					 EnableMenuItem(hMainMenu,IDM_BEFEHLSLISTE,		MF_ENABLED);
					 EnableMenuItem(hMainMenu,IDM_SENDEFILE,			MF_ENABLED);
					 EnableMenuItem(hMainMenu,IDM_POSITIONSANA,		MF_ENABLED);
					 EnableMenuItem(hMainMenu,IDM_GELENKDEMO,			MF_ENABLED);
					 EnableMenuItem(hMainMenu,IDM_VISUALISIERUNG,	MF_ENABLED);
				 }
				 else
					EnableMenuItem(hMainMenu,IDM_ROBINIT,MF_ENABLED);
				 if(bVideomatInit)
					 EnableMenuItem(hMainMenu,IDM_VIDEOSTEU,MF_ENABLED);
				 else
					 EnableMenuItem(hMainMenu,IDM_VIDEOINIT,MF_ENABLED);
				 break;
		case WM_CREATE:
				 hRobBitmap = LoadBitmap(hInst,"rob");
				 hAllBitmap = LoadBitmap(hInst,"all");
				 hHourGlass = LoadCursor(NULL,IDC_WAIT);
				 hMaster    = LoadCursor(hInst,"MASTER");
				 hSivips    = LoadCursor(hInst,"SIVIPS");
				 hRotPen1 = CreatePen(PS_SOLID,1,RGB(255,0,0));
				 hBlauPen1 = CreatePen(PS_SOLID,1,RGB(0,255,0));
				 hGruenPen1 = CreatePen(PS_SOLID,1,RGB(0,0,255));
				 hLilaPen1 = CreatePen(PS_SOLID,1,RGB(154,38,217));
				 hSchwarzPen1 = CreatePen(PS_SOLID,1,RGB(128,128,128));
				 hGelbPen1 = CreatePen(PS_SOLID,1,RGB(205,219,36));
				 hHellblauPen1 = CreatePen(PS_SOLID,1,RGB(28,218,227));
				 hGrauPen1 = CreatePen(PS_SOLID,1,RGB(173,100,82));
				 hRotBrush1 = CreateSolidBrush(RGB(255,0,0));
				 hBlauBrush1 = CreateSolidBrush(RGB(0,0,255));
				 hGruenBrush1 = CreateSolidBrush(RGB(0,255,0));
				 hLilaBrush1 = CreateSolidBrush(RGB(154,38,217));
				 hSchwarzBrush1 = CreateSolidBrush(RGB(128,128,128));
				 hGelbBrush1 = CreateSolidBrush(RGB(205,219,36));
				 hHellblauBrush1 = CreateSolidBrush(RGB(28,218,227));
				 hGrauBrush1 = CreateSolidBrush(RGB(173,100,82));
				 break;
		case WM_PAINT:
				 hDC= BeginPaint(hWnd,&ps);
				 hMemoryDC = CreateCompatibleDC(hDC);
				 hOldBitmap = SelectObject(hMemoryDC,hAllBitmap);
				 if(hOldBitmap)
				 {
					 BitBlt (hDC,0,0,507,308,hMemoryDC,0,0,SRCCOPY);
					 SelectObject(hMemoryDC,hOldBitmap);
				 }
				 DeleteDC(hMemoryDC);
				 EndPaint(hWnd,&ps);
				 break;
		case WM_DESTROY:
				 if(bNetzdefine&&bSpeichernAbfrage)
				 {
					 if(MessageBox(GetFocus(),
							(LPSTR)"Moechten Sie das neuronale Netz speichern",(LPSTR)"",
								MB_YESNO|MB_ICONQUESTION|MB_SYSTEMMODAL)==IDYES)
					 {
						 if(bSpeichern)
							 SendMessage(hWndBrain,WM_COMMAND,IDM9_SPEICHERN,0L);
						 else
							 SendMessage(hWndBrain,WM_COMMAND,IDM9_ALS,0L);
					 }
				 }
				 DestroyWindow(hWndBrain);
				 DeleteObject(hRobBitmap);
				 DeleteObject(hAllBitmap);
				 DeleteObject(hRotPen1);
				 DeleteObject(hBlauPen1);
				 DeleteObject(hLilaPen1);
				 DeleteObject(hSchwarzPen1);
				 DeleteObject(hGelbPen1);
				 DeleteObject(hHellblauPen1);
				 DeleteObject(hGruenPen1);
				 DeleteObject(hGrauPen1);
				 DeleteObject(hRotBrush1);
				 DeleteObject(hBlauBrush1);
				 DeleteObject(hLilaBrush1);
				 DeleteObject(hSchwarzBrush1);
				 DeleteObject(hGelbBrush1);
				 DeleteObject(hHellblauBrush1);
				 DeleteObject(hGruenBrush1);
				 DeleteObject(hGrauBrush1);
				 if(bMoveMasterInit)
				 {
					 SendeComm1(nCid,"PD 999,0,400,200,-90,90,O\n");
					 SendeComm1(nCid,"PD 1,153.6,-232.9,813.4,-10.2,179.9,O\n");
					 SendeComm1(nCid,"SP 18 \n");
					 SendeComm1(nCid,"MO 1\n");
				 }
				 CloseComm(nCid);
				 CloseComm(nCid2);
				 PostQuitMessage(0);
				 break;
		default:
				 return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (NULL);
}//MainWndProc

void Anzeige(int Wert)
{
char  dooferdummy[20];
	itoa(Wert,dooferdummy,10);
	MessageBox(GetFocus(),(LPSTR)dooferdummy,(LPSTR)"Anzeige",MB_OK);
}//Anzeige



