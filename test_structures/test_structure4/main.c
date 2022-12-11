#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	char name[3];
	char birth[8];
	char type[4];
} UBIPAY_51;

int main()
{
	UBIPAY_51 data;
	char buff[15 + 1];
	char name[3 + 1];
	char birth[8 + 1];
	char type[4 + 1];

	memset(name, 0x00, sizeof(name));
	memset(birth, 0x00, sizeof(birth));
	memset(type, 0x00, sizeof(type));
	memset(buff, 0x00, sizeof(buff));
	memcpy(buff, "hsy19840824test", sizeof(buff));

	// 구조체 선언 후 아래와 같이 하면 구조체에 복사가 됨...
	// 대신 딱 맞게 복사되는건 아니고 첫번째 변수는 두~세번째 변수의 합계만큼 나오고, 두번째 변수는 세번째 변수의 합계만큼 나옴
	// 그래서 구조체의 값들을 일일히 다른 변수에 담아줘야 함
	// 다른 변수에 담을 때 팁은 구조체 선언시 각 캐릭터 배열에 널값은 제외하고 배열의 크기를 선언하고
	// 이 값들을 다른 캐릭터 배열에 담을 때는 다른 캐릭터 배열엔 크기를 널값을 선언하고
	// 복사할때 사이즈는 널값을 제외한 사이즈만큼 복사해야됨... 캐릭터 크기만큼 복사하면 널값이
	// 안들어가서 이상하게 로그가 찍힘
	memset((char *)&data, 0x00, sizeof(data));
	memcpy((char *)&data, buff, sizeof(data));

	printf("buff(%s)\n", buff);
	printf("data.name(%s)\n", data.name);
	printf("data.birth(%s)\n", data.birth);
	printf("data.type(%s)\n", data.type);

	memcpy(name, data.name, sizeof(data.name));
	memcpy(birth, data.birth, sizeof(data.birth));
	memcpy(type, data.type, sizeof(data.type));
	
	printf("name(%s)\n", name);
	printf("birth(%s)\n", birth);
	printf("type(%s)\n", type);
	
	return 0;
}
