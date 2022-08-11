#include "cthulhu.h"

void
buserr()
{
	STATE("HEY, I just had a bus error...");
	Quit();
}

void
segerr()
{
	STATE("Hey, I just had a segmentation violation...");
	Quit();
}
