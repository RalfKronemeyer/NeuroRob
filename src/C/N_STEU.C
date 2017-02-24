/****************************************************************************
 Modul:        N_STEU.C

 Aufgabe:      öbernimmt die Verwaltung der Steuerungdialogbox

 Funktionen:   Steuerung
****************************************************************************/


#include "windows.h"
#include "neurorob.h"
#include "n_extern.h"
#include "ndl_steu.h"
#include "ndl_visu.h"
#include "neuromen.h"
#include "ndl_list.h"

/****************************************************************************
 Funktion:  		Steuerung

 Aufgabe:       Darstellen und Bearbeiten der Steuerungdialogbox

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/

BOOL FAR PASCAL Steuerung(HWND			hDlg,
													unsigned	message,
													WORD			wParam,
													LONG			lParam)

{
char				texti[11];
HDC					hDC;
PAINTSTRUCT	ps;
HDC					hMemoryDC;
int					balkenmerker;
BOOL				speed;
int					scrollmax[4]={20,100,63,45};
int					page[4]={4,10,5,5};
char				befehl[20];

	switch (message){
		case WM_INITDIALOG:
				 SetScrollRange(GetDlgItem(hDlg,ID2_SPEED),SB_CTL,0,scrollmax[0],0);
				 SetScrollPos(GetDlgItem(hDlg,ID2_SPEED),SB_CTL,roboterspeed,1);
				 SetScrollRange(GetDlgItem(hDlg,ID2_STEP),SB_CTL,0,scrollmax[1],0);
				 SetScrollPos(GetDlgItem(hDlg,ID2_STEP),SB_CTL,roboterstep,1);
				 SetScrollRange(GetDlgItem(hDlg,ID2_GRID),SB_CTL,0,scrollmax[2],0);
				 SetScrollPos(GetDlgItem(hDlg,ID2_GRID),SB_CTL,robotergrid,1);
				 SetScrollRange(GetDlgItem(hDlg,ID2_DEGREE),SB_CTL,0,scrollmax[3],0);
				 SetScrollPos(GetDlgItem(hDlg,ID2_DEGREE),SB_CTL,roboterdegree,1);
				 wsprintf((LPSTR)dummy,(LPSTR)"Geschwindigkeit : %i",roboterspeed);
				 SetWindowText(GetDlgItem(hDlg,ID2_TEXTSPEED),(LPSTR)dummy);
				 wsprintf(befehl,"SP %i\n",roboterspeed);
				 SendeComm1(nCid,befehl);
				 wsprintf((LPSTR)dummy,(LPSTR)"Schrittweite : %i mm",roboterstep);
				 SetWindowText(GetDlgItem(hDlg,ID2_TEXTMM),(LPSTR)dummy);
				 wsprintf((LPSTR)dummy,(LPSTR)"Greifst‰rke : %i",robotergrid);
				 SetWindowText(GetDlgItem(hDlg,ID2_TEXTGRID),(LPSTR)dummy);
				 wsprintf(befehl,"GP %i,%i,3\n",robotergrid,robotergrid);
				 SendeComm1(nCid,befehl);
				 wsprintf((LPSTR)dummy,(LPSTR)"Schrittwinkel : %i ∞",roboterdegree);
				 SetWindowText(GetDlgItem(hDlg,ID2_TEXTGRAD),(LPSTR)dummy);
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam){
					 case IDOK:
					 case IDCANCEL:
								SendMessage(hDlg,WM_SYSCOMMAND,SC_CLOSE,0L);
								return (TRUE);
					 case ID2_HAND_OPEN:
								SendeComm1(nCid,(LPSTR)"GO\n");
								break;
					 case ID2_HAND_CLOSE:
								SendeComm1(nCid,(LPSTR)"GC\n");
								break;
					 case ID2_SHOULDER_P:
								wsprintf(befehl,"MJ 0,%i,0,0,0\n",roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_SHOULDER_N:
								wsprintf(befehl,"MJ 0,%i,0,0,0\n",-roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_WAIST_P:
								wsprintf(befehl,"MJ %i,0,0,0,0\n",roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_WAIST_N:
								wsprintf(befehl,"MJ %i,0,0,0,0\n",-roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_ELBOW_P:
								wsprintf(befehl,"MJ 0,0,%i,0,0\n",roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_ELBOW_N:
								wsprintf(befehl,"MJ 0,0,%i,0,0\n",-roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_HAND_UP:
								wsprintf(befehl,"MJ 0,0,0,%i,0\n",roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_HAND_DOWN:
								wsprintf(befehl,"MJ 0,0,0,%i,0\n",-roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_HAND_RIGHT:
								wsprintf(befehl,"MJ 0,0,0,0,%i\n",-roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_HAND_LEFT:
								wsprintf(befehl,"MJ 0,0,0,0,%i\n",roboterdegree);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_MO_999:
								SendeComm1(nCid,"MO 999\n");
								break;
					 case ID2_LINKS:
								wsprintf(befehl,"DS %i,0,0\n",+roboterstep);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_RECHTS:
								wsprintf(befehl,"DS %i,0,0\n",-roboterstep);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_ZURUECK:
								wsprintf(befehl,"DS 0,%i,0\n",-roboterstep);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_VOR:
								wsprintf(befehl,"DS 0,%i,0\n",roboterstep);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_RUNTER:
								wsprintf(befehl,"DS 0,0,%i\n",-roboterstep);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_HOCH:
								wsprintf(befehl,"DS 0,0,%i\n",roboterstep);
								SendeComm1(nCid,befehl);
								break;
					 case ID2_ADD:
								if(bBefehlsliste==FALSE)
									SendMessage(GetParent(hDlg),WM_COMMAND,IDM_BEFEHLSLISTE,0L);
								SendMessage(hDlgBefehlsliste,WM_COMMAND,ID3_ADD,0L);
								SetFocus(hDlg);
								break;
					 case ID2_DEFINIERN:
								GetDlgItemText(hDlg,ID2_POSEDIT,(LPSTR)dummy,5);
								ndummy=atoi(dummy);
								if(ndummy<2 || ndummy>990)
								{
									MessageBox(GetFocus(),(LPSTR)"Unzulaessige Positionsnummer",
										NULL,MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									break;
								}
								HE(dummy);
								if (ndummy<998)
								{
									wsprintf((LPSTR)dummy,(LPSTR)"%i",ndummy+1);
									SetDlgItemText(hDlg,ID2_POSEDIT,(LPSTR)dummy);
								}
								break;
				 }
				 if(bVisualisierung)
					 SendMessage(hDlgVisualisierung,WM_COMMAND,ID10_AKTUELL,0L);
				 break;
		case WM_SYSCOMMAND:
				 switch(wParam){
					 case SC_CLOSE:
								DestroyWindow(hDlgSteuerung);
								FreeProcInstance(lpSteuerung);
								bSteuerung = FALSE;
								hDlgSteuerung = NULL;
								CheckMenuItem(hMainMenu,IDM_STEUERUNG,MF_UNCHECKED);
								return TRUE;
					 default:
								return FALSE;
				 }
				 break;
		case WM_PAINT:
				 hDC = BeginPaint(hDlg,&ps);
				 hMemoryDC  = CreateCompatibleDC(hDC);
				 hOldBitmap = SelectObject(hMemoryDC,hRobBitmap);
				 if(hOldBitmap)
				 {
						BitBlt (hDC,60,40,600,550,hMemoryDC,0,0,SRCCOPY);
						SelectObject(hMemoryDC,hOldBitmap);
				 }
				 DeleteDC(hMemoryDC);
				 EndPaint(hDlg,&ps);
				 break;
		case WM_HSCROLL:
				 hDC=GetDC(hDlg);
				 balkenmerker = GetScrollPos(HIWORD(lParam),SB_CTL);
				 if (HIWORD(lParam) == GetDlgItem(hDlg,ID2_SPEED))
						speed=0;
				 if (HIWORD(lParam) == GetDlgItem(hDlg,ID2_STEP))
						speed=1;
				 if (HIWORD(lParam) == GetDlgItem(hDlg,ID2_GRID))
						speed=2;
				 if (HIWORD(lParam) == GetDlgItem(hDlg,ID2_DEGREE))
						speed=3;
				 switch(wParam){
					 case SB_LINEDOWN:
								if(balkenmerker<scrollmax[speed])
								++balkenmerker;
								break;
					 case SB_LINEUP:
								if(balkenmerker>0)
									--balkenmerker;
								break;
					 case SB_PAGEDOWN:
								if(balkenmerker<scrollmax[speed]-page[speed])
									balkenmerker+=page[speed];
								else
									balkenmerker=scrollmax[speed];
								break;
					 case SB_PAGEUP:
								if(balkenmerker>page[speed])
									balkenmerker-=page[speed];
								else
									balkenmerker=0;
								break;
					 case SB_BOTTOM:
								balkenmerker=0;
								break;
					 case SB_TOP:
								balkenmerker=scrollmax[speed];
								break;
					 case SB_THUMBTRACK:
								balkenmerker=LOWORD(lParam);
								break;
				 }
				 SetScrollPos(HIWORD(lParam),SB_CTL,balkenmerker,1);
				 if (speed == 0)
				 {
						roboterspeed = balkenmerker;
						wsprintf((LPSTR)dummy,(LPSTR)"Geschwindigkeit : %i",roboterspeed);
						SetWindowText(GetDlgItem(hDlg,ID2_TEXTSPEED),(LPSTR)dummy);
						wsprintf(befehl,"SP %i\n",roboterspeed);
						SendeComm1(nCid,befehl);
				 }
				 if (speed == 1)
				 {
						roboterstep = balkenmerker;
						wsprintf((LPSTR)dummy,(LPSTR)"Schrittweite : %i mm",roboterstep);
						SetWindowText(GetDlgItem(hDlg,ID2_TEXTMM),(LPSTR)dummy);
				 }
				 if (speed == 2)
				 {
						robotergrid = balkenmerker;
						wsprintf((LPSTR)dummy,(LPSTR)"Greifst‰rke : %i",robotergrid);
						SetWindowText(GetDlgItem(hDlg,ID2_TEXTGRID),(LPSTR)dummy);
						wsprintf(befehl,"GP %i,%i,3\n",robotergrid,robotergrid);
						SendeComm1(nCid,befehl);
				 }
				 if (speed == 3)
				 {
						roboterdegree = balkenmerker;
						wsprintf((LPSTR)dummy,(LPSTR)"Schrittwinkel : %i ∞",
							roboterdegree);
						SetWindowText(GetDlgItem(hDlg,ID2_TEXTGRAD),(LPSTR)dummy);
				 }
				 ReleaseDC(hDlg,hDC);
				 break;
	}
	return(FALSE);
}//Steuerung
