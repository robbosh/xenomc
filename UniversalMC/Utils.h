void WideToChar(char* Dest, const WCHAR* Source)
{
	int i = 0;

	while(Source[i] != '\0')
	{
		Dest[i] = (CHAR)Source[i];
		++i;
	}

	Dest[i] = '\0';
}