/*****************************************************************
*
*    Copyright (C), 2010-2020, NewLand Computer Co., Ltd.
*    Version:  VER1.0
*    Description: ϵͳ����/ֹͣ�ӳ���
*    History:
*
*    Function List:
*            main:���س���ʼ
*
*    date:   2010-7-20
*    author: zhou hong di
*    created: zhou hong di
*
*    date:
*    author:
*    modify description
*
*****************************************************************/
#include <stdlib.h>
#include "apr.h"
#include "apr_network_io.h"
#include "apr_errno.h"
#include "apr_general.h"
#include <errno.h>

#define STRLEN 128

/*
 *
 * Function: main
 * Description: Ӧ���������������������������ʼ
 * ����:
 *      destip    -   ��������IP��ַ
 *      port      -   ���������PORt
 *      command   -   start or stop 
 *      name      -   Ҫ�����ķ�������
 * Return:
 * Other:
 */

int main(int argc, char *argv[])
{
    apr_pool_t *context;
    apr_socket_t *sock;
    apr_size_t length;
    apr_status_t stat;
    int iPort;
    char datasend[STRLEN];
    char datarecv[STRLEN];
    char msgbuf[80];
    char *local_ipaddr, *remote_ipaddr;
    char *dest = "127.0.0.1";
    apr_port_t local_port, remote_port;
    apr_interval_time_t timeout = apr_time_from_sec(2);
    apr_sockaddr_t *local_sa, *remote_sa;

    setbuf(stdout, NULL);
    if (argc != 5 ) {
        printf( "Usage: %s dest port command name\n", argv[0] );
        return -1;
    }

    dest = argv[1];
    iPort = atoi( argv[2] );
    sprintf( datasend, "%s %s", argv[3],argv[4] );

    if (apr_initialize() != APR_SUCCESS) {
        fprintf(stderr, "Something went wrong\n");
        exit(-1);
    }
    atexit(apr_terminate);

    if (apr_pool_create(&context, NULL) != APR_SUCCESS) {
        fprintf(stderr, "Something went wrong\n");
        exit(-1);
    }

    stat = apr_sockaddr_info_get( &remote_sa, dest, APR_INET,
                                  iPort, 0, context );
    if( stat != APR_SUCCESS)
    {
        fprintf(stdout, "Address resolution failed for %s: %s\n", 
                dest, apr_strerror(stat, msgbuf, sizeof(msgbuf)));
        exit(-1);
    }

    stat = apr_socket_create( &sock, APR_INET, SOCK_STREAM, APR_PROTO_TCP,
                              context );
    if ( stat != APR_SUCCESS )
    {
        fprintf(stderr, "Couldn't create socket\n");
        exit(-1);
    }

    stat = apr_socket_timeout_set(sock, timeout);
    if (stat) {
        fprintf(stderr, "Problem setting timeout: %d\n", stat);
        exit(-1);
    }

    stat = apr_socket_connect(sock, remote_sa);

    if (stat != APR_SUCCESS) {
        apr_socket_close(sock);
        fprintf(stderr, "Could not connect: %s (%d)\n", 
		apr_strerror(stat, msgbuf, sizeof(msgbuf)), stat);
        fflush(stderr);
        exit(-1);
    }

    apr_socket_addr_get(&remote_sa, APR_REMOTE, sock);
    apr_sockaddr_ip_get(&remote_ipaddr, remote_sa);
    /* apr_sockaddr_port_get(&remote_port, remote_sa); */
    apr_socket_addr_get(&local_sa, APR_LOCAL, sock);
    apr_sockaddr_ip_get(&local_ipaddr, local_sa);
    /* apr_sockaddr_port_get(&local_port, local_sa); */

    length = STRLEN;
    if ((stat = apr_socket_send(sock, datasend, &length) != APR_SUCCESS)) {
        apr_socket_close(sock);
        fprintf(stderr, "Problem sending data: %s (%d)\n",
		apr_strerror(stat, msgbuf, sizeof(msgbuf)), stat);
        exit(-1);
    }
   
    length = STRLEN; 

    memset( datarecv, 0, sizeof(datarecv) );
    if ((stat = apr_socket_recv(sock, datarecv, &length)) != APR_SUCCESS) {
        apr_socket_close(sock);
        fprintf(stderr, "Problem receiving data: %s (%d)\n", 
		apr_strerror(stat, msgbuf, sizeof(msgbuf)), stat);
        exit(-1);
    }
    apr_socket_close(sock);
    fprintf(stderr, "%s\n", datarecv);
    return 0;

    fprintf(stdout, "OK\n");

    fprintf(stdout, "Client:  Shutting down socket.......");
    if (apr_socket_shutdown(sock, APR_SHUTDOWN_WRITE) != APR_SUCCESS) {
        apr_socket_close(sock);
        fprintf(stderr, "Could not shutdown socket\n");
        exit(-1);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "Client:  Closing down socket.......");
    if (apr_socket_close(sock) != APR_SUCCESS) {
        fprintf(stderr, "Could not shutdown socket\n");
        exit(-1);
    }
    fprintf(stdout, "OK\n");

    return 1;
}
