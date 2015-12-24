# include	<stdio.h>
# include	<sys/types.h>
# include	<errno.h>

int read_en_master_key ( master,pin )
char	master[],pin[];
{
	/*static first_in = 1;
	int	flag = 0;*/
	char	masterkey[17], pinkey[17];

	
        /*
	if( first_in ){
		$ select pinkey, masterkey
		    into $pinkey, $masterkey
		    from tspt_key;
		if ( sqlca.sqlcode != 0 ) {
			return -2;
		}
		first_in = 0;
	}
        */
        strcpy( masterkey, "1234123412341234" );
        strcpy( pinkey,    "1234123412341234" );

	strcpy ( master, masterkey );
	strcpy ( pin, pinkey );

	return 0;
}


killblank(s)
char s[];
{
	int i;
	i=strlen(s)-1;
	while(s[i]==' ')
		s[i--]='\0';
}

int CheckDac(inbuf,outbuf,indac)
char    inbuf[];
char    outbuf[];
char    indac[];
{

	S_DAC(inbuf, outbuf);

	return(strncmp(indac,outbuf,16));
}
