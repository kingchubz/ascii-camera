#include <windows.h>
#include <stdio.h>
#include <wchar.h>

#include "escapi.h"

#define WND_WIDTH 170
#define WND_HEIGHT 44

union RGBint
{
    int iPixel;
    unsigned char c[4];
};

constexpr char tile[] =
{
    '.','-','+','$','#'
};

char getChar(RGBint clr,float gamma){
    char c = '#';

    float brightness;

    brightness = clr.c[0] + clr.c[1] + clr.c[2];
    brightness /= 255;
    brightness += gamma;

    if(brightness <= 0.20)
        c = tile[0];
        //c = tile[4];
    else if(brightness <= 0.4)
        c = tile[1];
        //c = tile[3];
    else if(brightness <= 0.6)
        c = tile[2];
        //c = tile[2];
    else if(brightness <= 0.8)
        c = tile[3];
        //c = tile[1];
    else if(brightness <= 1)
        c = tile[4];
        //c = tile[0];

    return c;
}



int main()
{
    int nCam = setupESCAPI();

    if(nCam == 0)
        return -1;

    SimpleCapParams params;

    params.mWidth = WND_WIDTH;
    params.mHeight = WND_HEIGHT;
    params.mTargetBuf = new int[WND_WIDTH*WND_HEIGHT];

    if(initCapture(0, &params) == 0)
        return -1;

    HANDLE cHandle = CreateConsoleScreenBuffer(
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              NULL,
                              CONSOLE_TEXTMODE_BUFFER,
                              NULL
                              );

    SetConsoleActiveScreenBuffer(cHandle);

    char* screen = new char[WND_WIDTH*WND_HEIGHT];

    RGBint clr;
    float gamma = 0;

    while(!GetAsyncKeyState(VK_ESCAPE))
    {
        doCapture(0);
        while(!isCaptureDone(0));

        for(int i = 0; i < WND_WIDTH; i++)
        {
            for(int j = 0; j < WND_HEIGHT; j++)
            {
                clr.iPixel = params.mTargetBuf[i*WND_HEIGHT+j];
                screen[i*WND_HEIGHT+j] = getChar(clr,gamma);
            }
        }

        screen[WND_WIDTH*WND_HEIGHT-1] = '\0';

        DWORD lpNumberOfCharsWritten;

        WriteConsoleOutputCharacter(cHandle,screen,WND_WIDTH*WND_HEIGHT,{0,0}, &lpNumberOfCharsWritten);

        if(GetAsyncKeyState(VK_UP))
            gamma += 0.1;
        else if(GetAsyncKeyState(VK_DOWN))
            gamma -= 0.1;
    }

    deinitCapture(0);

    return 0;
}
