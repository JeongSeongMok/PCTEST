#include <stdio.h>
#include <stdlib.h>

int fibo(int n){
	if(n<2) return n;
	return fibo(n-1)+fibo(n-2);
}

int main()
{

	char *temp;
	
	int i;
	scanf("%d", &i);

	if(i==1) {while(1);} //timeout
	else if(i==2) { printf("12341234\n"); return 0;}//wrong output
	else if(i==3) { sprintf(temp,"sfasd"); return 0;}//segnemtation fault
	else if(i==4) { abort();} //wrong termination
	else if(i==5) { fibo(250);} //too much time 
	else {                    //correct output
		while(1){
			printf("%d\n",i);
			if(!i) break;
			i/=2;
		}
	}
	return 0;
}
