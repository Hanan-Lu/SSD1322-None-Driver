#ifndef _ASCII_FONT_H__
#define _ASCII_FONT_H__

typedef enum{
    ARIAL_8X16=0,
    CONSOLAS_12X21,
    GADUGI_16X16,
    SOURCECODEPRO_16X24,
    CONSOLAS_8X16,
    INCONSOLATA_8X11,
    INVALID_FONT,
}ASCIIFont;

typedef struct 
{
    ASCIIFont font;
    unsigned char const *baseAddr;
    unsigned char width;
    unsigned char high;
}ASCIIFontStruct;

//#define ASCIIFONTPARAM(font, baseAddr, width, high) {font, baseAddr, width, high};

unsigned char const* getFontptr(ASCIIFont font, unsigned char ch);
unsigned char getFontWidth(ASCIIFont font);
unsigned char getFontHigh(ASCIIFont font);
unsigned char getFontByteNum(ASCIIFont font);
#endif
