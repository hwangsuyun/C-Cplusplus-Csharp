#include <stdio.h>
#include <string.h>

char W[1024];
char T[1024];
int dp[101][101];

int memoization(int w, int t)
{
    if (dp[w][t] != -1) return dp[w][t];

    while (w < strlen(W) && t < strlen(T) && (W[w] == T[t] || W[w] == '?'))
    {
        dp[w][t] = memoization(w + 1, t + 1);
        return dp[w][t];
    }
    if (w == strlen(W))
    {
        dp[w][t] = (t == strlen(T)) ? 1 : 0;
        return dp[w][t];
    }

    if (W[w] == '*')
    {
        if (memoization(w + 1, t) || (t < strlen(T) && memoization(w, t + 1)))
        {
            dp[w][t] = 1;
            return dp[w][t];
        }
    }
    dp[w][t] = 0;
    return dp[w][t];
}

int main()
{
    int type = 26;

    for (int i = 0; i < 101; i++)
    {
        memset(dp, -1, sizeof(dp));
    }

    switch (type)
    {
        case 0:
            // true
            memcpy(W, "abcdefg", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 1:
            // false
            memcpy(W, "aacdefg", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 2:
            // true
            memcpy(W, "*cde*", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 3:
            // false
            memcpy(W, "cde*", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 4:
            // false
            memcpy(W, "*cde", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 5:
	// false
            memcpy(W, "**cde", sizeof(W));
            memcpy(T, "abcdfcdefg", sizeof(T));
            break;
        case 6:
            // true
            memcpy(W, "**cde*", sizeof(W));
            memcpy(T, "abcdfcdefg", sizeof(T));
            break;
        case 7:
            // false
            memcpy(W, "*cdef*cdef", sizeof(W));
            memcpy(T, "abcdexxxcdefgcc", sizeof(T));
            break;
        case 8:
            // true
            memcpy(W, "*xxx*?g", sizeof(W));
            memcpy(T, "abcxxdexxxcdefg", sizeof(T));
            break;
        case 9:
            // false
            memcpy(W, "*cdef*cdef*", sizeof(W));
            memcpy(T, "abcdexxxcdefg", sizeof(T));
            break;
        case 10:
            // false
            memcpy(W, "*cdf*cdef*", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 11:
            // true
            memcpy(W, "*", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 12:
            // true
            memcpy(W, "*", sizeof(W));
	memcpy(T, "", sizeof(T));
            break;
        case 13:
            // true
            memcpy(W, "?", sizeof(W));
            memcpy(T, "d", sizeof(T));
            break;
        case 14:
            // true
            memcpy(W, "*?", sizeof(W));
            memcpy(T, "ddd", sizeof(T));
            break;
        case 15:
            // true
            memcpy(W, "?*", sizeof(W));
            memcpy(T, "ddd", sizeof(T));
            break;
        case 16:
            // true
            memcpy(W, "???", sizeof(W));
            memcpy(T, "ddd", sizeof(T));
            break;
        case 17:
            // true
            memcpy(W, "*?*", sizeof(W));
            memcpy(T, "ddd", sizeof(T));
            break;
        case 18:
            // false
            memcpy(W, "?", sizeof(W));
            memcpy(T, "", sizeof(T));
            break;
        case 19:
            // true
            memcpy(W, "*?cde*", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
	break;
        case 20:
            // true
            memcpy(W, "??cde??", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 21:
            // false
            memcpy(W, "??cde?", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 22:
            // false
            memcpy(W, "ppppp", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 23:
            // flase
            memcpy(W, "??c??", sizeof(W));
            memcpy(T, "abcdefg", sizeof(T));
            break;
        case 24:
            // true
            memcpy(W, "*cdef*cdef?", sizeof(W));
            memcpy(T, "abcdefxxxcdefg", sizeof(T));
            break;
        case 25:
            // true
            memcpy(W, "*cdef*cdef*", sizeof(W));
            memcpy(T, "abcdefxxxcdefg", sizeof(T));
            break;
        case 26:
            // false
            memcpy(W, "*cdef*cdef", sizeof(W));
            memcpy(T, "abcdefxxxcdefg", sizeof(T));
            break;
        default:
	break;
    }

    if (memoization(0, 0))
    {
        printf("wild card is true, W : (%s), T : (%s)\n", W, T);
    }
    else
    {
        printf("wild card is false, W : (%s), T : (%s)\n", W, T);
    }

    return 0;
}