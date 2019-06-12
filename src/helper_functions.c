int split(char *arg, char *value)
{
    if(!arg)
        return -1;

    char ch;
    int len = 0;
    int loc = 0;
    int max_len = strlen(arg);
    for(len=0, ch=arg[0]; ch != '\0'; ch=arg[len++])
    {
        if(ch == '\0')
            return -1;
        if(ch == '=')
            loc = len;
    }
    if(loc == 0)
        return -1;
    for(int n=0; n<len; n++)
        value[n] = arg[n+loc];
    arg[loc-1] = '\0';
    return 0;
}