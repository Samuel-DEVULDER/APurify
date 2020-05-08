#include <dos/dos.h>
#include <intuition/intuition.h>

#ifdef __GNUC__
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void itoa(char *rep,ULONG u)
{
	BYTE i=2;

	while ((i--) || u)
	{
		rep[i] = '0' + u%10L;
		u /=10L;
	}
}

void Failat(char * s)
{
	CPTR con;
	if (con = Open("CON:0/10/400/50/Clock Requester .../AUTO/CLOSE/WAIT/INACTIVE",MODE_NEWFILE))
	{
		/*FPrintf(con,s);*/
		Write(con,s,strlen(s));
	}
}

struct Library *DosBase;
struct IntuitionBase *IntuitionBase;

int main(void)
{
	struct NewWindow newwindow=
	{
		500,0,100,11,
		0,1,
		IDCMP_CLOSEWINDOW,
		WFLG_CLOSEGADGET | WFLG_DRAGBAR | WFLG_SMART_REFRESH,
		NULL,NULL,
		"00:00:00",
		NULL,
		NULL,
		0,0,0,0,
		WBENCHSCREEN
	};
	struct Window *window;
	struct IntuiMessage *message;
	char *title;
	UBYTE cont = 1;
	ULONG micros,seconds;
	char hours[2],mins[2],secs[2];


fgetc(stdin);

	if (DosBase = OpenLibrary("dos.library",36L))
	{
		if (IntuitionBase = OpenLibrary("intuition.library",36L))
		{
			if (window = OpenWindow(&newwindow))
			{
				title = window->Title;
				while ((message = (struct IntuiMessage *)(GetMsg(window->UserPort))) || cont)
				{
					if (message)
					{
						ReplyMsg((struct Message *)message);
						cont=0;
					}
					Delay(50);
					CurrentTime(&seconds,&micros);
					itoa(hours,((seconds / 3600L)%24L));
					itoa(mins,((seconds / 60L)%60L));
					itoa(secs,(seconds%60L));
					title[0]=hours[0];
					title[1]=hours[1];
					title[3]=mins[0];
					title[4]=mins[1];
					title[6]=secs[0];
					title[7]=secs[1];
					RefreshWindowFrame(window);
				}
				CloseWindow(window);
			}
			else Failat("Can't open window Sorry !!!\n");
			CloseLibrary(IntuitionBase);
		}
		CloseLibrary(DosBase);
	}
	return 0;
}                                                                 
