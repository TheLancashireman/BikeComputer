#ifndef LIQUIDCRYSTAL_H
#define LIQUIDCRYSTAL_H	1

#include <Arduino.h>
#include <stdio.h>

class LiquidCrystal
{
public:
	LiquidCrystal(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
	{
	}

	static inline void setCursor(uint8_t col, uint8_t row)
	{
		printf("\n LCD(%d,%d): ", col, row);
	}

	static inline int write(const char *s)
	{
		return printf("%s", s);
	}

	static inline int write(char c)
	{
		putc(c, stdout);
		return 1;
	}

	static inline int print(const char *s)
	{
		return printf("%s", s);
	}

	static inline int print(unsigned n)
	{
		return 0;
	}

	static inline int print(double d, int n)
	{
		return printf("%.2f", d);
	}

	static inline int begin(uint8_t ncol, uint8_t nrow)
	{
		return 0;
	}

	static inline int command(uint8_t cmd)
	{
		return 0;
	}

	static inline int display(void)
	{
		return 0;
	}

#if 0
	static inline void clear_row(uint8_t r)
	{
		printf("\n");
	}
#endif
};

#endif
