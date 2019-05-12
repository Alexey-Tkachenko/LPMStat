#include "LcdIo.h"
#include "libs/LCD/LiquidCrystal_I2C.h"
#include "Globals.h"

const byte LCD_ROWS = 2;
const byte LCD_COLS = 16;


LiquidCrystal_I2C lcd((byte)I2C::LCD, LCD_COLS, LCD_ROWS);


void lcdClear()
{
	lcd.clear();
}

void lcdClearLine(byte line)
{
	lcd.setCursor(0, line);
	for (byte i = 0; i < LCD_COLS; ++i)
	{
		lcd.write(' ');
	}
}

void lcdInit()
{
	lcd.begin();
    lcd.noAutoscroll();
    lcd.noCursor();
    lcd.noBlink();
    

    byte c[8];
    memset(c, 0, 8);

    c[0] = B00110;
    c[1] = B01001;
    c[2] = B01001;
    c[3] = B00110;

    lcd.createChar(SYMBOL_DEG, c);

    c[0] = B00010;
    c[1] = B00010;
    c[2] = B00100;
    c[3] = B00100;
    lcd.createChar(SYMBOL_MIN, c);

    c[0] = B01001;
    c[1] = B01001;
    c[2] = B10010;
    c[3] = B10010;
    lcd.createChar(SYMBOL_SEC, c);

    c[0] = B00000;
    c[1] = B11111;
    c[2] = B11011;
    c[3] = B10001;
    c[4] = B10001;
    c[5] = B10001;
    c[6] = B11011;
    c[7] = B11111;
    lcd.createChar(SYMBOL_START, c);

    c[0] = B00000;
    c[1] = B11111;
    c[2] = B11011;
    c[3] = B10101;
    c[4] = B11111;
    c[5] = B10101;
    c[6] = B11011;
    c[7] = B11111;
    lcd.createChar(SYMBOL_STOP, c);


    lcd.setBacklight(0xFF);
}

void lcdPrint(const char *a, const char *b, bool clear)
{
    if (clear)
    {
        lcdClear();
    }

    if (a)
    {
        lcd.setCursor(0, 0);
        lcd.print(a);
    }

    if (b)
    {
        lcd.setCursor(0, 1);
        lcd.print(b);
    }
}


void lcdPrint(const __FlashStringHelper *a, const __FlashStringHelper *b, bool clear)
{
	if (clear)
	{
		lcdClear();
	}

	if (a)
	{
		lcd.setCursor(0, 0);
		lcd.print(a);
	}

	if (b)
	{
		lcd.setCursor(0, 1);
		lcd.print(b);
	}
}

void lcdPutCharAt(byte row, byte col, char symbol)
{
	lcd.setCursor(col, row);
	lcd.write(symbol);
}

void lcdPutLineAt(byte row, byte col, const char* symbols)
{
	lcd.setCursor(col, row);
	lcd.print(symbols);
}

void lcdPutLineAt(byte row, byte col, const __FlashStringHelper* symbols)
{
	lcd.setCursor(col, row);
	lcd.print(symbols);
}

void lcdSetBacklight(bool value)
{
    lcd.setBacklight(value ? 1 : 0);
}

