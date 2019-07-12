

#include <stdio.h>

#include <types.h>

u32 x = 1;
u32 y = 2;
u32 z = 0;
const char msg[] = "NIK %d\n";



u32 funct()
{
	
	x++;
	y += x;
	printf(msg, y);
	
	return y;
	
}

void func()
{
	x++;
	y += x;
	printf(msg, y);
	funct();
	
}
