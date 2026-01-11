#include "HD44780_ToFile.h"

int do_HD44780_ToFile_test()
{
	HD44780_ToFileBase lcd{};

	lcd.Init();

	lcd.Clear();

	lcd.SetCursorPos(0, 0);
	lcd.Print("Hello, World!");

	lcd.SetCursorPos(1, 0);
	lcd.Print("HD44780 Test");

	return 0;
}
