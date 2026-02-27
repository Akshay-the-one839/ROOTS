#include <16f690.h>
#INCLUDE <stdlib.h>
#FUSES INTRC,NOWDT,PUT,PROTECT,NOMCLR,NOBROWNOUT
#use delay(clock=8M)

#use rs232(baud=9600,xmit=PIN_B7,rcv=PIN_B5,stream=COM_1,ERRORS)
#include "internal_eeprom.c"

// -- Pin Definitions ------------------------------
#define SW3  PIN_C6
#define SW4  PIN_C7

#define LED1 PIN_A2
#define LED2 PIN_C0
#define LED3 PIN_C1
#define LED4 PIN_C2
#define LED5 PIN_A0
#define LED6 PIN_A1

// -- Global Variables -----------------------------
int1 rx_flag = 0;
char tempdata;

// -- UART Receive Interrupt ------------------------
#int_RDA
void RDA_isr(void)
{
   char temp;
   temp = getc(COM_1);

   if (temp == '#') {
      tempdata = getc(COM_1);   // read command byte after '#'
      rx_flag = 1;
   }
}

// -- Main -----------------------------------------
void main()
{
   output_a(0x00);
   output_c(0xf0);

   set_tris_a(0x00);
   set_tris_c(0xf0);
   set_tris_b(0x20);

   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   while (kbhit(COM_1)) getc(COM_1);   // flush UART buffer

   output_low(LED1);
   output_low(LED2);
   output_low(LED3);
   output_low(LED4);
   output_low(LED5);
   output_low(LED6);

   printf("\r\nRX PIC Ready...\r\n");

   while(TRUE)
   {
      // -- SW3 pressed --
      if (!input(SW3)) {
         putc('#', COM_1); putc('G', COM_1);   // ? no \r
         delay_ms(200);
         putc('#', COM_1); putc('G', COM_1);
         delay_ms(200);
         putc('#', COM_1); putc('G', COM_1);
         delay_ms(200);

         while (!input(SW3));                   // wait for release

         putc('#', COM_1); putc('g', COM_1);   // ? no \r
         delay_ms(200);
         putc('#', COM_1); putc('g', COM_1);
         delay_ms(200);
         putc('#', COM_1); putc('g', COM_1);
         delay_ms(200);
      }

      // -- SW4 pressed --
      if (!input(SW4)) {
         putc('#', COM_1); putc('H', COM_1);
         delay_ms(200);
         putc('#', COM_1); putc('H', COM_1);
         delay_ms(200);
         putc('#', COM_1); putc('H', COM_1);
         delay_ms(200);

         while (!input(SW4));

         putc('#', COM_1); putc('h', COM_1);
         delay_ms(200);
         putc('#', COM_1); putc('h', COM_1);
         delay_ms(200);
         putc('#', COM_1); putc('h', COM_1);
         delay_ms(200);
      }

      // -- Process received data from TX side --
      if (rx_flag) {
         rx_flag = 0;

         printf("\r\nReceived: %c\r\n", tempdata);

         if (tempdata == 'A')
            output_high(LED1);
         if (tempdata == 'a')
            output_low(LED1);

         if (tempdata == 'B')
            output_high(LED2);
         if (tempdata == 'b')
            output_low(LED2);

         if (tempdata == 'C')
            output_high(LED3);
         if (tempdata == 'c')
            output_low(LED3);

         if (tempdata == 'D')
            output_high(LED4);
         if (tempdata == 'd')
            output_low(LED4);

         delay_ms(100);
      }
   }
}
