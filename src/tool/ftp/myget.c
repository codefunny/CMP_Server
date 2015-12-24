#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include "myftpcli.h"

int main( int argc, char *argv[] )
{
   int ret;
   struct timeval t1, t2;

   if( argc != 8 )
   {
      printf( "Usage: %s ip port organ operator pass remotefile localfile\n",
              argv[0] );
      exit( 0 );
   }

   gettimeofday( &t1, NULL );
   ret = myftp_get( argv[1], atoi(argv[2]), argv[3], argv[4], argv[5], argv[6],
                    argv[7] );
   gettimeofday( &t2, NULL );
   if( ret != 0 )
   {
      printf( "ret = %d errno = %d reason = %s\n",
              ret, myerrno, strerror(errno) );
   }

   printf( "%d | %06d\n", t1.tv_sec, t1.tv_usec );
   printf( "%d | %06d\n", t2.tv_sec, t2.tv_usec );

   return 0;
}
