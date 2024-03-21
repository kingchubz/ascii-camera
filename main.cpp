#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <iostream>

#include "escapi.h"

union RGBint
{
    int iPixel;
    unsigned char c[4];
};

constexpr char tile[] =
{
    '.','-','+','$','#'
};

struct win_dimentions {
    int w;
    int h;
};

// Determines character depending on brightness of a pixel
char getChar(RGBint clr,float gamma){
    char c = '#';

    float brightness;

    brightness = (clr.c[0] + clr.c[1] + clr.c[2]) / 3;
    brightness /= 255;
    brightness += gamma;

    if(brightness <= 0.20)
        c = tile[0];
    else if(brightness <= 0.4)
        c = tile[1];
    else if(brightness <= 0.6)
        c = tile[2];
    else if(brightness <= 0.8)
        c = tile[3];
    else if(brightness <= 1)
        c = tile[4];

    return c;
}

win_dimentions get_win_dimentions(HANDLE cHandle) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    win_dimentions w_dim;

    GetConsoleScreenBufferInfo(cHandle, &csbi);

    w_dim.w = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    w_dim.h = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    return w_dim;
}

int main()
{
    int nCam = setupESCAPI();

    if(nCam == 0) {
        std::cout << "Could not initialize camera\n";
        return -1;
    }


    HANDLE cHandle = CreateConsoleScreenBuffer(
                              GENERIC_READ | GENERIC_WRITE,
                              0,
                              NULL,
                              CONSOLE_TEXTMODE_BUFFER,
                              NULL
                              );

    SetConsoleActiveScreenBuffer(cHandle);

    const win_dimentions W_DIM = get_win_dimentions(cHandle);

    SimpleCapParams params;

    params.mWidth = W_DIM.w;
    params.mHeight = W_DIM.h;
    params.mTargetBuf = new int[W_DIM.w*W_DIM.h];

    if(initCapture(0, &params) == 0){
        std::cout << "Found camera, but could not initialize capture\n";
        return -2;
    }


    // Console screen as array of characters
    char* screen = new char[W_DIM.w*W_DIM.h];

    RGBint clr;
    float gamma = 0;

    while(!GetAsyncKeyState(VK_ESCAPE))
    {
        doCapture(0);
        while(!isCaptureDone(0));

        for(int i = 0; i < W_DIM.w; i++)
        {
            for(int j = 0; j < W_DIM.h; j++)
            {
                clr.iPixel = params.mTargetBuf[i*W_DIM.h+j];
                screen[i*W_DIM.h+j] = getChar(clr,gamma);
            }
        }

        screen[W_DIM.w*W_DIM.h-1] = '\0';

        DWORD lpNumberOfCharsWritten;

        WriteConsoleOutputCharacter(cHandle,screen,W_DIM.w*W_DIM.h,{0,0}, &lpNumberOfCharsWritten);

        if(GetAsyncKeyState(VK_UP))
            gamma += 0.1;
        else if(GetAsyncKeyState(VK_DOWN))
            gamma -= 0.1;
    }

    deinitCapture(0);

    return 0;
}
