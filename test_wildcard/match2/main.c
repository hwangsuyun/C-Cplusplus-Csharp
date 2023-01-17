#include <stdio.h>
#include <string.h>

int is_match(char* line, char* pattern)
{
    int wildcard = 0;
    char* last_pattern_start = 0;
    char* last_line_start = 0;

    while (*line)
	{
        if (*pattern == '*')
        {
	//1. 와일드카드(*)일 경우 
	// 1) 와일드카드(*)가 패턴문자의 마지막일 경우
	//   1-1) 1 리턴
	// 2) 와일드카드(*) Flag ON!
	// 3) 마지막 와일드카드(*) 위치 등록
	// 4) pattern 만 1 증가(pattern에 매칭되는 문자가 여러개 일 수 있기 때문에)
            if (*(pattern+1) == '\0')
                return 1;

            last_pattern_start = pattern;
            wildcard = 1;
            pattern++;
        }
        else if (*pattern == *line || *pattern == '?')
        {
	//1. 문자가 같을 경우 || 와일드카드(?)일 경우
	// 1) 와일드카드(*) Flag가 켜져있으면 
	//   1-1) line의 시작위치 1증가
	//   1-2) 와일드카드(*) Flag OFF
	// 2) pattern과 line 1씩 증가
            if(wildcard == 1){
                last_line_start = line + 1;
	}

            line++;
            pattern++;
            wildcard = 0;
        }
        else if (wildcard)
        {
	//1. 두 문자가 다름
	//2. 와일드카드 문자가 아님
	//3. 와일드카드(*) Flag가 켜져있음
	// 1) line 1 증가
	line++;
        }
        else
        {
	//1. 두 문자가 다름
	//2. 와일드카드 문자가 아님
	//3. 앞에 와일드카드(*)이 존재하지 않음
	//4. ** 두 문자열 비교 중 **
	if (last_pattern_start != 0) // try to restart the mask on the rest
	{
		//1. 와일드카드(*)에 속하는 여러 문자 중 패턴에 매칭되는 중복 문자로인해 와일드카드(*) Flag가 꺼진 경우 대비
		// 1) pattern 위치를 와일드카드(*)가 존대했던 위치로 변경
		// 2) line위치를 마지막 라인의 위치로 변경
		pattern = last_pattern_start;
		line = last_line_start;
		last_line_start = 0;
	}
	else
	{
		//1. 조건 없이 다르면 0 리턴
		return 0;
	}
        }

    }

    if (*pattern == '\0' || (*pattern == '*' && strlen(pattern+1) == 0))
        return 1;
    else
        return 0;
}

int main()
{
    char line[100];
    char pattern[100];
    int ret = 0;

    memset(line, 0x00, sizeof(line));
    memset(pattern, 0x00, sizeof(pattern));
    memcpy(line, "ab*cdefxxxcdef", sizeof(line)); // * in the path, process die
    //memcpy(line, "*abc*defxx*x*cdef*", sizeof(line));
    memcpy(pattern, "*cdef*cdef*", sizeof(pattern));

    ret = is_match(line, pattern);
    printf("ret : (%d)\n", ret);

    return 0;
}
