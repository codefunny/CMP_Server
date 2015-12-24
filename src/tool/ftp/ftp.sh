if [ $# -ne 1 ] ; then
  echo "Usage: $0 start|stop"
  exit 1
fi

cmd=$1
pro=$0

case $cmd in
start)
  myftpd -p 21221 -c $MYROOT/etc/usrlist -u `echo $UPLOAD_PATH` -d `echo $DOWNLOAD_PATH` 
  ;;
stop)
  klprc myftpd > /dev/null
  ;;
*)
  echo "Usage: $pro start|stop"
  ;;
esac
