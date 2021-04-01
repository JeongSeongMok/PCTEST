#include <stdio.h>
#include <stdlib.h>

int main(){

	int i;
	scanf("%d",&i);

	while(1)
	{
		printf("%d\n",i);
		if(!i) break;
		i/=2;
	}
	return 0;
}
