#!/bin/bash
NAME=$1
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "killall 'nodfuture' "
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "killall 'kfutured' "
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "killall 'wssfuture' "
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "killall 'sleep' "
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "killall 'logrotate.sh' "
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "rm -rf /root/.local/share/futureio/nodfuture "
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "rm -rf /root/.local/share/futureio/wssfuture "
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "rm -rf /root/log/ "
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "rm -rf /log/ "
docker ps | grep $NAME- | awk '{print $1}' | xargs -i docker exec -d {} bash -c "killall  rand.sh"
