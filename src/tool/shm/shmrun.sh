if [ $# -lt 1 ] ; then

   echo "shmrun databasename"

   exit 1

fi



dbaccess $1 - - << !

        unload to $HOME/data/chkcfg.txt select svcid,cursubseq,condid,nextsubseq,dfltflag from chkcfg order by svcid, cursubseq;

        unload to $HOME/data/constcond.txt select condid,layerid,subseq,msgdscrb,fldseq,fldtype,startbit,endbit,maxvalue,minvalue,flag,maxflag,minflag from constcond order by condid, layerid;

        unload to $HOME/data/msgfldcfg.txt select msgappid,fldseq,sepfldseq,fldengname,fldchnname,sepfldflag,lenfldlen,fldlen,lenfldtype,fldtype,lenflddef,flddef,lenfldalign,fldalign,sepsymbol from msgfldcfg order by msgappid, fldseq, sepfldseq;

        unload to $HOME/data/msgnameid.txt select msgappid,msgappname from msgnameid;

        unload to $HOME/data/nodecommcfg.txt select nodeid,nodename,nodedscrb,netproto,linktype,syncflag,hostip,hostname,hostcommport,monitorip,mtrhostname,mtrport,msgappid,encrpttype,minprocess,maxprocess,usableflag,svrkeyid,svwkeyid from nodecommcfg order by nodeid;

        unload to $HOME/data/nodetrancfg.txt select nodeid,msgappid,msgappname,tomidtrancodeid,tomsgtrancodeid,tomidid,tomsgid,tomidisst,tomsgisst,crttrancodetype,crttrancodepos,crttrancodeseq,crtmsgfixfld,wrgcodetype,wrgtrancodepos,wrgtrancodeseq,wrgmsgfixfld,logtabname,logfld,maxretrytime,delaytime from nodetrancfg order by nodeid;

        unload to $HOME/data/relacond.txt select * from relacond order by condid, layerid;

        unload to $HOME/data/service.txt select nodeid,trancode,svcid,svcname,timeout,balanceflag,svcnote from service order by svcid, nodeid, trancode;

        unload to $HOME/data/servicecfg.txt select chnnodeid,svctrancode,svcid,hostnodeid,subseq,subid,crckind,crcsubid,retrytime,outtime,parentsubseq,sublayer,batflag,batmode,lstid from servicecfg order by svcid, subseq;

        unload to $HOME/data/subservice.txt select nodeid,trancode,subid,subname,templetid,trancodeseq,succcode,recordspertime,msgappid from subservice order by subid, nodeid, trancode;

        unload to $HOME/data/subsvcfldsrccfg.txt select fldasmid,fldfragseq,srcinftype,srcsubseq,srcfldseq,srcrepfldflag,srcbeginpos,srclength,srcnote from subsvcfldsrccfg order by fldasmid, fldfragseq;

        unload to $HOME/data/subsvcreqcfg.txt select svcid,subseq,fldseq,batsubseq,batsubfldseq,fldtype,repfldflag,reptmsubseq,reptmfld,reptmtype,fldfragcalflag,fldasmid,balflag from subsvcreqcfg order by svcid, subseq, fldseq;

        unload to $HOME/data/tranfldcfg.txt select nodeid,trancode,msgdscrb,reptimesseq,reptimesnum,reptype,tranfld from tranfldcfg order by nodeid, trancode, msgdscrb;

        unload to $HOME/data/retcodemap.txt select hostnodeid,hostretcodetype,channodeid,chanretcodetype,elretcode,chanretcode,hostretcode,errormsg from retcodemap;

        unload to $HOME/data/trancode.txt select city_code,t_tx_kind,t_tx_code,t_tx_name,v_tx_code,v_req_code,v_tx_name,cash_flag,k_tx_code,k_tx_name,o_tx_code,o_tx_name,busi_code,ag_unit_no,state from trancode;

        unload to $HOME/data/kernsvc.txt select node_id,sub_tran_code,serv_name from kernel_serv;

        unload to $HOME/data/commroute.txt select nodeid,commtype,routecond,routepara from commroute;

        unload to $HOME/data/fldmap.txt select source,target from fldmap;

!

