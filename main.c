/****************************************************************
Ilitek ILI9341 Color TFT LCD controller demo
(C) Copyright 2011-2013 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

This program can't be compiled with the Evaluation or
Standard versions of CodeVisionAVR, because the
glcd_ili9341.lib file is supplied only with the
Advanced license.
In this case you can program the ATxmega128A1 FLASH with
the contents of the
\EXAMPLES\Graphic LCD\ILI9341\64k Colors\Exe\demo_fft24_64k.hex
file.

The demo is designed to run on the Atmel XMEGA-A1 Xplained
development board with an ATxmega128A1
chip using the internal 32 MHz oscillator
as clock source.

A type NHD-2.4-240320SF-CTXI-F1 240x320 2.4" TFT LCD
module from www.newhavendisplay.com
is used for the purpose of this demo.
It can be replaced with other similar modules with
the ILI9341 controller.

In this demo the controller is operated in 64k color mode
using a 8 bit interface .

The TFT LCD module connections are:
DB0  - GND
DB1  - GND
DB2  - GND
DB3  - GND
DB4  - GND
DB5  - GND
DB6  - GND
DB7  - GND

DB8  - PORTA bit 0
DB9  - PORTA bit 1
DB10 - PORTA bit 2
DB11 - PORTA bit 3
DB12 - PORTA bit 4
DB13 - PORTA bit 5
DB14 - PORTA bit 6
DB15 - PORTA bit 7

/CS  - PORTD bit 0
RS (D/C) - PORTD bit 1
/RD  - PORTD bit 2
/WR  - PORTD bit 3
/RST - PORTD bit 4

IM0  - +3.3V Use 8 bit interface mode

Backlight connections:
LED-A  - +3.3V
LED-K1 - connected to GND using a 470 ohm resistor
LED-K2 - connected to GND using a 470 ohm resistor
LED-K3 - connected to GND using a 470 ohm resistor
LED-K4 - connected to GND using a 470 ohm resistor

Power supply connections:
GND   - GND
VDD   - +3.3V
IOVDD - +3.3V

If you are using a different LCD module,
please use the connections from the module datasheet.

The connections can be modified by accessing the
Project|Configure|C Compiler|Libraries|Graphic LCD menu.
****************************************************************/

#include <io.h>
#include <glcd.h>
#include <delay.h>

/* Meter functions */
#include "meter.h"
/* Bar-graph gauge functions */
#include "bargraph.h"
/* Include some fonts needed by the demo */
#include <font5x7.h>
#include "arial14.h"
#include "arial_bold14.h"
#include "courier7x14.h"
#include "courier_bold14.h"
#include "la_bamba17.h"
#include <arial_10.h>   //����������� �����
/* Button functions */
#include "button.h"
/* Glyphs for buttons */
#include "ok_glyph.h"
#include "cancel_glyph.h"

/* Small dots filling pattern */
flash unsigned char small_dots[]={0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55};

