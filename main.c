//LCD module connections
#define LCD_RS_PIN     PIN_D0
#define LCD_RW_PIN     PIN_D1
#define LCD_ENABLE_PIN PIN_D2
#define LCD_DATA4      PIN_D3
#define LCD_DATA5      PIN_D4
#define LCD_DATA6      PIN_D5
#define LCD_DATA7      PIN_D6
//End LCD module connections

#include <18F4520.h> 
#fuses NOMCLR NOBROWNOUT NOLVP INTRC_IO
#use delay(clock = 8MHz)
#include <lcd.c>
#use fast_io(B)
#use I2C(master, I2C1, FAST = 100000)

char time[] = "TIME:  :  :  ";
char calendar[] = "DATE:  /  /20  ";
unsigned int8  second, minute, hour, date, month, year, day;
void ds1307_display(){
  // chuyen ma BCD thanh so thap phan
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour = (hour >> 4) * 10 + (hour & 0x0F);
  date = (date >> 4) * 10 + (date & 0x0F);
  month = (month >> 4) * 10 + (month & 0x0F);
  year = (year >> 4) * 10 + (year & 0x0F);
  // End conversion
  time[12]     = second % 10  + 48;
  time[11]     = second / 10  + 48;
  time[9]      = minute % 10  + 48;
  time[8]      = minute / 10  + 48;
  time[6]      = hour % 10  + 48;
  time[5]      = hour / 10  + 48;
  calendar[14] = year % 10 + 48;
  calendar[13] = year / 10  + 48;
  calendar[9]  = month % 10 + 48;
  calendar[8]  = month / 10 + 48;
  calendar[6]  = date % 10 + 48;
  calendar[5]  = date / 10 + 48;
  lcd_gotoxy(1, 1);                              // den dong 1 cot 1
  printf(lcd_putc, time);                        // Display time
  lcd_gotoxy(1, 2);                              // den dong 2 cot 1
  printf(lcd_putc, calendar);                    // Display calendar
}
void ds1307_write(unsigned int8 address, data_){
  i2c_start();                                   // khoi tao giaoo thuc i2c
  i2c_write(0xD0);                               // khai bao dia chi ds1307
  i2c_write(address);                            // gui dia chi ds1307
  i2c_write(data_);                              // ghi du lieu vao ds1307
  i2c_stop();                                    // dung lai
}
void main(){
  setup_oscillator(OSC_8MHZ);                    // dat cho bo giao thanh 8MHz
  port_b_pullups(3);                             // kich hoat che do nhan cho RB0 va RB1
  lcd_init();                                    // khoi tao mo dun LCD 
  lcd_putc('\f');                                // hien thi LCD ro rang
  while(TRUE){
   if(!input(PIN_B0)){                           // neu nut RB0 duoc nhan
    lcd_putc('\f');                              // LCD clear
    lcd_gotoxy(5, 1);                            // den cot 5 hang 1
    lcd_putc("Minute:");
    delay_ms(200);
    while(TRUE){
     if(!input(PIN_B1))                          
      minute++;                                  
     if(minute > 59)
      minute = 0;
     lcd_gotoxy(8, 2);                           
     printf(lcd_putc,"%02u", minute);
     if(!input(PIN_B0))
      break;
     delay_ms(200);
    }
    lcd_putc('\f');                             
    lcd_gotoxy(6, 1);                            
    lcd_putc("Hour:");
    delay_ms(200);
    while(TRUE){
     if(!input(PIN_B1))
      hour++;
     if(hour > 23)
      hour = 0;
     lcd_gotoxy(8, 2);                          
     printf(lcd_putc,"%02u", hour);
     if(!input(PIN_B0))
      break;
     delay_ms(200);
    }
    lcd_putc('\f');                            
    lcd_gotoxy(6, 1);                         
    lcd_putc("Date:");
    delay_ms(200);
    while(TRUE){
     if(!input(PIN_B1))
      date++;
     if(date > 31)
      date = 1;
     lcd_gotoxy(8, 2);                          
     printf(lcd_putc,"%02u", date);
     if(!input(PIN_B0))
      break;
     delay_ms(200);
    }
    lcd_putc('\f');                             
    lcd_gotoxy(6, 1);                         
    lcd_putc("Month:");
    delay_ms(200);
    while(TRUE){
     if(!input(PIN_B1))
      month++;
     if(month > 12)
      month = 1;
     lcd_gotoxy(8, 2);                           
     printf(lcd_putc,"%02u", month);
     if(!input(PIN_B0))
      break;
     delay_ms(200);
    }
    lcd_putc('\f');                             
    lcd_gotoxy(6, 1);                            
    lcd_putc("Year:");
    lcd_gotoxy(7, 2);                            
    lcd_putc("20");
    delay_ms(200);
    while(TRUE){
     if(!input(PIN_B1))
      year++;
     if(year > 99)
      year = 0;
     lcd_gotoxy(9, 2);                         
     printf(lcd_putc,"%02u", year);
     if(!input(PIN_B0)){
      // chuyen ma BCD thanh so thap phan
      minute = ((minute / 10) << 4) + (minute % 10);
      hour = ((hour / 10) << 4) + (hour % 10);
      date = ((date / 10) << 4) + (date % 10);
      month = ((month / 10) << 4) + (month % 10);
      year = ((year / 10) << 4) + (year % 10);
     
      ds1307_write(1, minute);                 
      ds1307_write(2, hour);
      ds1307_write(4, date);
      ds1307_write(5, month);
      ds1307_write(6, year);
      ds1307_write(0, 0);                       
      delay_ms(200);
      break;
     }
     delay_ms(200);
    }
   }
   i2c_start();                                  
   i2c_write(0xD0);                             
   i2c_write(0);                               
   i2c_start();                                 
   i2c_write(0xD1);                             
   second = i2c_read(1);                         
   minute = i2c_read(1);                       
   hour   = i2c_read(1);                        
   day    = i2c_read(1);                        
   date   = i2c_read(1);                         
   month  = i2c_read(1);                         
   year   = i2c_read(0);                        
   i2c_stop();                                  
   ds1307_display();                           
   delay_ms(50);
  }
}
