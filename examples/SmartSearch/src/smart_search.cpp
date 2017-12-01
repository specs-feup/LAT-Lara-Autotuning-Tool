#include <stdio.h>

int main() {
	int a = 0;
#pragma lara marker smartSearchTest
	{
		int x = 2000;
		for(int i = 0; i < x % 20; i++){
			int y = 1000 + x;
			int z = x;
			x = 6969;
			for(int j = 0; j < x + y; j++){
				printf("(%d,%d)", i, j);
			}
		}
   	}

	printf("Hello");
}