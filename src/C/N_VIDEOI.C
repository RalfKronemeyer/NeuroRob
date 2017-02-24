/****************************************************************************
 Modul:   		N_VIDEO.C

 Aufgabe:    	EnthÑlt Funktion, die die Kommunikation mit der Sivips-Anlage
							Åbernehmen

 Funktionen:  VideoInit
							HolePartSchwerpunkt
							VideoSteuerung
							SendeComm2
							HoleVideomatZeile
****************************************************************************/

#include "windows.h"
#include "neurorob.h"
#include "n_extern.h"
#include "neuromen.h"
#include "ndl_vid.h"
#include "math.h"

/****************************************************************************
 Funktion:    	VideoInit

 Aufgabe:     	Initialisiert die serielle Schnittstelle

 Parameter:

 RÅckgabewert:  BOOL  TRUE wenn erfolgreich
****************************************************************************/


BOOL VideoInit (void)
{
char 		szSetting[] = "COM2: 9600,E,7,2";
int 		nFehler;
COMSTAT Commstatus;
DCB 		CommDCB;

	if(nCid!=0)
		CloseComm(nCid2);
	if (BuildCommDCB(szSetting,&CommDCB)!=0)
		return(FALSE);
	CommDCB.fOutxDsrFlow=1;
	if((nCid2 = OpenComm((LPSTR)"COM2",128,128))<0)
		return(FALSE);
	if (SetCommState(&CommDCB)!=0)
		return(FALSE);
	FlushComm(nCid2,0);
	FlushComm(nCid2,1);
	return(TRUE);
}//VideoInit

/****************************************************************************
 Funktion:    	HolePartSchwerpunkt

 Aufgabe:     	Holt vom Videomaten den Schwerpunkt eines Parts

 Parameter:     -

 RÅckgabewert:  -
****************************************************************************/

void HolePartSchwerpunkt(void)
{
char Koordinaten[50];

	Koordinaten[0]='\0';
	SendeComm2(nCid2,"X");
	HoleVideomatZeile((LPSTR)Koordinaten);
	ndummy=0;
	while(Koordinaten[ndummy]==' ')
		++ndummy;
	strcpy(dummy,&(Koordinaten[ndummy]));
	yPart=atof(dummy);
	Koordinaten[0]='\0';
	SendeComm2(nCid2,"A");
	HoleVideomatZeile((LPSTR)Koordinaten);
	ndummy=0;
	while(Koordinaten[ndummy]==' ')
		++ndummy;
	strcpy(dummy,&(Koordinaten[ndummy]));
	xPart=atof(dummy);
}//HolePartSchwerpunkt

/****************************************************************************
 Funktion:  		Videosteuerung

 Aufgabe:       Darstellen und bearbeiten der VideoSteuerung-Dialogbox

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/


BOOL FAR PASCAL VideoSteuerung(	HWND			hDlg,
																unsigned	message,
																WORD			wParam,
																LONG			lParam)
{
	switch (message){
		case WM_INITDIALOG:
				 return (TRUE);
		case WM_COMMAND:
				 switch(wParam){
					 case	IDOK:
					 case IDCANCEL:
								SendMessage(hDlg,WM_SYSCOMMAND,SC_CLOSE,0L);
								return (TRUE);
					 case ID21_LIVEG:
								SendeComm2(nCid2,"B");
								break;
					 case ID21_LIVEB:
								SendeComm2(nCid2,"D");
								break;
					 case ID21_GRAU:
								SendeComm2(nCid2,"C");
								break;
					 case ID21_BINAER:
								SendeComm2(nCid2,"E");
								break;
					 case ID21_PLUS:
								SendeComm2(nCid2,"P");
								break;
					 case ID21_MINUS:
								SendeComm2(nCid2,"M");
								break;
					 case ID21_HISTOGRAMM:
								SendeComm2(nCid2,"H");
								break;
					 case ID21_ORIENTIERUNG:
								SendeComm2(nCid2,"O");
								break;
				 }
				 break;
		case WM_SYSCOMMAND:
				 switch(wParam){
					 case SC_CLOSE:
								DestroyWindow(hDlgVideoSteuerung);
								FreeProcInstance(lpVideoSteuerung);
								bVideoSteuerung = FALSE;
								hDlgVideoSteuerung = NULL;
								CheckMenuItem(hMainMenu,IDM_VIDEOSTEU,MF_UNCHECKED);
								return TRUE;
					 default:
								return FALSE;
				 }
				 break;
	}
	return(FALSE);
}//VideoSteuerung

/****************************************************************************
 Funktion:      SendeComm2

 Aufgabe:       Sendet Zeichenketten zur Sivips-Anlage

 Parameter:     nCid          : Handle der Schnittstelle
								*befehlsstring: Zeiger auf die zu sendende Zeichenkette

 RÅckgabewert:  int   negativ falls Fehler aufgetreten
****************************************************************************/


int	SendeComm2(HANDLE	nCid2,char *befehlsstring)
{
COMSTAT	CommStatus;
int			nFehler;

	if((nFehler=GetCommError(nCid2,&CommStatus))!=0)
	{
		itoa(nFehler,dummy,10);
		MessageBox(GetFocus(),(LPSTR)dummy,(LPSTR)"GetCommError",
			MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		return(-20);
	}
	do
	{
		GetCommError(nCid2,&CommStatus);
	}while(CommStatus.cbOutQue!=0);

	if(WriteComm(nCid2,befehlsstring,min(lstrlen(befehlsstring),
			128-CommStatus.cbOutQue))<0)
	{
		MessageBox(GetFocus(),(LPSTR)"Fehler beim Schreiben auf COM2",
			(LPSTR)"WriteComm",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
		return(-20);
	}
	return(0);
}//SendeComm2

/****************************************************************************
 Funktion: 				HoleViedomatZeile

 Aufgabe:         EmpfÑngt eine Zeichenkette vom Videomatsystem

 Parameter:       video :Zeiger auf Zeichenkette die genÅgend Speicherplatz
									zur VerfÅgung stellt

 RÅckgabewert:    -
****************************************************************************/


void HoleVideomatZeile(LPSTR video)
{
	video[0]='\0';
	do
	{
		ndummy = ReadComm(nCid2,(LPSTR)dummy,50);
		dummy[ndummy]='\0';
		lstrcat((LPSTR)video,(LPSTR)dummy);
	}while((ndummy == 0) || (dummy[ndummy-1]!='\n'));
	dummy[0] = '\0';
}//HoleVideomatZeile

