#include "matrix.h"
#include "libsCore.h"

unsigned const char matrix_fonts[]={
0x00,0x00,0x00,0x00,0x00,	/*char 00h*/
0x00,0x00,0x00,0x00,0x00,	/*char 01h*/
0x00,0x00,0x00,0x00,0x00,	/*char 02h*/
0x00,0x00,0x00,0x00,0x00,	/*char 03h*/
0x00,0x00,0x00,0x00,0x00,	/*char 04h*/
0x00,0x00,0x00,0x00,0x00,	/*char 05h*/
0x00,0x00,0x00,0x00,0x00,	/*char 06h*/
0x00,0x00,0x00,0x00,0x00,	/*char 07h*/
0x00,0x00,0x00,0x00,0x00,	/*char 08h*/
0x00,0x00,0x00,0x00,0x00,	/*char 09h*/
0x00,0x00,0x00,0x00,0x00,	/*char 0Ah*/
0x00,0x00,0x00,0x00,0x00,	/*char 0Bh*/
0x00,0x00,0x00,0x00,0x00,	/*char 0Ch*/
0x00,0x00,0x00,0x00,0x00,	/*char 0Dh*/
0x00,0x00,0x00,0x00,0x00,	/*char 0Eh*/
0x00,0x00,0x00,0x00,0x00,	/*char 0Fh*/
0x00,0x00,0x00,0x00,0x00,	/*char 10h*/
0x00,0x00,0x00,0x00,0x00,	/*char 11h*/
0x00,0x00,0x00,0x00,0x00,	/*char 12h*/
0x00,0x00,0x00,0x00,0x00,	/*char 13h*/
0x00,0x00,0x00,0x00,0x00,	/*char 14h*/
0x00,0x00,0x00,0x00,0x00,	/*char 15h*/
0x00,0x00,0x00,0x00,0x00,	/*char 16h*/
0x00,0x00,0x00,0x00,0x00,	/*char 17h*/
0x00,0x00,0x00,0x00,0x00,	/*char 18h*/
0x00,0x00,0x00,0x00,0x00,	/*char 19h*/
0x00,0x00,0x00,0x00,0x00,	/*char 1Ah*/
0x00,0x00,0x00,0x00,0x00,	/*char 1Bh*/
0x00,0x00,0x00,0x00,0x00,	/*char 1Ch*/
0x00,0x00,0x00,0x00,0x00,	/*char 1Dh*/
0x00,0x00,0x00,0x00,0x00,	/*char 1Eh*/
0x00,0x00,0x00,0x00,0x00,	/*char 1Fh*/
0x00,0x00,0x00,0x00,0x00,	/*char 20h*/
0x00,0x00,0x4f,0x00,0x00,	/*char 21h*/
0x00,0x07,0x00,0x07,0x00,	/*char 22h*/
0x14,0x7f,0x14,0x7f,0x14,	/*char 23h*/
0x24,0x2a,0x7f,0x2a,0x12,	/*char 24h*/
0x23,0x13,0x08,0x64,0x62,	/*char 25h*/
0x36,0x49,0x55,0x22,0x50,	/*char 26h*/
0x00,0x05,0x03,0x00,0x00,	/*char 27h*/
0x00,0x1c,0x22,0x41,0x00,	/*char 28h*/
0x00,0x41,0x22,0x1c,0x00,	/*char 29h*/
0x14,0x08,0x3e,0x08,0x14,	/*char 2Ah*/
0x08,0x08,0x3e,0x08,0x08,	/*char 2Bh*/
0x00,0x50,0x30,0x00,0x00,	/*char 2Ch*/
0x08,0x08,0x08,0x08,0x08,	/*char 2Dh*/
0x00,0x60,0x60,0x00,0x00,	/*char 2Eh*/
0x20,0x10,0x08,0x04,0x02,	/*char 2Fh*/
0x3e,0x51,0x49,0x45,0x3e,	/*char 30h*/
0x40,0x42,0x7f,0x40,0x40,	/*char 31h*/
0x42,0x61,0x51,0x49,0x46,	/*char 32h*/
0x21,0x41,0x45,0x4b,0x31,	/*char 33h*/
0x18,0x14,0x12,0x7f,0x10,	/*char 34h*/
0x47,0x45,0x45,0x45,0x39,	/*char 35h*/
0x3c,0x4a,0x49,0x49,0x30,	/*char 36h*/
0x01,0x71,0x09,0x05,0x03,	/*char 37h*/
0x36,0x49,0x49,0x49,0x36,	/*char 38h*/
0x06,0x49,0x49,0x29,0x1e,	/*char 39h*/
0x00,0x36,0x36,0x00,0x00,	/*char 3Ah*/
0x00,0x56,0x36,0x00,0x00,	/*char 3Bh*/
0x08,0x14,0x22,0x41,0x00,	/*char 3Ch*/
0x14,0x14,0x14,0x14,0x14,	/*char 3Dh*/
0x00,0x41,0x22,0x14,0x08,	/*char 3Eh*/
0x02,0x01,0x51,0x09,0x06,	/*char 3Fh*/
0x32,0x49,0x79,0x41,0x3e,	/*char 40h*/
0x7e,0x11,0x11,0x11,0x7e,	/*char 41h*/
0x7f,0x49,0x49,0x49,0x36,	/*char 42h*/
0x3e,0x41,0x41,0x41,0x22,	/*char 43h*/
0x7f,0x41,0x41,0x22,0x1c,	/*char 44h*/
0x7f,0x49,0x49,0x49,0x41,	/*char 45h*/
0x7f,0x09,0x09,0x09,0x01,	/*char 46h*/
0x3e,0x41,0x49,0x49,0x7a,	/*char 47h*/
0x7f,0x08,0x08,0x08,0x7f,	/*char 48h*/
0x00,0x41,0x7f,0x41,0x00,	/*char 49h*/
0x20,0x40,0x41,0x3f,0x01,	/*char 4Ah*/
0x7f,0x08,0x14,0x22,0x41,	/*char 4Bh*/
0x7f,0x40,0x40,0x40,0x40,	/*char 4Ch*/
0x7f,0x02,0x0c,0x02,0x7f,	/*char 4Dh*/
0x7f,0x04,0x08,0x10,0x7f,	/*char 4Eh*/
0x3e,0x41,0x41,0x41,0x3e,	/*char 4Fh*/
0x7f,0x09,0x09,0x09,0x06,	/*char 50h*/
0x3e,0x41,0x51,0x21,0x5e,	/*char 51h*/
0x7f,0x09,0x19,0x29,0x46,	/*char 52h*/
0x46,0x49,0x49,0x49,0x31,	/*char 53h*/
0x01,0x01,0x7f,0x01,0x01,	/*char 54h*/
0x3f,0x40,0x40,0x40,0x3f,	/*char 55h*/
0x1f,0x20,0x40,0x20,0x1f,	/*char 56h*/
0x3f,0x40,0x38,0x40,0x3f,	/*char 57h*/
0x63,0x14,0x08,0x14,0x63,	/*char 58h*/
0x0f,0x10,0x60,0x10,0x0f,	/*char 59h*/
0x61,0x51,0x49,0x45,0x43,	/*char 5Ah*/
0x00,0x7f,0x41,0x41,0x00,	/*char 5Bh*/
0x15,0x16,0x7c,0x16,0x15,	/*char 5Ch*/
0x00,0x41,0x41,0x7f,0x00,	/*char 5Dh*/
0x04,0x02,0x01,0x02,0x04,	/*char 5Eh*/
0x40,0x40,0x40,0x40,0x40,	/*char 5Fh*/
0x00,0x01,0x02,0x04,0x00,	/*char 60h*/
0x20,0x54,0x54,0x54,0x78,	/*char 61h*/
0x7f,0x48,0x44,0x44,0x38,	/*char 62h*/
0x38,0x44,0x44,0x44,0x20,	/*char 63h*/
0x38,0x44,0x44,0x48,0x7f,	/*char 64h*/
0x38,0x54,0x54,0x54,0x18,	/*char 65h*/
0x08,0x7e,0x09,0x01,0x02,	/*char 66h*/
0x0c,0x52,0x52,0x52,0x3e,	/*char 67h*/
0x7f,0x08,0x04,0x04,0x78,	/*char 68h*/
0x00,0x44,0x7d,0x40,0x00,	/*char 69h*/
0x20,0x40,0x44,0x3d,0x00,	/*char 6Ah*/
0x7f,0x10,0x28,0x44,0x00,	/*char 6Bh*/
0x00,0x41,0x7f,0x40,0x00,	/*char 6Ch*/
0x78,0x08,0x10,0x08,0x70,	/*char 6Dh*/
0x7c,0x08,0x04,0x04,0x78,	/*char 6Eh*/
0x38,0x44,0x44,0x44,0x38,	/*char 6Fh*/
0x7c,0x14,0x14,0x14,0x08,	/*char 70h*/
0x08,0x14,0x14,0x18,0x7c,	/*char 71h*/
0x7c,0x08,0x04,0x04,0x08,	/*char 72h*/
0x48,0x54,0x54,0x54,0x20,	/*char 73h*/
0x04,0x3f,0x44,0x40,0x20,	/*char 74h*/
0x3c,0x40,0x40,0x20,0x7c,	/*char 75h*/
0x1c,0x20,0x40,0x20,0x1c,	/*char 76h*/
0x3c,0x40,0x30,0x40,0x3c,	/*char 77h*/
0x44,0x28,0x10,0x28,0x44,	/*char 78h*/
0x0c,0x50,0x50,0x50,0x3c,	/*char 79h*/
0x44,0x64,0x54,0x4c,0x44,	/*char 7Ah*/
0x00,0x08,0x36,0x41,0x00,	/*char 7Bh*/
0x00,0x00,0x7f,0x00,0x00,	/*char 7Ch*/
0x00,0x41,0x36,0x08,0x00,	/*char 7Dh*/
0x08,0x08,0x2a,0x1c,0x08,	/*char 7Eh*/
0x08,0x1c,0x2a,0x08,0x08,	/*char 7Fh*/
0x00,0x00,0x00,0x00,0x00,	/*char 80h*/
0x00,0x00,0x00,0x00,0x00,	/*char 81h*/
0x00,0x00,0x00,0x00,0x00,	/*char 82h*/
0x00,0x00,0x00,0x00,0x00,	/*char 83h*/
0x00,0x00,0x00,0x00,0x00,	/*char 84h*/
0x00,0x00,0x00,0x00,0x00,	/*char 85h*/
0x00,0x00,0x00,0x00,0x00,	/*char 86h*/
0x00,0x00,0x00,0x00,0x00,	/*char 87h*/
0x00,0x00,0x00,0x00,0x00,	/*char 88h*/
0x00,0x00,0x00,0x00,0x00,	/*char 89h*/
0x00,0x00,0x00,0x00,0x00,	/*char 8Ah*/
0x00,0x00,0x00,0x00,0x00,	/*char 8Bh*/
0x00,0x00,0x00,0x00,0x00,	/*char 8Ch*/
0x00,0x00,0x00,0x00,0x00,	/*char 8Dh*/
0x00,0x00,0x00,0x00,0x00,	/*char 8Eh*/
0x00,0x00,0x00,0x00,0x00,	/*char 8Fh*/
0x00,0x00,0x00,0x00,0x00,	/*char 90h*/
0x00,0x00,0x00,0x00,0x00,	/*char 91h*/
0x00,0x00,0x00,0x00,0x00,	/*char 92h*/
0x00,0x00,0x00,0x00,0x00,	/*char 93h*/
0x00,0x00,0x00,0x00,0x00,	/*char 94h*/
0x00,0x00,0x00,0x00,0x00,	/*char 95h*/
0x00,0x00,0x00,0x00,0x00,	/*char 96h*/
0x00,0x00,0x00,0x00,0x00,	/*char 97h*/
0x00,0x00,0x00,0x00,0x00,	/*char 98h*/
0x00,0x00,0x00,0x00,0x00,	/*char 99h*/
0x00,0x00,0x00,0x00,0x00,	/*char 9Ah*/
0x00,0x00,0x00,0x00,0x00,	/*char 9Bh*/
0x00,0x00,0x00,0x00,0x00,	/*char 9Ch*/
0x00,0x00,0x00,0x00,0x00,	/*char 9Dh*/
0x00,0x00,0x00,0x00,0x00,	/*char 9Eh*/
0x00,0x00,0x00,0x00,0x00,	/*char 9Fh*/
0x00,0x00,0x00,0x00,0x00,	/*char A0h*/
0x70,0x50,0x70,0x00,0x00,	/*char A1h*/
0x00,0x00,0x07,0x01,0x01,	/*char A2h*/
0x40,0x40,0x70,0x00,0x00,	/*char A3h*/
0x10,0x20,0x40,0x00,0x00,	/*char A4h*/
0x00,0x18,0x18,0x00,0x00,	/*char A5h*/
0x0a,0x0a,0x4a,0x2a,0x1e,	/*char A6h*/
0x04,0x44,0x34,0x14,0x0c,	/*char A7h*/
0x40,0x20,0x70,0x08,0x00,	/*char A8h*/
0x18,0x08,0x4c,0x48,0x38,	/*char A9h*/
0x48,0x48,0x78,0x48,0x48,	/*char AAh*/
0x48,0x28,0x18,0x7c,0x08,	/*char ABh*/
0x08,0x7c,0x08,0x28,0x18,	/*char ACh*/
0x40,0x48,0x48,0x78,0x40,	/*char ADh*/
0x54,0x54,0x54,0x7c,0x00,	/*char AEh*/
0x18,0x00,0x58,0x40,0x38,	/*char AFh*/
0x08,0x08,0x08,0x08,0x08,	/*char B0h*/
0x01,0x41,0x3d,0x09,0x07,	/*char B1h*/
0x10,0x08,0x7c,0x02,0x01,	/*char B2h*/
0x06,0x02,0x43,0x22,0x1e,	/*char B3h*/
0x42,0x42,0x7e,0x42,0x42,	/*char B4h*/
0x22,0x12,0x0a,0x7f,0x02,	/*char B5h*/
0x42,0x3f,0x02,0x42,0x3e,	/*char B6h*/
0x0a,0x0a,0x7f,0x0a,0x0a,	/*char B7h*/
0x08,0x46,0x42,0x22,0x1e,	/*char B8h*/
0x04,0x03,0x42,0x3e,0x02,	/*char B9h*/
0x42,0x42,0x42,0x42,0x7e,	/*char BAh*/
0x02,0x4f,0x22,0x1f,0x02,	/*char BBh*/
0x4a,0x4a,0x40,0x20,0x1e,	/*char BCh*/
0x42,0x22,0x12,0x2a,0x46,	/*char BDh*/
0x02,0x3f,0x42,0x4a,0x46,	/*char BEh*/
0x06,0x48,0x40,0x20,0x1e,	/*char BFh*/
0x08,0x46,0x4a,0x32,0x1e,	/*char C0h*/
0x0a,0x4a,0x3e,0x09,0x08,	/*char C1h*/
0x0e,0x00,0x4e,0x20,0x1e,	/*char C2h*/
0x04,0x45,0x3d,0x05,0x04,	/*char C3h*/
0x00,0x7f,0x08,0x10,0x00,	/*char C4h*/
0x44,0x24,0x1f,0x04,0x04,	/*char C5h*/
0x40,0x42,0x42,0x42,0x40,	/*char C6h*/
0x42,0x2a,0x12,0x2a,0x06,	/*char C7h*/
0x22,0x12,0x7b,0x16,0x22,	/*char C8h*/
0x00,0x40,0x20,0x1f,0x00,	/*char C9h*/
0x78,0x00,0x02,0x04,0x78,	/*char CAh*/
0x3f,0x44,0x44,0x44,0x44,	/*char CBh*/
0x02,0x42,0x42,0x22,0x1e,	/*char CCh*/
0x04,0x02,0x04,0x08,0x30,	/*char CDh*/
0x32,0x02,0x7f,0x02,0x32,	/*char CEh*/
0x02,0x12,0x22,0x52,0x0e,	/*char CFh*/
0x00,0x2a,0x2a,0x2a,0x40,	/*char D0h*/
0x38,0x24,0x22,0x20,0x70,	/*char D1h*/
0x40,0x28,0x10,0x28,0x06,	/*char D2h*/
0x0a,0x3e,0x4a,0x4a,0x4a,	/*char D3h*/
0x04,0x7f,0x04,0x14,0x0c,	/*char D4h*/
0x40,0x42,0x42,0x7e,0x40,	/*char D5h*/
0x4a,0x4a,0x4a,0x4a,0x7e,	/*char D6h*/
0x04,0x05,0x45,0x25,0x1c,	/*char D7h*/
0x0f,0x40,0x20,0x1f,0x00,	/*char D8h*/
0x7c,0x00,0x7e,0x40,0x30,	/*char D9h*/
0x7e,0x40,0x20,0x10,0x08,	/*char DAh*/
0x7e,0x42,0x42,0x42,0x7e,	/*char DBh*/
0x0e,0x02,0x42,0x22,0x1e,	/*char DCh*/
0x42,0x42,0x40,0x20,0x18,	/*char DDh*/
0x02,0x04,0x01,0x02,0x00,	/*char DEh*/
0x07,0x05,0x07,0x00,0x00,	/*char DFh*/
0x38,0x44,0x48,0x30,0x4c,	/*char E0h*/
0x30,0x55,0x54,0x55,0x78,	/*char E1h*/
0xff,0xff,0xff,0xff,0xff,	/*char E2h*/
0x28,0x54,0x54,0x44,0x20,	/*char E3h*/
0xff,0xff,0xff,0xff,0xff,	/*char E4h*/
0x38,0x44,0x4c,0x54,0x24,	/*char E5h*/
0xff,0xff,0xff,0xff,0xff,	/*char E6h*/
0xff,0xff,0xff,0xff,0xff,	/*char E7h*/
0x20,0x40,0x3c,0x04,0x04,	/*char E8h*/
0x04,0x04,0x00,0x0e,0x00,	/*char E9h*/
0xff,0xff,0xff,0xff,0xff,	/*char EAh*/
0x0a,0x04,0x0a,0x00,0x00,	/*char EBh*/
0x18,0x24,0x7e,0x24,0x10,	/*char ECh*/
0x14,0x7f,0x54,0x40,0x40,	/*char EDh*/
0x7c,0x09,0x05,0x05,0x78,	/*char EEh*/
0x38,0x45,0x44,0x45,0x38,	/*char EFh*/
0xff,0xff,0xff,0xff,0xff,	/*char F0h*/
0xff,0xff,0xff,0xff,0xff,	/*char F1h*/
0x3c,0x4a,0x4a,0x4a,0x3c,	/*char F2h*/
0x60,0x50,0x20,0x50,0x30,	/*char F3h*/
0x58,0x64,0x04,0x64,0x58,	/*char F4h*/
0x3c,0x41,0x40,0x21,0x7c,	/*char F5h*/
0x63,0x55,0x49,0x41,0x41,	/*char F6h*/
0x44,0x3c,0x04,0x7c,0x44,	/*char F7h*/
0x45,0x29,0x11,0x29,0x45,	/*char F8h*/
0xff,0xff,0xff,0xff,0xff,	/*char F9h*/
0x14,0x14,0x7c,0x14,0x12,	/*char FAh*/
0x44,0x3c,0x14,0x14,0x74,	/*char FBh*/
0x7c,0x14,0x1c,0x14,0x7c,	/*char FCh*/
0x08,0x08,0x2a,0x08,0x08,	/*char FDh*/
0x00,0x00,0x00,0x00,0x00,	/*char FEh*/
0xff,0xff,0xff,0xff,0xff	/*char FFh*/
};


void matrix_font_debug_out_sample(unsigned int matrix_led_pattern[])
{
  static int cnt='A'*5;
  int i;
  for(i=0;i<8;i++){
    matrix_led_pattern[i]=matrix_fonts[cnt+i]; 
  }
  cnt++;

  //debug����
  //lcd_putdec(5,1,3,cnt+i);
  //lcd_putdec(8,1,3,matrix_fonts[cnt+i]);
  //lcd_putudec(11,1,5,(unsigned int)(&matrix_fonts[cnt+i]));

}
