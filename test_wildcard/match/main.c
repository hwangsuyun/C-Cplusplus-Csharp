#include <stdio.h>
#include <string.h>

int is_match(char* line, char* pattern)
{
    // returns 1 (true) if there is a match
    // returns 0 if the pattern is not whitin the line
    int wildcard = 0;

    char* last_pattern_start = 0;
    char* last_line_start = 0;
    do
    {
        if (*pattern == *line)
        {
            if(wildcard == 1)
                last_line_start = line + 1;

            line++;
            pattern++;
            wildcard = 0;
        }
        else if (*pattern == '?')
        {
            if(*(line) == '\0') // the line is ended but char was expected
                return 0;
            if(wildcard == 1)
                last_line_start = line + 1;
            line++;
            pattern++;
            wildcard = 0;
        }
        else if (*pattern == '*')
        {
            if (*(pattern+1) == '\0')
            {
                return 1;
	}

            last_pattern_start = pattern;
            //last_line_start = line + 1;
            wildcard = 1;

            pattern++;
        }
        else if (wildcard)
        {
            /*
            if (*line == *pattern)
            {
                wildcard = 0;
                line++;
                pattern++;
                last_line_start = line + 1 ;
            }
            else
            {
	*/
                line++;
            //}
        }
        else
        {
            if ((*pattern) == '\0' && (*line) == '\0')  // end of mask
                return 1; // if the line also ends here then the pattern match
            else
            {
                if (last_pattern_start != 0) // try to restart the mask on the rest
                {
                    pattern = last_pattern_start;
                    line = last_line_start;
                    last_line_start = 0;
                }
                else
                {
                    return 0;
	   }
            }
        }

    } while (*line);


    if (*pattern == '\0')

    {
        return 1;
    }
    else
    {
        if ((*pattern == '?' || *pattern == '*') && strlen(line) == 0)
        {
            return 1;
        }
        return 0;
    }
}

int main()
{
    char line[100];
    char pattern[100];
    int ret = 0;

    memset(line, 0x00, sizeof(line));
    memset(pattern, 0x00, sizeof(pattern));
    memcpy(line, "babbbc", sizeof(line));
    memcpy(pattern, "*bb*", sizeof(pattern));

    ret = is_match(line, pattern);
    printf("ret : (%d)\n", ret);

    return 0;
}