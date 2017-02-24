/****************************************************************************
 Modul:   		N_POSITI

 Aufgabe:    	Stellt Funktionen zur Positionsanalyse zur VerfÅgung

 Funktionen:  Positionsana
****************************************************************************/

#include <windows.h>
#include "n_extern.h"
#include "ndl_pos.h"
#include "neurorob.h"
#include "string.h"
#include <stdio.h>


char 	Befehl[50];
char 	Befehl1[50];
char 	where[50];
char 	feld1[10];
char	feld2[10];
char	feld3[10];
char	feld4[10];
char	feld5[10];
char	feld6[10];
char	feld7[10];
int 	i,j,c,anzahl;
int		lenwhere=0;
DWORD index;

/****************************************************************************
 Funktion:  		Positionsana

 Aufgabe:       Darstellung der Dialogbox fÅr die Positionsanalyse

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/

BOOL FAR PASCAL Positionsana (HWND 			hDlg,
															unsigned 	message,
															WORD 			wParam,
															LONG 			lParam)
{
COMSTAT		CommStatus;
int 			nFehler;
FILE      *FilePointer;

	switch(message){
		case WM_INITDIALOG:
				 return(TRUE);
		case WM_COMMAND:
				 switch(wParam){
					 case IDOK:
					 case IDCANCEL:
								EndDialog(hDlg, FALSE);
								return(TRUE);
					 case ID8_ROBOTER:
								SendDlgItemMessage(hDlg,ID8_POSITION,LB_RESETCONTENT,0,0);
								SetCursor(LoadCursor(NULL,IDC_WAIT));
								for(i=1;i<=999;i++)
								{
									itoa(i,dummy,10);
									wsprintf((LPSTR)Befehl,(LPSTR)"PR %s\n",(LPSTR)dummy);
									FlushComm(nCid,1);
									lenwhere=0;
									Sende(nCid,Befehl);
									where[0]='\0';
									do
									{
										ndummy=ReadComm(nCid,(LPSTR)dummy,50);
										dummy[ndummy]='\0';
										lstrcat((LPSTR)where,(LPSTR)dummy);
										lenwhere+=ndummy;
										nFehler = GetCommError(nCid,&CommStatus);
										if (nFehler>0)
											MessageBox(GetFocus(),(LPSTR)"Comm",(LPSTR)"FEHLER",
												MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									}while(dummy[ndummy-1]!='\n');
									where[lenwhere-3] = '\0';
									ndummy = lstrlen((LPSTR)where);
									if(ndummy>11)
									{
										itoa(i,dummy,10);
										ndummy = SendDlgItemMessage(hDlg,ID8_POSITION,
											LB_ADDSTRING,0,(LPSTR)dummy);
									}
								}
								SetCursor(LoadCursor(NULL,IDC_ARROW));
								return(TRUE);
					 case ID8_POSITION:
								if(HIWORD(lParam)!=LBN_SELCHANGE)
									break;
								index=SendDlgItemMessage(hDlg,ID8_POSITION,LB_GETCURSEL,0,0);
								if (index == LB_ERR)
									MessageBox(GetFocus(),(LPSTR)"Kein Eintrag",(LPSTR)"DATEN",
										MB_OK|MB_SYSTEMMODAL);
								else
								{
									SendDlgItemMessage(hDlg,ID8_POSITION,LB_GETTEXT,index,
										(LPSTR)dummy);
									SetDlgItemText(hDlg,ID8_POSITIONSNR,(LPSTR)dummy);
									wsprintf((LPSTR)Befehl,(LPSTR)"PR %s\n",(LPSTR)dummy);
									FlushComm(nCid,1);
									Sende(nCid,(LPSTR)Befehl);
									where[0]='\0';
									do
									{
										ndummy=ReadComm(nCid,(LPSTR)dummy,50);
										dummy[ndummy]='\0';
										lstrcat((LPSTR)where,(LPSTR)dummy);
										lenwhere+=ndummy;
										nFehler = GetCommError(nCid,&CommStatus);
										if (nFehler>0)
											MessageBox(GetFocus(),(LPSTR)"Comm",NULL,
												MB_OK|MB_SYSTEMMODAL);
									}while(dummy[ndummy-1]!='\n');
									where[lenwhere-3] = '\0';
									i=0;
									c=0;
									while(where[i]!=',')
									{
										feld1[c] = where[i];
										i++;
										c++;
									}
									feld1[c]='\0';
									c=0;
									i++;
									while(where[i]!=',')
									{
										feld2[c] = where[i];
										i++;
										c++;
									}
									feld2[c]='\0';
									c=0;
									i++;
									while(where[i]!=',')
									{
										feld3[c] = where[i];
										i++;
										c++;
									}
									feld3[c]='\0';
										c=0;
										i++;
									while(where[i]!=',')
									{
										feld4[c] = where[i];
										i++;
										c++;
									}
									feld4[c]='\0';
									c=0;
									i++;
									while(where[i]!=',')
									{
										feld5[c] = where[i];
										i++;
										c++;
									}
									feld5[c]='\0';
									c=0;
									i++;
									while(where[i]!='\0')
									{
										feld6[c] = where[i];
										i++;
										c++;
									}
									feld6[c]='\0';
									SetDlgItemText(hDlg,ID8_XWERT,(LPSTR)feld1);
									SetDlgItemText(hDlg,ID8_YWERT,(LPSTR)feld2);
									SetDlgItemText(hDlg,ID8_ZWERT,(LPSTR)feld3);
									SetDlgItemText(hDlg,ID8_HANDPITCH,(LPSTR)feld4);
									SetDlgItemText(hDlg,ID8_HANDROLL,(LPSTR)feld5);
									SetDlgItemText(hDlg,ID8_HANDGRIP,(LPSTR)feld6);
								}
								break;
					 case ID8_MOVE:
								SendMessage(hDlg,WM_COMMAND,ID8_DATEN,0L);
								index=SendDlgItemMessage(hDlg,ID8_POSITION,LB_GETCURSEL,0,0);
								if(index == LB_ERR)
								{
									MessageBox(GetFocus(),(LPSTR)"Kein Eintrag",(LPSTR)"DATEN",
										MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
								}
								else
								{
									SendDlgItemMessage(hDlg,ID8_POSITION,LB_GETTEXT,index,
										(LPSTR)dummy);
									SetDlgItemText(hDlg,ID8_POSITIONSNR,(LPSTR)dummy);
									wsprintf((LPSTR)Befehl,(LPSTR)"MO %s,%s\n",(LPSTR)dummy,
										(LPSTR)feld6);
									FlushComm(nCid,1);
									SendeComm1(nCid,Befehl);
								}
								break;
					 case ID8_NEW:
								index=SendDlgItemMessage(hDlg,ID8_POSITION,LB_GETCURSEL,0,0);
								if(index==LB_ERR)
								{
									MessageBox(GetFocus(),(LPSTR)"Kein Eintrag",(LPSTR)"DATEN",
										MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
								}
								else
								{
									SendDlgItemMessage(hDlg,ID8_POSITION,LB_GETTEXT,index,
										(LPSTR)dummy);
									SendDlgItemMessage(hDlg,ID8_POSITION,LB_DELETESTRING,index,
										0);
									SetDlgItemText(hDlg,ID8_POSITIONSNR,(LPSTR)dummy);
									wsprintf((LPSTR)Befehl,(LPSTR)"PC %s\n",(LPSTR)dummy);
									FlushComm(nCid,1);
									SendeComm1(nCid,Befehl);
								}
								break;
					 case ID8_SAVE:
								lstrcpy((LPSTR)DefExt,(LPSTR)".POS");
								szFileCaption = (LPSTR)"Positionsdaten speichern";
								lpProcLaden = MakeProcInstance(DateiLaden, hInst);
								if(!DialogBox(hInst,"FILEOPERATION",hDlg,lpProcLaden))
									break;
								FreeProcInstance(lpProcLaden) ;
								FilePointer=fopen(FileName,"wt");
								if(!FilePointer)
								{
									MessageBox(GetFocus(),
										(LPSTR)"Datei konnte nicht geoeffnet werden",
										(LPSTR)"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									return (FALSE);
								}
								anzahl=SendDlgItemMessage(hDlg,ID8_POSITION,LB_GETCOUNT,0,0);
								for(index=0;index<anzahl;index++)
								{
									SendDlgItemMessage(hDlg,ID8_POSITION,LB_GETTEXT,index,
										(LPSTR)dummy);
									wsprintf((LPSTR)dummy2,(LPSTR)"%s\n",(LPSTR)dummy);
									fputs(dummy2,FilePointer);
									wsprintf((LPSTR)Befehl,(LPSTR)"PR %s",(LPSTR)dummy2);
									lenwhere=0;
									FlushComm(nCid,1);
									Sende(nCid,Befehl);
									where[0]='\0';
									do
									{
										ndummy=ReadComm(nCid,(LPSTR)dummy,50);
										dummy[ndummy]='\0';
										lstrcat((LPSTR)where,(LPSTR)dummy);
										lenwhere+=ndummy;
										nFehler = GetCommError(nCid,&CommStatus);
										if (nFehler>0)
											MessageBox(GetFocus(),(LPSTR)"Comm",(LPSTR)"FEHLER",
												MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									}while(dummy[ndummy-1]!='\n');
									where[lenwhere-2] = '\n';
									where[lenwhere-2] = '\0';
									lstrcat((LPSTR)where,(LPSTR)"\n");
									fputs(where,FilePointer);
								}
								fclose(FilePointer);
								break;
					 case ID8_LOAD:
								SendDlgItemMessage(hDlg,ID8_POSITION,LB_RESETCONTENT,0,0L);
								SendeComm1(nCid,"NW\n");
								strcpy(DefExt,".POS");
								szFileCaption = (LPSTR)"Positionen laden";
								lstrcpy((LPSTR)DefExt,(LPSTR)".POS");
								lpProcLaden = MakeProcInstance(DateiLaden, hInst);
								if(!DialogBox(hInst,"FILEOPERATION",hDlg,lpProcLaden))
									break;
								FreeProcInstance(lpProcLaden) ;
								FilePointer=fopen(FileName,"rt");
								if(!FilePointer)
								{
									MessageBox(GetFocus(),
									(LPSTR)"Datei konnte nicht geoeffnet werden",
									(LPSTR)"Fehler",MB_OK|MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									return (FALSE);
								}
								ndummy=0;
								do
								{
									if(fgets(dummy,90,FilePointer)==NULL)
										break;
									dummy[strlen(dummy)-1]='\0';
									ndummy=SendDlgItemMessage(hDlg,ID8_POSITION,
										LB_ADDSTRING,0,(LPSTR)dummy);
									if((ndummy==LB_ERR)||(ndummy==LB_ERRSPACE))
										MessageBox(GetFocus(),
											(LPSTR)"Fehler beim Laden einer Liste",
											(LPSTR)"Daten lesen",
											MB_OK||MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
									if(fgets(dummy2,90,FilePointer)==NULL)
											break;
									dummy2[strlen(dummy2)-1]='\0';
									wsprintf((LPSTR)Befehl1,(LPSTR)"%s,%s",(LPSTR)dummy,
										(LPSTR)dummy2);
									wsprintf((LPSTR)Befehl,(LPSTR)"PD %s",(LPSTR)Befehl1);
									lstrcat((LPSTR)Befehl,(LPSTR)"\n");
									SendeComm1(nCid,Befehl);
								}while(TRUE);
								fclose(FilePointer);
								break;
				 }
				 break;
	}
	return(FALSE);
}//Positionsana

