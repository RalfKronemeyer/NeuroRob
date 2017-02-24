/****************************************************************************
 Modul:        	N_ABOUT.C

 Aufgabe:       Darstellung der Indormationsdialogbox

 Funktionen:    About
****************************************************************************/

#include "windows.h"
#include "neurorob.h"
#include "n_extern.h"
#include "ndl_abo.h"

FARPROC  lpProcFace;


/****************************************************************************
 Funktion:  		About

 Aufgabe:       Informationsdialogbox darstellen

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/

BOOL FAR PASCAL About(HWND			hDlg,
											unsigned	message,
											WORD			wParam,
											LONG			lParam)
{
	switch (message){
		case WM_INITDIALOG:
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam){
							 case IDOK:
							 case IDCANCEL:
										EndDialog(hDlg, TRUE);
										return(TRUE);
							 case ID12_SHOW:
										lpProcFace = MakeProcInstance(Face, hInst);
										DialogBox(hInst,"FACEBOX",hDlg,lpProcFace);
										FreeProcInstance(lpProcFace);
										break;
				 }
	}
	return (FALSE);
}//About


/****************************************************************************
 Funktion:  		Face

 Aufgabe:       Informationsdialogbox darstellen

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/

BOOL FAR PASCAL Face(HWND			hDlg,
										 unsigned	message,
										 WORD			wParam,
										 LONG			lParam)


{
HDC 				hDC;
HDC					hMemoryDC;
PAINTSTRUCT ps;

	switch (message){
		case WM_INITDIALOG:
				 hFaceBitmap = LoadBitmap(hInst,"face");
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam){
							 case IDOK:
							 case IDCANCEL:
										EndDialog(hDlg, TRUE);
										return(TRUE);
				 }
		case WM_PAINT:
				 hDC = BeginPaint(hDlg,&ps);
				 hMemoryDC = CreateCompatibleDC(hDC);
				 hOldBitmap = SelectObject(hMemoryDC,hFaceBitmap);
				 if(hOldBitmap)
				 {
					 BitBlt (hDC,0,0,600,600,hMemoryDC,0,0,SRCCOPY);
					 SelectObject(hMemoryDC,hOldBitmap);
				 }
				 DeleteDC(hMemoryDC);
				 EndPaint(hDlg,&ps);
					break;

	}
	return (FALSE);
}//Face

