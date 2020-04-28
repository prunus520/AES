/*
 *     檔名： frdsgn.h
 *     全名： front design
 *     作者： Cerasus
 *     描述： 
 */

#ifndef _FRDSGN_H_ 
#define _FRDSGN_H_

#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <direct.h>
#include <time.h>

extern bool keyFlag;


/********************  宣告函式  ********************/ 


/********************
 *  視窗與事件設定 
 ********************/
void setWindows(const char* title, int W, int H, int X, int Y);
void setConsoleSize(int W, int H);

/********************
 *  光標設定
 ********************/
void setCursorXY(int x, int y);
COORD getCursorXY();
void setCursorInfo(int dwSize, int bVisible);

/********************
 *  文字設定與美化
 ********************/
void setTextColor(int color);
void textHR(int sum = 70, bool blank = true);
void textCenter(const char* text, int color = 0x0F, int buffer = 71, bool blank = true);
void textCenterf(const char* text, ...);
void textCenterf(int color, const char* text, ...);
bool getchars(char* buf, int count, int mode = 0);
void cls(int from = 19, int to = 47);
void border(int from, int to);

/********************
 *  檔案與密碼處理 
 ********************/
bool inputBox(const char* name, const char** in, unsigned short size, int mode = 0);
void setpws(const char* name, const char** pw, unsigned short size, int mode = 0);
bool targetFile(const char* name, const char** pw, unsigned short size);
void splitFile(const char* tpath, const char* path, const char* file);
bool delFile(const char* path);
void delBox(const char* path);
void box(int W, int H, int X, int Y);
void perbar(long completed, long complete, bool crypt);
bool rearKeyEvent(void);
void pageMenu(const char* name, const char* option[], int count, int num);

#endif