/* Solid filling pattern */
flash unsigned char solid_fill[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

/* Hexagon point coordinates
   The last point's coordinates must match
   the first point coordinates,
   so that the polygon will be closed */
flash GLCDPOINT_t hexagon[7]=
{
{40,0},
{57,10},
{57,30},
{40,40},
{23,30},
{23,10},
{40,0}
};

flash char * flash button_state[2]=
      {"Buttons not pressed",
       "Buttons pressed     "};

typedef struct
      {
      flash char *name;
      GLCDCOL_t col;
      } color_data;

flash color_data colors[]=
{
"Black",GLCD_CL_BLACK,
"White",GLCD_CL_WHITE,
"Gray",GLCD_CL_GRAY,
"Light Gray",GLCD_CL_LIGHT_GRAY,
"Green",GLCD_CL_GREEN,
"Lime",GLCD_CL_LIME,
"Blue",GLCD_CL_BLUE,
"Red",GLCD_CL_RED,
"Aqua",GLCD_CL_AQUA,
"Yellow",GLCD_CL_YELLOW,
"Magenta",GLCD_CL_MAGENTA,
"Cyan",GLCD_CL_CYAN,
"Dark Cyan",GLCD_CL_DARK_CYAN,
"Orange",GLCD_CL_ORANGE,
"Pink",GLCD_CL_PINK,
"Brown",GLCD_CL_BROWN,
"Violet",GLCD_CL_VIOLET,
"Silver",GLCD_CL_SILVER,
"Gold",GLCD_CL_GOLD,
"Navy",GLCD_CL_NAVY,
"Maroon",GLCD_CL_MAROON,
"Purple",GLCD_CL_PURPLE,
"Olive",GLCD_CL_OLIVE
};

/* Buffer used for image storage */
unsigned char buffer[3100];

// System Clocks initialization
void system_clocks_init(void)
{
unsigned char n,s;

// Optimize for speed
#pragma optsize-
// Save interrupts enabled/disabled state
s=SREG;
// Disable interrupts
#asm("cli")


// Restore interrupts enabled/disabled state
SREG=s;
// Restore optimization for size if needed
#pragma optsize_default
}

void main()
{
GLCDINIT_t glcd_init_data;
unsigned char i;
int x,y;
meter_t meter;
bargraph_t bargraph;
button_t button_ok,button_cancel;

/* Initialize the LCD controller with the default values
   from glcd_ili9341.h */

/* Specify the current font for displaying text */
glcd_init_data.font=font5x7;
/* No function is used for reading image data from external memory */
glcd_init_data.readxmem=NULL;
/* No function is used for writing image data to external memory */
glcd_init_data.writexmem=NULL;
/* Horizontal display reversed */
glcd_init_data.reverse_x=ILI9341_REVX_REV;
/* Vertical display not reversed */
glcd_init_data.reverse_y=ILI9341_REVY_NORM;
/* Color bit writing order: BGR */
glcd_init_data.cl_bits_order=ILI9341_CL_BITS_BGR;
/* Use default value for pump ratio */
glcd_init_data.pump_ratio=ILI9341_DEFAULT_PUMP_RATIO;
/* Use default value for DDVDH enhanced mode */
glcd_init_data.ddvdh_enh_mode=ILI9341_DEFAULT_DDVDH_ENH_MODE;
/* Use default value for CR timing */
glcd_init_data.cr_timing=ILI9341_DEFAULT_CR_TIMING;
/* Use default value for EQ timing */
glcd_init_data.cr_timing=ILI9341_DEFAULT_EQ_TIMING;
/* Use default value for precharge_timing */
glcd_init_data.precharge_timing=ILI9341_DEFAULT_PRECHARGE_TIMING;
/* Use default value for VRH */
glcd_init_data.vrh=ILI9341_DEFAULT_VRH;
/* Use default value for VCOML */
glcd_init_data.vcoml=ILI9341_DEFAULT_VCOML;
/* Use default value for VCOMH */
glcd_init_data.vcomh=ILI9341_DEFAULT_VCOMH;
/* Use default value for VCOM offset */
glcd_init_data.vcom_offset=ILI9341_DEFAULT_VCOM_OFFSET;
/* LCD type normally white */
glcd_init_data.lcd_type=ILI9341_LCD_TYPE_WHITE;
/* LCD scan mode: interlaced */
glcd_init_data.scan_mode=ILI9341_SCAN_INTERLACED;
/* Frame rate: 70Hz */
glcd_init_data.frame_rate=ILI9341_FRAME_RATE_70;

glcd_init(&glcd_init_data);

/* Use transparent displaying mode for text */
glcd_transparent(true);


///* Display literal char string located in FLASH */
//glcd_outtextf("Ilitek ILI9341 Graphics Demo\n"
//              "Here are some font samples:");
//
///* Display some font samples */
//glcd_setcolor(GLCD_CL_YELLOW);
//glcd_outtextxyf(0,20,"Standard, 5x7 pixels");
//
//glcd_setfont(arial14);
//glcd_setcolor(GLCD_CL_RED);
//glcd_outtextxyf(0,30,"Arial, 14 pixels height\n");
//
//glcd_setfont(arial_bold14);
//glcd_setcolor(GLCD_CL_MAGENTA);
//glcd_outtextf("Arial Bold, 14 pixels height\n");
//
//glcd_setfont(courier7x14);
//glcd_setcolor(GLCD_CL_GREEN);
//glcd_outtextf("Courier, 7x14 pixels\n");
//
//glcd_setfont(courier_bold14);
//glcd_setcolor(GLCD_CL_LIME);
//glcd_outtextf("Courier Bold,14 pixels height\n");
//
////glcd_setfont(la_bamba17);
////glcd_setcolor(GLCD_CL_AQUA);
////glcd_outtextf("La Bamba,17 pixels height\n");
//
//glcd_setfont(arial);
//glcd_setcolor(GLCD_CL_YELLOW);
//glcd_outtextf("Arial_10, 19-pixel HEIGHT\n");
//
//
//glcd_setfont(arial_bold14);
//glcd_setcolor(GLCD_CL_CYAN);
//glcd_outtextf("\nVariable text spacing:\n");
//
//glcd_setfont(courier_bold14);
//glcd_setcolor(GLCD_CL_YELLOW);
//glcd_outtextf("ILI9341 TFT Controller\n");
///* Set horizontal justification: 2 pixels and
//   vertical justification: 1 pixel */
//glcd_settextjustify(2,1);
//glcd_outtextf("ILI9341 TFT Controller\n");
///* Set horizontal justification: 3 pixels and
//   vertical justification: 1 pixel */
//glcd_settextjustify(3,1);
//glcd_outtextf("ILI9341 TFT Controller");
//
///* Set horizontal justification: 1 pixels and
//   vertical justification: 1 pixel */
//glcd_settextjustify(1,1);
//glcd_setfont(arial_bold14);
//glcd_setcolor(GLCD_CL_CYAN);
//glcd_outtextxyf(0,190,"Line styles:");
//
///* Draw various styles of lines */
//glcd_setcolor(GLCD_CL_ORANGE);
///* Line thickness: 1 pixel */
//glcd_line(0,210,239,210);
//
//glcd_setlinestyle(1,GLCD_LINE_DOT_SMALL);
//glcd_line(0,215,239,215);
//
//glcd_setlinestyle(1,GLCD_LINE_DOT_LARGE);
//glcd_line(0,220,239,220);
//
///* Line thickness: 2 pixels */
//glcd_setlinestyle(2,GLCD_LINE_SOLID);
//glcd_line(0,225,239,225);
//
//glcd_setlinestyle(2,GLCD_LINE_DOT_SMALL);
//glcd_line(0,230,239,230);
//
//glcd_setlinestyle(2,GLCD_LINE_DOT_LARGE);
//glcd_line(0,235,239,235);
//
///* Line thickness: 4 pixels */
//glcd_setlinestyle(4,GLCD_LINE_SOLID);
//glcd_line(0,240,239,240);
//
//glcd_setlinestyle(4,GLCD_LINE_DOT_SMALL);
//glcd_line(0,245,239,245);
//
//glcd_setlinestyle(4,GLCD_LINE_DOT_LARGE);
//glcd_line(0,250,239,250);
//
//glcd_setcolor(GLCD_CL_CYAN);
//glcd_outtextxyf(0,260,"Coloured lines:");
//
///* Draw some coloured lines */
//glcd_setlinestyle(1,GLCD_LINE_SOLID);
//i=1; /* Don't use black color for lines */
//for (x=0; x<240; x+=4)
//    {
//    glcd_setcolor(colors[i++].col);
//    if (i>=(sizeof(colors)/sizeof(color_data))) i=1;
//    glcd_line(0,275,x,319);
//    }
//
//i=1;
//for (x=239; x>=0; x-=4)
//    {
//    glcd_setcolor(colors[i++].col);
//    if (i>=(sizeof(colors)/sizeof(color_data))) i=1;
//    glcd_line(239,319,x,275);
//    }
//
///* 5 seconds delay */
//delay_ms(5000);
///* Clear display */
//glcd_clear();
//
///* Draw a hexagon with line thickness: 1 pixel */
//glcd_setcolor(GLCD_CL_YELLOW);
//glcd_setlinestyle(1,GLCD_LINE_SOLID);
//glcd_drawpoly(7,hexagon);
//
///* Write a text inside the hexagon */
//glcd_setfont(font5x7);
//glcd_outtextxyf(30,17,"Fill");
//
///* Fill the hexagon with solid red color fill */
//glcd_setfillcolor(GLCD_CL_RED);
//glcd_floodfill(48,24,GLCD_CL_YELLOW /* border color */);
//
/////* Save hexagon image in RAM */
////glcd_getimage(22,0,37,41,buffer);
//
//glcd_setfont(font5x7);
//glcd_outtextxyf(0,47,"Image was saved to RAM");
//
///* Draw a circle with line thickness: 1 pixel */
//glcd_circle(21,83,20);
//
///* Draw an arc of a circle with line thickness: 1 pixel */
//glcd_setcolor(GLCD_CL_RED);
//glcd_arc(21,83,0,120,15);
//
///* Draw a circle with line thickness: 3 pixels */
//glcd_setlinestyle(3,GLCD_LINE_SOLID);
//glcd_setcolor(GLCD_CL_LIME);
//glcd_circle(63,83,20);
//
///* Draw an arc of a circle with line thickness: 3 pixels */
//glcd_setcolor(GLCD_CL_MAGENTA);
//glcd_arc(63,83,0,210,15);
//
//glcd_setcolor(GLCD_CL_YELLOW);
//glcd_outtextxyf(0,107,"Circles & arcs");
//
///* Draw a rectangle with line thickness: 1 pixel
//   and solid line */
//glcd_setcolor(GLCD_CL_RED);
//glcd_setlinestyle(1,GLCD_LINE_SOLID);
//glcd_rectrel(100,60,40,25);
//
///* Draw a rectangle with line thickness: 3 pixel
//   and dotted line */
//glcd_setcolor(GLCD_CL_AQUA);
//glcd_setlinestyle(3,GLCD_LINE_DOT_LARGE);
//glcd_rectrel(142,60,40,25);
//
///* Draw a bar filled with a pattern of green color */
//glcd_setfillf(small_dots,GLCD_CL_GREEN);
//glcd_bar(114,89,170,104);
//
//glcd_setcolor(GLCD_CL_YELLOW);
//glcd_outtextxyf(100,107,"Rectangles & filled bar");
//
///* Draw rounded rectangle */
//glcd_setcolor(GLCD_CL_RED);
//glcd_rectround(5,125,74,38,8);
//
//glcd_setcolor(GLCD_CL_YELLOW);
//glcd_outtextxyf(16,136,"Rounded");
//glcd_outtextxyf(16,144,"rectangle");
//
///* Set line thickness: 1 pixel */
//glcd_setlinestyle(1,GLCD_LINE_SOLID);
//
//glcd_setcolor(GLCD_CL_YELLOW);
//glcd_outtextxyf(100,120,"Pie");
//glcd_outtextxyf(100,127,"slice:");
//
////Draw a pie slice filled with blue color 
//glcd_setfillcolor(GLCD_CL_BLUE);
//glcd_pieslice(170,143,30,300,20);
//
//glcd_outtextxyf(0,170,"Put\nimage\nfrom\nRAM:");
//
/////* Display image saved in RAM
////   without transparency */
////glcd_putimage(46,170,buffer,GLCD_PUTCOPY);
//
//glcd_outtextxyf(0,215,"Non-transparent mode");
//
///* Set transparent color */
//glcd_settpcolor(GLCD_CL_RED);
//
/////* Display image saved in RAM
////   with transparency */
////glcd_putimage(150,170,buffer,GLCD_PUTTP);
//
//glcd_outtextxyf(130,215,"Transparent mode");
//
///* Set fill pattern and color */
//glcd_setfillf(solid_fill,GLCD_CL_WHITE);


glcd_setfont(arial_bold14);
glcd_setcolor(GLCD_CL_CYAN);
//glcd_outtextxyf(30,225,"Meter & Bar-graph gauges:");

glcd_outtextxyf(30,30,"Meter & Bar-graph gauges:");

/* Display a meter gauge */
meter.xc=120;
meter.yc=205;
meter.radius=80;
meter.min_value=20;
meter.max_value=100;
meter.ticks=11;
//meter.crt_value=60;

meter_init(&meter);

/* Display a bar-graph gauge */
bargraph.xt=20;
bargraph.yt=230;
bargraph.width=200;
bargraph.height=60;
bargraph.min_value=meter.min_value;
bargraph.max_value=meter.max_value;
bargraph.ticks=meter.ticks;

bargraph_init(&bargraph);

while(1){

for (i=0; i<4; i++)
    {
    for (x=meter.min_value; x<=meter.max_value; x+=5)
        {
        meter_update(&meter,x);
        bargraph_update(&bargraph,x);
        delay_ms(50);
        }
    for (x=meter.max_value; x>=meter.min_value; x-=5)
        {
        meter_update(&meter,x);
        bargraph_update(&bargraph,x);
        delay_ms(50);
        }
    }
}
///* 1 second delay */
//delay_ms(1000);
///* Clear display */
//glcd_clear();
//
//glcd_setfont(arial_bold14);
//glcd_setcolor(GLCD_CL_CYAN);
//glcd_outtextxyf(27,0,"Display an image from FLASH:");
//
/////* Display image stored in FLASH */
////glcd_putimagef(27,20,img,GLCD_PUTCOPY);
//
//glcd_outtextxyf(27,165,"Button examples:");
//
///* Create OK button */
//button_ok.left=27;
//button_ok.top=195;
//button_ok.width=80;
//button_ok.height=30;
//button_ok.border_color=GLCD_CL_BLUE;
//button_ok.bg_color=GLCD_CL_LIGHT_GRAY;
//button_ok.pressed_color=GLCD_CL_WHITE;
//button_ok.text_color=GLCD_CL_BLACK;
//button_ok.glyph_tp_color=GLCD_CL_WHITE;
//button_ok.glyph=ok_glyph;
//button_ok.caption="OK";
//button_ok.font=arial_bold14;
//
//button_init(&button_ok);
//
///* Create Cancel button */
///* Most of the parameters are the same for both buttons, */
//button_cancel=button_ok;
///* except left coordinate, */
//button_cancel.left=124;
///* glyph */
//button_cancel.glyph=cancel_glyph;
///* and caption */
//button_cancel.caption="Cancel";
//
//button_init(&button_cancel);
//
///* Ensure that text is displayed in non-transparent mode */
//glcd_transparent(false);
//
///* Toggle buttons */
//for (i=0;i<10;i++)
//    {
//    button_ok.pressed=!button_ok.pressed;
//    button_update(&button_ok);
//
//    button_cancel.pressed=button_ok.pressed;
//    button_update(&button_cancel);
//
//    glcd_setcolor(GLCD_CL_CYAN);
//    glcd_outtextxyf(27,240,button_state[button_ok.pressed]);
//    delay_ms(500);
//    }
//
///* 2 seconds delay */
//delay_ms(2000);
//
///* Clear display */
//glcd_clear();
//
//glcd_setfont(arial_bold14);
//glcd_setcolor(GLCD_CL_CYAN);
//glcd_outtextf("Predefined colors:");
//
///* Display the predefined colors */
//
///* Set fill pattern and color */
//glcd_setfillf(solid_fill,GLCD_CL_WHITE);
//
//glcd_setfont(font5x7);
//glcd_setcolor(GLCD_CL_WHITE);
//
//for (i=0; i<(sizeof(colors)/sizeof(color_data));i++)
//    {
//    glcd_setfillcolor(colors[i].col);
//    x=(i%2)*120;
//    y=(i/2)*20+20;
//    glcd_outtextxyf(x,y+6,colors[i].name);
//    glcd_barrel(x+65,y,45,19);
//    };
//
///* 2 seconds delay */
//delay_ms(2000);
//
//glcd_setfont(arial_bold14);
//glcd_setcolor(GLCD_CL_CYAN);
//glcd_outtextxyf(0,280,"Entering sleep mode");
//
///* 2 seconds delay */
//delay_ms(2000);
//
///* Enter sleep mode */
//glcd_display(false);
//
///* 2 seconds delay */
//delay_ms(2000);
//
///* Exit from sleep mode */
//glcd_display(true);
//
//glcd_outtextxyf(0,280,"Exit from sleep mode");
//
///* Stop here */
//while (1);

}
