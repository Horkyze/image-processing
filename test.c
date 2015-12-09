#include <stdio.h>
double A_get_alpha(int a, int x, int y){
	double d = 255;

	return (a == 0 )? 0 : (a / 255.0);
}

int main(int argc, char const *argv[])
{
	int i;
	for (i = 0; i < 255; i++) {
		printf("%lf\n", A_get_alpha(i, 1, 1) );
	}
	return 0;
}
