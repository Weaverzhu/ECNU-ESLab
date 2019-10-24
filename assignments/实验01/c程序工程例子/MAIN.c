#include<stdio.h>
int main()
	{
	/*第一个例子：从1加到10，最后输出结果sum*/
	int i,sum;
	sum=0;
	for(i=1;i<=10;i++)
	{
		sum=sum+i;
	}
	printf("sum=%d\n",sum);
    while(1);
}
