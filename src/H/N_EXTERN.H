extern	HMENU			hBrainMenu;
extern	HMENU			hMainMenu;
extern  BOOL			bMoveMasterInit;
extern  BOOL			bVideomatInit;
extern	BOOL			bBrain;
extern	BOOL			bBefehlsliste;
extern	BOOL			bVideoSteuerung;
extern	BOOL			bVisualisierung;
extern	BOOL			bSteuerung;
extern	BOOL			bRepeatmodus;
extern	BOOL			bEinzelschrittmodus;
extern	BOOL			bNetzdefine;
extern	BOOL			bSpeichernAbfrage;
extern	BOOL			bSpeichern;
extern	HWND			hWndNeurorob;
extern	HWND			hWndBrain;
extern	HWND			hDlgBefehlsliste;
extern	HWND			hDlgVisualisierung;
extern	HWND			hDlgSteuerung;
extern	HWND			hDlgVideoSteuerung;
extern  FARPROC		lpBefehlsliste;
extern  FARPROC		lpVisualisierung;
extern  FARPROC		lpSteuerung;
extern  FARPROC		lpVideoSteuerung;
extern	FARPROC		lpProcLaden;
extern	DCB				CommDCB;
extern	int	  		nCid;
extern	int				nCid2;
extern  char			dummy[];
extern  char			dummy1[];
extern  char			dummy2[];
extern	int				ndummy;
extern	HANDLE		hInst;
extern	HBITMAP		hRobBitmap,
									hOldBitmap,
									hFaceBitmap,
									hAllBitmap;
extern	HCURSOR		hHourGlass;
extern	HCURSOR		hSivips;
extern	HCURSOR		hMaster;
extern	int				roboterspeed;
extern	int				roboterstep;
extern	int				robotergrid;
extern	int				roboterdegree;
//			Folgende globale Variable kommen von OpenDlg (SDK-Exsample)

extern	char FileName[];
extern	char PathName[];
extern	char OpenName[];
extern	char DefPath[];
extern	char DefSpec[];
extern	char DefExt[];
extern	char str[];


extern  LPSTR			szFileCaption;

extern  HPEN hRotPen1;
extern  HPEN hBlauPen1;
extern  HPEN hGruenPen1;
extern  HPEN hLilaPen1;
extern  HPEN hSchwarzPen1;
extern  HPEN hGelbPen1;
extern  HPEN hHellblauPen1;
extern  HPEN hGrauPen1;

extern  HBITMAP hGehirnBitmap;

extern  HBRUSH hRotBrush1;
extern  HBRUSH hBlauBrush1;
extern  HBRUSH hGruenBrush1;
extern  HBRUSH hLilaBrush1;
extern  HBRUSH hSchwarzBrush1;
extern  HBRUSH hGelbBrush1;
extern  HBRUSH hHellblauBrush1;
extern  HBRUSH hGrauBrush1;

extern	double xPart;
extern	double yPart;
extern	double xCent;
extern	double yCent;
extern	double xUrsprung;
extern	double yUrsprung;

