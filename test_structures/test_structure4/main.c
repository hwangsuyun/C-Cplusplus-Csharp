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

	// ����ü ���� �� �Ʒ��� ���� �ϸ� ����ü�� ���簡 ��...
	// ��� �� �°� ����Ǵ°� �ƴϰ� ù��° ������ ��~����° ������ �հ踸ŭ ������, �ι�° ������ ����° ������ �հ踸ŭ ����
	// �׷��� ����ü�� ������ ������ �ٸ� ������ ������ ��
	// �ٸ� ������ ���� �� ���� ����ü ����� �� ĳ���� �迭�� �ΰ��� �����ϰ� �迭�� ũ�⸦ �����ϰ�
	// �� ������ �ٸ� ĳ���� �迭�� ���� ���� �ٸ� ĳ���� �迭�� ũ�⸦ �ΰ��� �����ϰ�
	// �����Ҷ� ������� �ΰ��� ������ �����ŭ �����ؾߵ�... ĳ���� ũ�⸸ŭ �����ϸ� �ΰ���
	// �ȵ��� �̻��ϰ� �αװ� ����
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
