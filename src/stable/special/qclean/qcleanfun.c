#include "qclean/qclean.h"

double GetCurrentMillTime()
{
    struct   timeval  tv ;
    double   millnum;
    double   dTmp1;
    double   dTmp2;

    gettimeofday (&tv, NULL );

     dTmp1 = tv.tv_sec;

        millnum = dTmp1 * 1000 + tv.tv_usec/1000 ;
    return( millnum );
}

int Myusleep(unsigned int usec) 
{
    static subtotal = 0;        /* microseconds */
    int msec = 0;                   /* milliseconds */

    /* 'foo' is only here because some versions of 5.3 have
    * a bug where the first argument to poll() is checked
    * for a valid memory address even if the second argument is 0.
    */
    struct pollfd foo;

    subtotal += usec;
    /* if less then 1 msec request, do nothing but remember it
       if (subtotal < 1000) return(0);*/
    msec = subtotal/1000;
    subtotal = subtotal%1000;
    return poll(&foo,(unsigned long)0,msec);
}

