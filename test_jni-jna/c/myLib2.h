typedef void(*EventCallback)(char *, int);
void callbackTrigger(EventCallback callback);
void printHello();
int printString(char *str);
void callInnerFunction();
