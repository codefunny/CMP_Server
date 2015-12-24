#       The information in this file is provided "AS IS" without warranty.
#
# .profile      -- Commands executed by a login Korn shell
#

#for informix system
INFORMIXDIR=/home/informix
LD_LIBRARY_PATH=$INFORMIXDIR/lib:$INFORMIXDIR/lib/esql:$HOME/bin

#for informix application
INFORMIXSERVER=easylinksvr
ONCONFIG=onconfig.mid

LANG=english_us.8859
CONFIGFILEPATH=$HOME/etc

PATH=$INFORMIXDIR/bin:/bin:/usr/bin:/usr/sbin:$HOME/EasyLink/bin:$HOME/EasyLink/shdir:$HOME/bin:.:/usr/local/bin

export PATH INFORMIXDIR INFORMIXSERVER ONCONFIG LD_LIBRARY_PATH LANG CONFIGFILEPATH

eval `tset -m ansi:${TERM:-ansi} -m :\?${TERM:-ansi} -e -r -s -Q`
PS1='$PWD>'
export PS1 TERM

set -o vi

#umask 066

TRACE_PATH=/home/ifx/log/
TRACE=1
TRACE_LEVEL=4
export TRACE_PATH TRACE TRACE_LEVEL

EFTPEXECDIR=/home/easylink/bin/
EFTPCONGDIR=/home/easylink/easyftp/int/
export EFTPEXECDIR EFTPCONGDIR

# for houtai host mac check
#CHKMAC=TRUE
#export CHKMAC

alias rm='rm -i'

. setenv

#for easylink enviorment
CHANNEL_CONFIG_PATH=$HOME/conf       # storage channel port and name map node id
EASYLINK_PATH=$HOME/bin              # easylink execute file path
EASYLINK_NODEID=116
export CHANNEL_CONFIG_PATH EASYLINK_PATH EASYLINK_NODEID

export SENDDIR=$HOME/senddir
export RECVDIR=$HOME/recvdir

HZ=2000
FILE_DATA_DIR=$HOME/IbsFileData
export FILE_DATA_DIR HZ
FILEPWD=/home/agent/filetest;export FILEPWD
alias dir="lf -a"

DBPATH=$HOME/form
export DBPATH

