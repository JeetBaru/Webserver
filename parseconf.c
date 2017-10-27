#include "parseconf.h"

char * getsecondword(char * str)
{
	while( *str !=' ')
	{
		if(*str == '\0')
			return NULL;
		str++;
	}
	
	str++;	

	*(str+strlen(str)-1) = '\0';

	return str;
}

char * getitem(char * word)
{
	char line[100];
	char * rv;

	FILE * fp;
	fp = fopen("ws.conf","r");
	
	while(fgets(line,sizeof(line),fp) != NULL)
	{
		if(strstr(line,word)!=NULL)
		{
			rv = getsecondword(line);
			fclose(fp);
			return rv;
		}
		bzero(line,sizeof(line));
	}
	fclose(fp);
	return '\0';			
}

char * getportnumber()
{
	return getitem("listen");
}

char * getroot()
{
//	printf("%s\n",getitem("root"));
	return getitem("root");
}

char * getdefaultpage()
{
	return getitem("defaultwebpage");
}
/*
int main()
{
	char * buff;
	buff = getportnumber();
	printf("Port Number = %s\n",buff);

	printf("Root = %s",getroot());
	
	printf("Default page = %s",getdefaultpage());

	printf("File content = %s\n",getitem("txt"));
	
	return 0;

}
*/
