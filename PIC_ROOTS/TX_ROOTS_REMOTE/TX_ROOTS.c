#include <16f690.h>
#INCLUDE <stdlib.h>
#FUSES INTRC,NOWDT,PUT,PROTECT,NOMCLR,NOBROWNOUT
#use delay(clock=8M)

#use rs232(baud=9600,xmit=PIN_B7,rcv=PIN_B5,stream=GSM,ERRORS)
#include "internal_eeprom.c"

// -- Pin Definitions ------------------------------
#define BUTTON1 PIN_C4
#define BUTTON2 PIN_C3
#define BUTTON3 PIN_C6
#define BUTTON4 PIN_C7

#define LED1 PIN_A0
#define LED2 PIN_A1

// -- Global Variables -----------------------------
int1 rx_flag = 0;
char tempdata;

// -- UART Receive Interrupt ------------------------
#int_RDA
void RDA_isr(void)
{
   char temp;
   temp = getc(GSM);

   if (temp == '#') {
      tempdata = getc(GSM);   // read command byte after '#'
      rx_flag = 1;
   }
}

// -- Main -----------------------------------------
void main()
{
   set_tris_a(0x00);
   set_tris_c(0xe8);
   set_tris_b(0x20);

   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   while (kbhit(GSM)) getc(GSM);   // flush UART buffer

   output_low(LED1);
   output_low(LED2);

   while(TRUE)
   {
      // -- BUTTON 1 pressed --
      if (!input(BUTTON1)) {
         putc('#', GSM); putc('A', GSM);   // ? no \r
         delay_ms(200);
         putc('#', GSM); putc('A', GSM);
         delay_ms(200);
         putc('#', GSM); putc('A', GSM);
         delay_ms(200);

         while (!input(BUTTON1));          // wait for release

         putc('#', GSM); putc('a', GSM);   // ? no \r
         delay_ms(200);
         putc('#', GSM); putc('a', GSM);
         delay_ms(200);
         putc('#', GSM); putc('a', GSM);
         delay_ms(200);
      }

      // -- BUTTON 2 pressed --
      if (!input(BUTTON2)) {
         putc('#', GSM); putc('B', GSM);
         delay_ms(200);
         putc('#', GSM); putc('B', GSM);
         delay_ms(200);
         putc('#', GSM); putc('B', GSM);
         delay_ms(200);

         while (!input(BUTTON2));

         putc('#', GSM); putc('b', GSM);
         delay_ms(200);
         putc('#', GSM); putc('b', GSM);
         delay_ms(200);
         putc('#', GSM); putc('b', GSM);
         delay_ms(200);
      }

      // -- BUTTON 3 pressed --
      if (!input(BUTTON3)) {
         putc('#', GSM); putc('C', GSM);
         delay_ms(200);
         putc('#', GSM); putc('C', GSM);
         delay_ms(200);
         putc('#', GSM); putc('C', GSM);
         delay_ms(200);

         while (!input(BUTTON3));

         putc('#', GSM); putc('c', GSM);
         delay_ms(200);
         putc('#', GSM); putc('c', GSM);
         delay_ms(200);
         putc('#', GSM); putc('c', GSM);
         delay_ms(200);
      }

      // -- BUTTON 4 pressed --
      if (!input(BUTTON4)) {
         putc('#', GSM); putc('D', GSM);
         delay_ms(200);
         putc('#', GSM); putc('D', GSM);
         delay_ms(200);
         putc('#', GSM); putc('D', GSM);
         delay_ms(200);

         while (!input(BUTTON4));

         putc('#', GSM); putc('d', GSM);
         delay_ms(200);
         putc('#', GSM); putc('d', GSM);
         delay_ms(200);
         putc('#', GSM); putc('d', GSM);
         delay_ms(200);
      }

      delay_ms(50);

      // -- Process received data from RX side --
      if (rx_flag) {
         rx_flag = 0;

         printf("\r\nReceived: %c\r\n", tempdata);

         if (tempdata == 'G') {
            output_high(LED1);
            output_low(LED2);
         }
         if (tempdata == 'g') {
            output_low(LED1);
            output_low(LED2);
         }
         if (tempdata == 'H') {
            output_high(LED2);
            output_low(LED1);
         }
         if (tempdata == 'h') {
            output_low(LED2);
            output_low(LED1);
         }

         delay_ms(20);
      }
   }
}
