#include <stdio.h>
#include <stdlib.h>
#include "color.h"

#ifdef WIN32
#include <windows.h>
static HANDLE hConsole = NULL;
static WORD hConsoleDefAttr = 0;
#endif

//#define USE_ARCHAIC_TERM

// System specific color values
#ifdef WIN32
#define BLACK       0
#define RED         (FOREGROUND_RED)
#define GREEN       (FOREGROUND_GREEN)
#define YELLOW      (FOREGROUND_RED | FOREGROUND_GREEN)
#define BLUE        (FOREGROUND_BLUE)
#define MAGENTA     (FOREGROUND_BLUE | FOREGROUND_RED)
#define CYAN        (FOREGROUND_BLUE | FOREGROUND_GREEN)
#define WHITE       (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED)
#else
#define BLACK       0
#define RED         1
#define GREEN       2
#define YELLOW      3
#define BLUE        4
#define MAGENTA     5
#define CYAN        6
#define WHITE       7

#define NORMAL      0
#define BRIGHT      1
#define DIM         2
#define ITALIC      3
#define UNDERLINE   4
#define BLINK_SLOW  5
#define BLINK_FAST  6
#define REVERSE     7
#define HIDDEN      8
#endif

static int GetSysConsoleColor(int nColor)
{
	int nSysColor = 0;
	switch(nColor)
	{
	case CN_BLACK:
		nSysColor = BLACK;
		break;
	case CN_BLUE:
		nSysColor = BLUE;
		break;
	case CN_GREEN:
		nSysColor = GREEN;
		break;
	case CN_CYAN:
		nSysColor = CYAN;
		break;
	case CN_RED:
		nSysColor = RED;
		break;
	case CN_MAGENTA:
		nSysColor = MAGENTA;
		break;
	case CN_YELLOW:
		nSysColor = YELLOW;
		break;
	case CN_WHITE:
		nSysColor = WHITE;
		break;
	default:
		nSysColor = 0;
	}
	return nSysColor;
}

void textcolor(int attr/*=CN_NORMAL*/, int fg/*=CN_NO_COLOR*/, int bg/*=CN_NO_COLOR*/)
{
#ifdef WIN32
	int fg_win = -1;
	int bg_win = -1;
	bool bReverse = (attr & CN_REVERSE)>0;
	bool bForegroundIntense = ( (fg>0 && (fg & CN_BRIGHT)) || (attr & CN_BOLD));
	bool bBackgroundIntense = bg>7;

	WORD hConsoleAttr = 0;
	if (hConsole==NULL)
	{
		// Get default colors
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
		hConsoleDefAttr = consoleInfo.wAttributes;
	}
	if (attr==CN_NORMAL && fg<0 && bg<0)
		SetConsoleTextAttribute(hConsole, hConsoleDefAttr);

	// Set intended colors
	if (fg>=0)
		fg_win = GetSysConsoleColor(fg & 0x7);
	else
		fg_win = hConsoleDefAttr & 0x7;

	if (bg>=0)
		bg_win = GetSysConsoleColor(bg & 0x7);
	else
		bg_win = (hConsoleDefAttr & 0x70)>>4;

	// Set intensity
	if (bForegroundIntense) fg_win |= 8;
	if (bBackgroundIntense) bg_win |= 8;

	// Reverse if needed
	if (attr & CN_REVERSE)
	{
		int tmp = fg_win;
		fg_win = bg_win;
		bg_win = tmp;
	}

	// Set the colors
	hConsoleAttr = fg_win | (bg_win<<4);
	SetConsoleTextAttribute(hConsole, hConsoleAttr); //FOREGROUND_BLUE
#else
	int attr_lin = 0;
	int fg_lin = -1;
	int bg_lin = -1;
	if (fg>=0) fg_lin = GetSysConsoleColor(fg & 0x7);
	if (bg>=0) bg_lin = GetSysConsoleColor(bg & 0x7);

	if (attr & CN_BOLD) attr_lin |= BRIGHT;
	if (attr & CN_UNDERLINE) attr_lin |= UNDERLINE;
	if (attr & CN_REVERSE) attr_lin |= REVERSE;

#ifdef USE_ARCHAIC_TERM
	if (bg>0 && (bg & CN_BRIGHT))
	{
	    if (fg>0 && (fg & CN_BRIGHT)) fg_lin += 60;
	    bg_lin += 60;
	    //if (bg>0 && (bg & CN_BRIGHT)) bg_lin += 60;
	}
	else
	{
		if (fg>0 && (fg & CN_BRIGHT)) attr_lin |= BRIGHT;
	}
#else
	if (fg>0 && (fg & CN_BRIGHT)) fg_lin += 60;
	if (bg>0 && (bg & CN_BRIGHT)) bg_lin += 60;
#endif
	char command[23];
	if (attr == CN_NORMAL && fg<0 && bg<0)
		sprintf(command, "%c[0m", 0x1B);
	else
	{
		if (fg<0 && bg<0)
			sprintf(command, "%c[%dm", 0x1B, attr_lin);
		else if (fg>0 && bg<0)
			sprintf(command, "%c[%d;%dm", 0x1B, attr_lin, fg_lin + 30 );
		else if (fg<0 && bg>0)
			sprintf(command, "%c[%d;%dm", 0x1B, attr_lin, bg_lin + 40 );
		else
			sprintf(command, "%c[%d;%d;%dm", 0x1B, attr_lin, fg_lin + 30, bg_lin + 40 );
#ifdef USE_256_COLORS
		if (bg<0)
			sprintf(command, "%c[38;5;%dm", 0x1B, fg_lin);
		else
			sprintf(command, "%c[38;5;%dm%c[48;5;%dm", 0x1B, fg_lin, 0x1B, bg_lin);
#endif
	}
	printf("%s", command);
#endif
	fflush(stdout);
}

void textcolor_show()
{
	textcolor( CN_BOLD );
	printf("BOLD");
	textcolor();
	printf("\n");

	textcolor( CN_UNDERLINE );
	printf("UNDERLINED");
	textcolor();
	printf("\n");

	textcolor( CN_REVERSE );
	printf("REVERSED");
	textcolor();
	printf("\n\n");

	int i,j,t;
	for (t=0; t<2; t++)
	{
		for (i=0; i<8; i++)
		{
			if (t==0) textcolor( CN_NORMAL, i );
	        if (t==1) textcolor( CN_NORMAL, i | CN_BRIGHT );

			printf("%d|%d", i, t&1);
			textcolor();
			printf(" ");
		}
		printf("\n");
	}
	printf("\n");

	for (t=0; t<2; t++)
	{
		for (i=0; i<8; i++)
		{
			if (t==0) textcolor( CN_REVERSE, i );
	        if (t==1) textcolor( CN_REVERSE, i | CN_BRIGHT );

			printf("%d|%d", i, t&1);
			textcolor();
			printf(" ");
		}
		printf("\n");
	}
	printf("\n");

	for (t=0; t<4; t++)
	for (i=0; i<8; i++)
	{
		for (j=0; j<8; j++)
		{
			if (t==0) textcolor(CN_NORMAL, i , j );
			if (t==1) textcolor(CN_NORMAL, i | CN_BRIGHT, j );
			if (t==2) textcolor(CN_NORMAL, i , j | CN_BRIGHT);
			if (t==3) textcolor(CN_NORMAL, i | CN_BRIGHT, j | CN_BRIGHT);

			printf("%d|%d|%d|%d", i, j, t&1, (t&2));
			textcolor();
			printf(" ");
		}
		printf("\n");
	}
	printf("\n");
}
