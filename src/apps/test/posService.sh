TARGET=posService.h 
WEB=http://192.168.20.26:8800/A-Pay/services/posService?wsdl
wsdl2h -c -o $TARGET $WEB
sleep 1
soapcpp2 -c -C -x $TARGET
echo "The script run over!"

