/****************************************************************************
 Modul:    		N_FILE.C

 Aufgabe:     Stellt eine Dateiauswahlbox zur VerfÅgung

 Funktionen:  DateiLaden
****************************************************************************/

#include "windows.h"
#include "neurorob.h"
#include "n_extern.h"
#include "n_file.h"

/****************************************************************************
 Funktion:  		DateiLaden

 Aufgabe:       Dateiauswahldialogbox darstellen

 Parameter:
	HWND			hDlg   : Fensterhandle
	unsigned 	message: Typ der Nachricht
	WORD			wParam : NachrichtenabhÑngiger Wert
	LONG			lParam :        "               "

 RÅckgabewert:
	BOOL					TRUE wenn message abgearbeitet
****************************************************************************/

BOOL FAR PASCAL DateiLaden(HWND			hDlg,
													 unsigned	message,
													 WORD			wParam,
													 LONG			lParam)
{
	switch (message){
		case WM_INITDIALOG:
				 SendMessage(hDlg,WM_SETTEXT,NULL,(LPSTR)szFileCaption);
				 wsprintf((LPSTR)dummy,(LPSTR)"*%s",(LPSTR)DefExt);
				 SetDlgItemText(hDlg,ID6_FILE,(LPSTR)dummy);
				 DlgDirList(hDlg,(LPSTR)dummy,ID6_FILES,0,0x0000);
				 DlgDirList(hDlg,(LPSTR)"D:\*.*",
					ID6_DIRECTORIES,ID6_DIRECTORY,0xC010);
				 SendDlgItemMessage(hDlg,ID6_FILE,EM_SETSEL,NULL,MAKELONG(0,0x7fff));
				 SetFocus(GetDlgItem(hDlg,ID6_FILE));
				 return (FALSE);
		case WM_COMMAND:
				 switch(wParam){
					 case IDOK:
								SendDlgItemMessage(hDlg,ID6_FILE,WM_GETTEXT,13,
									(LPSTR)FileName);
								if((strchr(FileName,'*')==NULL)&&(strchr(FileName,'?')==NULL))
								{
									EndDialog(hDlg,TRUE);
									return(TRUE);
								}
								else
								{
									DlgDirList(hDlg,(LPSTR)FileName,ID6_FILES,0,0x0000);
								}
								break;
					 case IDCANCEL:
								EndDialog(hDlg, FALSE);
								return (TRUE);
					 case ID6_DIRECTORIES:
								switch(HIWORD(lParam)){
									 case LBN_DBLCLK:
												DlgDirSelect(hDlg,str,ID6_DIRECTORIES);
												strcat(str,"*");
												strcat(str,DefExt);
												DlgDirList(hDlg,(LPSTR)str,ID6_FILES,0,0x0000);
												DlgDirList(hDlg,(LPSTR)str,ID6_DIRECTORIES,
													ID6_DIRECTORY,0xC010);
												PostMessage(hDlg,WM_COMMAND,IDOK,0L);
												break;
								}
								break;
					 case ID6_FILES:
								switch(HIWORD(lParam)){
									 case LBN_SELCHANGE:
												DlgDirSelect(hDlg,str,ID6_FILES);
												SetDlgItemText(hDlg,ID6_FILE,(LPSTR)str);
												strcpy(FileName,str);
												break;
									 case LBN_DBLCLK:
												DlgDirSelect(hDlg,str,ID6_FILES);
												SetDlgItemText(hDlg,ID6_FILE,(LPSTR)str);
												strcpy(FileName,str);
												PostMessage(hDlg,WM_COMMAND,IDOK,NULL);
												break;
								}
								break;
					}
					break;
	}
	return (FALSE);
}//DateiLaden

