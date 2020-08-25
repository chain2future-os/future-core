#! /usr/bin/python
import os
import string
import sys
import time
buildpath=sys.argv[2]
clfuture=buildpath+"/programs/clfuture/clfuture"
PubKey="FUT6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV"
k=0
j=int(sys.argv[1])
for index in string.lowercase:
    if index=='a':
        index=""
        producer="futureio"+index
    else:
        producer="futureio"+index
        oscmd=clfuture+" system newaccount --buy-ram-kbytes 300 --stake-net '100.0000 SYS' --stake-cpu '100.0000 SYS' futureio "+producer+" "+PubKey
        os.system(oscmd)
        oscmd=clfuture+" push action futio.token transfer \'[ \"futureio\", "+producer+", \"10000001.0000 SYS\", \"m\" ]\' -p futureio"
        os.system(oscmd)
        #time.sleep(1)
        if k%2 == 0 :
            oscmd=clfuture+" system delegatebw "+producer+" "+producer+" \'80000.0000 SYS\' \'60000.0000 SYS\'"
            os.system(oscmd)
        
#        time.sleep(1)
	oscmd=clfuture+" system regproducer "+producer+" "+PubKey+" http://http-server-address:8888"
	os.system(oscmd)
	if k%2 == 1: 
	    oscmd=clfuture+" system delegatebw "+producer+" "+producer+" \'80000.0000 SYS\' \'60000.0000 SYS\'"
            os.system(oscmd)
    k+=1
    if k==j:
        break

