#! /usr/bin/python
import os
import time
import sys
buildpath=sys.argv[1]
clfuture=buildpath+"/programs/clfuture/clfuture"
contractPath=buildpath+"/contract/"
#Keys=($($clfuture create key | awk -F: '{print $2}'))
#PrivKey=${Keys[0]}
#PubKey=${Keys[1]}
PubKey="FUT6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV"
PrivKey="5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"
print("pub:"+PubKey)
print("pri:"+PrivKey)
### create default wallet
#WalletPwd=$($clfuture wallet create | tail -n 1 | sed 's/\"//g')
#if test -z $WalletPwd
#then
#   echo "Wallet password is empty, quit."
#   exit 0
#fi
# echo "Password for default, please save it safely : " $WalletPwd
# WalletPwd=PW5JDL5AjogThU1avkC3cxE7xThEmeeya2bmvrZqJxYCcGq2UWY6B
# $clfuture wallet unlock --password $WalletPwd 
oscmd=clfuture+" wallet import "+ PrivKey
os.system(oscmd)
time.sleep(1)
usrList=["user","tester","exchange","futio.msig","futio.stake","futio.token","futio.resfee,"futio.fee"]
for username in usrList:
    oscmd=clfuture+" create account futureio "+username+ " "+PubKey+" "+PubKey
    print (oscmd)
    os.system(oscmd)


oscmd=clfuture+" set contract futio.token "+buildpath+"/contracts/futureio.token -p futio.token "
os.system(oscmd)
time.sleep(1)
oscmd=clfuture+" push action futio.token create \'[ \"futureio\", \"1000000000.0000 SYS\", 0, 0, 0]\' -p futio.token"
os.system(oscmd)
time.sleep(1)
oscmd=clfuture+" push action futio.token issue \'[ \"futureio\", \"300000000.0000 SYS\", \"memo\"]\' -p futureio"
os.system(oscmd)
time.sleep(1)
oscmd=clfuture+" set contract futureio "+buildpath+"/contracts/futureio.system -p futureio"
os.system(oscmd)
time.sleep(1)
oscmd=clfuture+" set contract futureio "+buildpath+"/contracts/futureio.system -p futureio "
os.system(oscmd)
