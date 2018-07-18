#include "LcdIo.h"
#include <LiquidCrystal_I2C.h>
#include "Globals.h"


LiquidCrystal_I2C lcd((byte)I2C::LCD, 16, 2);

const byte LCD_ROWS = 2;
const byte LCD_COLS = 16;

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
	lcd.begin(16, 2);
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

void lcdSetBacklight(byte pwmValue)
{
    lcd.setBacklight(pwmValue);
}

