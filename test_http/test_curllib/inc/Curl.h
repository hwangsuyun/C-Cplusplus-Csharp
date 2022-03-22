/***************************************************************************
 *  *                                  _   _ ____  _
 *  *  Project                     ___| | | |  _ \| |
 *  *                             / __| | | | |_) | |
 *  *                            | (__| |_| |  _ <| |___
 *  *                             \___|\___/|_| \_\_____|
 *  *
 *  * Copyright (C) 1998 - 2015, Daniel Stenberg, <daniel@haxx.se>, et al.
 *  
 *  * This software is licensed as described in the file COPYING, which
 *  * you should have received as part of this distribution. The terms
 *  * are also available at https://curl.haxx.se/docs/copyright.html.
 *  
 *  * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 *  * copies of the Software, and permit persons to whom the Software is
 *  * furnished to do so, under the terms of the COPYING file.
 *  
 *  * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 *  * KIND, either express or implied.
 *  
 *  ***************************************************************************/
/* <DESC>
 * simple HTTP POST using the easy interface
 * </DESC>
 */

/*********************************Curl 라이브러리 응답값 수신 함수*/
typedef struct
{
        char*   data;   // 버퍼
        size_t  size;   // 사이즈
}HTMLCONTEXTDATA;

size_t callbackWriteMemory(void *ptr, size_t size, size_t nmemb, void *stream)
{
        size_t realsize = size * nmemb;
        HTMLCONTEXTDATA *mem = (HTMLCONTEXTDATA *)stream;

        mem->data = (char*)realloc(mem->data, mem->size + realsize + 1);
        if (mem->data) {
                memcpy(&(mem->data[mem->size]), ptr, realsize);
                mem->size += realsize;
                mem->data[mem->size] = '\0';
        }

        return realsize;
}

static void freeHtmlContextData(HTMLCONTEXTDATA *p)
{
        p ? (free(p->data), p->data = NULL, p->size = 0) : 1;
}
