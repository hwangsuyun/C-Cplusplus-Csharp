#include <stdio.h>

int main()
{
	char str[100]="This is a test sentence. Hello World!";
	int i;

	printf("원문: %s\n",str);

	for(i=0;str[i]; i++)
	{
		if((str[i]>='a')&&(str[i]<='z'))//if(islower(str[i]))
		{
			str[i] = str[i]-'a'+'A';
		}
		else
		{
			if((str[i]>='A')&&(str[i]<='Z'))//if(isupper(str[i]))
			{
				str[i] = str[i]-'A'+'a';
			}
		}
	}

	printf("변환 후: %s\n",str);

	return 0;
}
