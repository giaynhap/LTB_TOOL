#pragma once
#include <shlobj.h>
#include "gnLoadLtb.h"
#include "gnLoadWmdl.h"
#include "TgaHeader.h"
#define GL_VERTEX_ARRAY  
#include <gl\GL.h>

#include <iostream>;
#include <windows.h>
#include <Commdlg.h>
#include <CommCtrl.h>
#include <string>
#include "ImageLoad.h"
#include "SmdSave.h"

#pragma comment(lib,"comctl32.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"")

HMENU hMenu, subFile, subTool, helpMenu;

HWND hWndEdit, hWndEdit2, track_bar,hWndCheckBox,hwndCombox2,hwndListMesh, hwndListSubMesh;
HWND hwndResize, hwndconvert;
wchar_t openFileName[1024];
HWND hWndListAnim, hWndbLoadModel, hWndbLoadText,hwndWepnView;
HWND hWndMain, hWndOpenGL, subWindow;
HDC g_HDC;
bool isShowBone = true;
bool done;
bool isViewMode = false;
int modeShowModel = 0;
int mFpscontrolValue = 20;
static int window;
static int value = 0;

 
GLfloat gRotatef[] = { 0.0,0.0,0.0 };
GLfloat eyse_postion[] = { 150.0,0.0,0.0 };
GLfloat eyse_lookat[] = { 0.0,0.0,0.0 };
GLfloat OldgAngle[] = { 0.0,0.4,0.0 };

  

int mfirstPoint[2];
bool IsCLick = false;
# define M_PIl          3.141592653589793238462643383279502884L 

int SkyBoxIndex[6];
Model *LtbFile;
int m_meshSelect = 0;
int m_subMeshSelect = 0;

HMENU  ADDPOPUPMENU(HMENU hmenu, char * string) {
	HMENU hSubMenu = CreatePopupMenu();
	AppendMenu(hmenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, (LPCSTR)string);
	return hSubMenu;
}
void msgConvertImage(HWND parents = NULL );
void msgAnimShow(HWND parents = NULL);
#define ADDMENUITEM(hmenu, ID, string) \
	 AppendMenu(hmenu, MF_STRING, ID, string);
enum
{
	ID_FILE_LOAD_LTB,
	ID_FILE_LOAD_TEXTURES,
	ID_FILE_EXIT,
	ID_EDIT,
	ID_TRACKBAR,
	ID_COMBOBOX,
	ID_CHECKBOX,
	ID_LOADTEXT,
	ID_TOOL_SMD,
	ID_TOOL_TEX,
	ID_EX_MESH,
	ID_EX_ANIM,
	ID_EX_PROCESS,
	ID_CV_RESIZE,
	ID_TOOL_UNZIP,
	ID_TOOL_STUDIO_MDL,
	ID_TOOL_ABOUT,
	ID_WPN_VIEW
};

const wchar_t *GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

void load_sky_box()
{
	int uiVAO;

	int Sizex, Sizey;
	for (int i = 0; i < 6; i++)
		SkyBoxIndex[i] = TEX_GenerateID();

	LoadTgaImage("SkyBox/de_stormft.tga", SkyBoxIndex[0], &Sizex, &Sizey);
	LoadTgaImage("SkyBox/de_stormrt.tga", SkyBoxIndex[2], &Sizex, &Sizey);
	//printf(">> size : %d", SkyBoxIndex[3]);
	LoadTgaImage("SkyBox/de_stormlf.tga", SkyBoxIndex[3], &Sizex, &Sizey);

	LoadTgaImage("SkyBox/de_stormbk.tga", SkyBoxIndex[1], &Sizex, &Sizey);

	LoadTgaImage("SkyBox/de_stormup.tga", SkyBoxIndex[4], &Sizex, &Sizey);
	LoadTgaImage("SkyBox/de_stormdn.tga", SkyBoxIndex[5], &Sizex, &Sizey);


}


void SetupPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0 };
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}
void square(void) {

	glPushMatrix();

	glTranslatef(0, -90, 0);
	glScalef(200, 200, 200);
	glRotatef(90, 1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f(-1, -1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-1, 1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(1, 1, 0);
	glTexCoord2f(0, 0);
	glVertex3f(1, -1, 0);
	glEnd();
	glPopMatrix();
}
void initialise()
{


	FreeImage_Initialise();
	glEnable(GL_TEXTURE_2D);
	load_sky_box();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(90.0, 4.0 / 3.0, 1.0, 10000.0);
}

void updateConboxListAnim()
{
 
	SendMessage(hWndListAnim, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

	if (LtbFile == NULL) return;
	for (int k = 0; k < LtbFile->m_nAnim; k++)
	{
		SendMessageA(hWndListAnim, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)LtbFile->m_Anim[k].name);
	}
	SendMessageA(hWndListAnim, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	
}


void updateComboxLisSubMesh(int index)
{
	char name[255];
	SendMessage(hwndListSubMesh, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	memset(&name, 0, 255);
	if (LtbFile == NULL) return;
	for (int k = 0; k < LtbFile->m_Mesh[index].m_nSubMesh; k++)
	{
		sprintf(name, "SubMesh %d", k);
		SendMessageA(hwndListSubMesh, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)name);
	 
	}
	SendMessageA(hwndListSubMesh, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	m_meshSelect = 0;
}
void updateComboxLisMesh()
{
	 
	SendMessage(hwndListMesh, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
 
	if (LtbFile == NULL) return;
	for (int k = 0; k <LtbFile->m_nMesh; k++)
	{
		SendMessageA(hwndListMesh, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)LtbFile->m_Mesh[k].name);
	}
	SendMessageA(hwndListMesh, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	updateComboxLisSubMesh(0);
	m_subMeshSelect = 0;
}

void ExePath(char * path) {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);

	for (int k =strlen(buffer) - 1; k>0; k--)
		if (buffer[k] == '\\')
		{
			
			strncpy(path, buffer ,k+1);
			break;
		}
	  
}



void convertDtxToBmp(char *szFileName,char *dstFile,bool isRescan)
{
	static unsigned char pBuffer[1024 * 1024 * 4];
	int pWidth, pHeight;
	LoadDTXImage(szFileName,NULL,& pWidth,& pHeight, pBuffer);
	FIBITMAP *dst = FreeImage_ConvertFromRawBits(pBuffer, pWidth, pHeight, 4 * pWidth, 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	FreeImage_FlipVertical(dst);

	FIBITMAP* temp = FreeImage_ColorQuantizeEx(dst, FIQ_WUQUANT, 256, 0, (RGBQUAD *)1);
	int w = FreeImage_GetWidth(temp);
	int h = FreeImage_GetHeight(temp);
	if (isRescan)
	if (w>512 || h>512)
		temp = FreeImage_Rescale(temp, 512, 512);

	FreeImage_Save(FIF_BMP, temp, dstFile, BMP_DEFAULT);

	FreeImage_Unload(temp);
}


void doExeFile(char* exeFile , char * command,char *workingdir)
{
	SHELLEXECUTEINFOA ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = exeFile;
	ShExecInfo.lpParameters = command;
 
	ShExecInfo.lpDirectory = workingdir;
	 
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteExA(&ShExecInfo);
}