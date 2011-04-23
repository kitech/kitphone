#!/bin/sh

# arg: <start|stop|restart|check|fix|incr|decr>

CTRL_SWITCHER_USER=liuguangzhao01
CTRL_SWITCHER_PASSWORD=xxxxxxxxx
## array vars
CONFIG_FILE=`readlink -f $0|xargs dirname`/skyserv.ini
# AUDIO_SWITCHER_USERS=(`cat $HOME/skyserv.ini|grep AUDIO_SWITCHER_USERS|awk -F= '{print $2}'`)
# AUDIO_SWITCHER_PASSWORDS=(`cat $HOME/skyserv.ini|grep AUDIO_SWITCHER_PASSWORDS|awk -F= '{print $2}'`)
AUDIO_SWITCHER_USERS=(`cat $CONFIG_FILE|grep AUDIO_SWITCHER_USERS|awk -F= '{print $2}'`)
AUDIO_SWITCHER_PASSWORDS=(`cat $CONFIG_FILE|grep AUDIO_SWITCHER_PASSWORDS|awk -F= '{print $2}'`)
AUDIO_SWITCHER_CNT=${#AUDIO_SWITCHER_USERS[@]}
AUDIO_SWITCHER_CNT=`expr $AUDIO_SWITCHER_CNT - 1` # first is not calcated

CFGDIR=$HOME/skype_cluster/gateways
PIDDIR=$HOME/skype_cluster/pids
LOGDIR=$HOME/skype_cluster/logs
VNCPORT_BASE=6000
SCRN_BASE=1000
SCR_CNT=20

# XVFB=/usr/bin/Xvfb -screen 2 120x120x16 :2
XVFB=/usr/bin/Xvfb
OBOX=/usr/bin/openbox
if [ -f /usr/bin/awesome ] ; then
    OBOX=/usr/bin/awesome
fi
XVNC=/usr/bin/x11vnc
SKYPE=/usr/bin/skype
if [ ! -f $SKYPE ] ; then
    SKYPE=$HOME/xfsdev/skype_static-2.1.0.81/skype
#    SKYPE=$HOME/xfsdev/skype_static-2.0.0.72/skype
fi
SKYSERV=./skyserv

# manage this pid, get by bash tricky
XVFB_PID=
OBOX_PID=
XVNC_PID=
SKYPE_PID=
SKYSERV_PID=

SCRN=

######## check arg
CMDLINE=$0
ACTION=$1
AUTOFIX=0
if [ x"$ACTION" = x"" ] ; then
    echo "What do you want to do?";
    echo "Usage: $CMDLINE <start|stop|restart|check|fix|incr|decr>";
    exit;
fi
if [ x"$ACTION" = x"fix" ] ; then
    ACTION=check
    AUTOFIX=1
fi

mkdir -p $CFGDIR $PIDDIR $LOGDIR
# select x11vnc listen internal IP
LSN_IP=`/sbin/ifconfig -a|grep "inet addr"|egrep "(192.168|172.24)"|awk '{print $2}'|awk -F: '{print $2}'|head -n 1`
if [ x"$LSN_IP" = x"" ] ; then
    LSN_IP=localhost
fi

# simpe check config validation
if [ $SCR_CNT -gt $AUDIO_SWITCHER_CNT ] ; then
    SCR_CNT=$AUDIO_SWITCHER_CNT;
    echo "Wrap screen count to SWITCHER count: $SCR_CNT";
fi

# general functions
function start_xvfb()
{
    local SCRN=$1
    local PID_FILE=$2

    ## start xvfb server
    $XVFB :$SCRN -pixdepths 32 -screen :$SCRN 800x600x8 &
    XVFB_PID=$!
    echo $XVFB_PID > $PIDDIR/xvfb_$SCRN.pid

    export DISPLAY=:$SCRN
    sleep 1 # wait for xvfb started

    return 1;
}
# SCRN=45678
# start_xvfb $SCRN $XVFB_PID_FILE;
# RET=$?
# echo $RET;
# exit;

function start_obox()
{
    local SCRN=$1
    local PID_FILE=$2

    export DISPLAY=:$SCRN
    #############
    $OBOX &
    OBOX_PID=$!
    echo $OBOX_PID > $PIDDIR/obox_$SCRN.pid

    return 1;
}

function start_skype()
{
    local SCRN=$1
    local PID_FILE=$2

    export DISPLAY=:$SCRN
    # start skype
    mkdir -p $CFGDIR/sd${SCRN}
    if [ ! -f $CFGDIR/sd${SCRN}/shared.xml ] ; then
        cp -va $CFGDIR/shared.xml $CFGDIR/sd${SCRN}/
    fi
    mkdir -p $CFGDIR/sd${SCRN}/${AUDIO_SWITCHER_USERS[$idx]}/
    if [ ! -f $CFGDIR/sd${SCRN}/${AUDIO_SWITCHER_USERS[$idx]}/config.xml ] ; then
        cp -va $CFGDIR/config.xml $CFGDIR/sd${SCRN}/${AUDIO_SWITCHER_USERS[$idx]}/
    fi
    # echo drswinghead xxxxxxx | $SKYPE --pipelogin --dbpath=$CFGDIR/sd${SCRN} &
    echo ${AUDIO_SWITCHER_USERS[$idx]} ${AUDIO_SWITCHER_PASSWORDS[$idx]} | $SKYPE --pipelogin --dbpath=$CFGDIR/sd${SCRN} >$LOGDIR/skype_${SCRN}.log 2>&1 &
    SKYPE_PID=$!
    echo $SKYPE_PID > $PIDDIR/skype_${SCRN}.pid

    return 1;
}

function start_xvnc()
{
    local SCRN=$1
    local PID_FILE=$2
    local LOG_FILE=$LOGDIR/vnc_${SCRN}.log

    export DISPLAY=:$SCRN
    $XVNC -forever -listen $LSN_IP -autoport $VNCPORT -passwd skyvnc.${SCRN} -o $LOG_FILE &
    XVNC_PID=$!
    echo $XVNC_PID > $PIDDIR/xvnc_${SCRN}.pid

    return 1;
}

function start_skyserv()
{
    local SCRN=$1
    local PID_FILE=$2
    
    export DISPLAY=:$SCRN
    ####### 
    $SKYSERV > $LOGDIR/skyserv_${SCRN}.log 2>&1 &
    SKYSERV_PID=$!
    echo $SKYSERV_PID > $PIDDIR/skyserv_${SCRN}.pid

    return 1;
}

function stop_process()
{
    local PID_FILE=$1
    local PNAME=`echo $PID_FILE | awk -F/ '{print $(NF)}'| awk -F. '{print $1}'`
    
    if [ ! -f $PID_FILE ] ; then
        echo "$PNAME, pid file not found, can not terminate proccess";
        return 0;
    else
        local PROC_PID=`cat $PID_FILE`
        kill -9 $PROC_PID
        rm -f $PID_FILE
    fi

    return 1;
}

################# main
idx=1;
while true
do
    if [ $idx -gt $SCR_CNT ] ; then
        break;
    fi

    SCRN=`expr $SCRN_BASE + $idx`;
    VNCPORT=`expr $VNCPORT_BASE + $idx`;
    USER=${AUDIO_SWITCHER_USERS[$idx]};
    PASS=${AUDIO_SWITCHER_PASSWORDS[$idx]};

    XVFB_PID_FILE=$PIDDIR/xvfb_${SCRN}.pid
    OBOX_PID_FILE=$PIDDIR/obox_${SCRN}.pid
    SKYPE_PID_FILE=$PIDDIR/skype_${SCRN}.pid
    XVNC_PID_FILE=$PIDDIR/xvnc_${SCRN}.pid
    SKYSERV_PID_FILE=$PIDDIR/skyserv_${SCRN}.pid
    SKYSERV_LOG_FILE=$LOGDIR/skyserv_${SCRN}.log

    case "$ACTION" in
        start)
            ## start xvfb server
            $XVFB :$SCRN -pixdepths 32 -screen :$SCRN 800x600x8 &
            XVFB_PID=$!
            echo $XVFB_PID > $PIDDIR/xvfb_$SCRN.pid
            export DISPLAY=:$SCRN
            sleep 1 # wait for xvfb started

            #start openbox wm
            $OBOX &
            OBOX_PID=$!
            echo $OBOX_PID > $PIDDIR/obox_$SCRN.pid

            # start skype
            mkdir -p $CFGDIR/sd${SCRN}
            if [ ! -f $CFGDIR/sd${SCRN}/shared.xml ] ; then
                cp -v $CFGDIR/shared.xml $CFGDIR/sd${SCRN}/
            fi
            mkdir -p $CFGDIR/sd${SCRN}/${AUDIO_SWITCHER_USERS[$idx]}/
            if [ ! -f $CFGDIR/sd${SCRN}/${AUDIO_SWITCHER_USERS[$idx]}/config.xml ] ; then
                cp -v $CFGDIR/config.xml $CFGDIR/sd${SCRN}/${AUDIO_SWITCHER_USERS[$idx]}/
            fi
            # echo drswinghead xxxxxxx | $SKYPE --pipelogin --dbpath=$CFGDIR/sd${SCRN} &
            echo ${AUDIO_SWITCHER_USERS[$idx]} ${AUDIO_SWITCHER_PASSWORDS[$idx]} | $SKYPE --pipelogin --dbpath=$CFGDIR/sd${SCRN} &
            SKYPE_PID=$!
            echo $SKYPE_PID > $PIDDIR/skype_${SCRN}.pid

            $XVNC -forever -listen $LSN_IP -autoport $VNCPORT &
            XVNC_PID=$!
            echo $XVNC_PID > $PIDDIR/xvnc_${SCRN}.pid

            ####### 
            $SKYSERV > $LOGDIR/skyserv_${SCRN}.log &
            SKYSERV_PID=$!
            echo $SKYSERV_PID > $PIDDIR/skyserv_${SCRN}.pid

            echo "DISPLAY=:${SCRN} ./skyserv"
            echo "vncview ${LSN_IP}:${VNCPORT}"
            echo "killall -9 Xvfb"
             ;;
        stop)
            #stop_process $SKYPE_PID_FILE
            RET=$?

            stop_process $SKYSERV_PID_FILE
            RET=$?

#stop_process $XVNC_PID_FILE
            RET=$?

#stop_process $OBOX_PID_FILE
#           stop_process $XVFB_PID_FILE
            RET=$?
            ;;
        restart)
            ;;
        check)
            echo "Checking #$idx skyserv health ...";
            if [ ! -f $XVFB_PID_FILE ] ; then
                echo "XVFB($idx) :${SCRN} not exist, because pid file not exists";
                # break;
                if [ x"$AUTOFIX" = x"1" ] ; then
                    start_xvfb $SCRN $XVFB_PID_FILE
                    RET=$?
                fi
            else
                XVFB_PID=`cat $XVFB_PID_FILE`
                kill -s WINCH $XVFB_PID > /dev/null 2>&1
                XVFB_EXIST=$?
                if [ x"$XVFB_EXIST" != x"0" ]; then
                    echo "XVFB($idx) :${SCRN} not exist, because process disappearred: $XVFB_EXIST";
                    # break;
                    if [ x"$AUTOFIX" = x"1" ] ; then
                        echo "  XVFB($idx) :${SCRN} Tring restart ...";
                        start_xvfb $SCRN $XVFB_PID_FILE
                        RET=$?
                    fi
                else
                    echo "XVFB($idx) :$SCRN ..... OK";
                fi
            fi

            #######
            if [ ! -f $OBOX_PID_FILE ] ; then
                echo "OBOX($idx) :${SCRN} not exists, because pid file not exists";
                # break;
                if [ x"$AUTOFIX" = x"1" ] ; then
                    start_obox $SCRN $OBOX_PID_FILE
                    RET=$?
                fi
            else
                OBOX_PID=`cat $OBOX_PID_FILE`
                kill -s WINCH $OBOX_PID > /dev/null 2>&1
                OBOX_EXIST=$?
                if [ x"$OBOX_EXIST" != x"0" ] ; then
                    echo "OBOX($idx) :${SCRN} not exists, because process disappearred: $OBOX_EXIST";
                    # break;
                    if [ x"$AUTOFIX" = x"1" ] ; then
                        echo "  OBOX($idx) :${SCRN} Tring restart ...";
                        start_obox $SCRN $OBOX_PID_FILE
                        RET=$?
                    fi
                else
                    echo "OBOX($idx) :$SCRN ..... OK";
                fi
            fi

            ####### 
            if [ ! -f $SKYPE_PID_FILE ] ; then
                echo "SKYPE($idx) :${SCRN} not exists, because pid file not exists";
                # break;
                if [ x"$AUTOFIX" = x"1" ] ; then
                    start_skype $SCRN $SKYPE_PID_FILE
                    RET=$?
                fi
            else
                SKYPE_PID=`cat $SKYPE_PID_FILE`
                kill -s WINCH $SKYPE_PID > /dev/null 2>&1
                SKYPE_EXIST=$?
                if [ x"$SKYPE_EXIST" != x"0" ] ; then
                    echo "SKYPE($idx) :${SCRN} not exists, because process disappearred: $SKYPE_EXIST";
                    # break;
                    if [ x"$AUTOFIX" = x"1" ] ; then
                        echo "  SKYPE($idx) :${SCRN} Tring restart ...";
                        start_skype $SCRN $SKYPE_PID_FILE
                        RET=$?
                    fi
                else
                    echo "SKYPE($idx) :$SCRN ..... OK";
                fi
            fi

            ####### 
            if [ ! -f $XVNC_PID_FILE ] ; then
                echo "XVNC($idx) :${SCRN} not exists, because pid file not exists";
                # break;
                if [ x"$AUTOFIX" = x"1" ] ; then
                    start_xvnc $SCRN $XVNC_PID_FILE
                    RET=$?
                fi
            else
                XVNC_PID=`cat $XVNC_PID_FILE`
                kill -s WINCH $XVNC_PID > /dev/null 2>&1
                XVNC_EXIST=$?
                if [ x"$XVNC_EXIST" != x"0" ] ; then
                    echo "XVNC($idx) :${SCRN} not exists, because process disappearred: $XVNC_EXIST";
                    # break;
                    if [ x"$AUTOFIX" = x"1" ] ; then
                        echo "  XVNC($idx) :${SCRN} Tring restart ...";
                        start_xvnc $SCRN $XVNC_PID_FILE
                        RET=$?
                    fi
                else
                    echo "XVNC($idx) :$SCRN ..... OK";
                fi
            fi

            ######## skyserv
            if [ ! -f $SKYSERV_PID_FILE ] ; then
                echo "SKYSERV($idx) :${SCRN} not exists, because pid file not exists";
                # break; 如果被误删，那么只能重启了，这可能出现问题
                if [ x"$AUTOFIX" = x"1" ] ; then
                    start_skyserv $SCRN $SKYSERV_PID_FILE
                    RET=$?
                fi
            else
                SKYSERV_PID=`cat $SKYSERV_PID_FILE`
                kill -s WINCH $SKYSERV_PID > /dev/null 2>&1
                SKYSERV_EXIST=$?
                if [ x"$SKYSERV_EXIST" != x"0" ] ; then
                    echo "SKYSERV($idx) :${SCRN} not exists, because process disappearred: $SKYSERV_EXIST";
                    # break;
                    if [ x"$AUTOFIX" = x"1" ] ; then
                        echo "  SKYSERV($idx) :${SCRN} Tring restart ...";
                        start_skyserv $SCRN $SKYSERV_PID_FILE
                        RET=$?
                    fi
                else
                    # check log file size, assert(log_file_size!=0);
                    if [ -s $SKYSERV_LOG_FILE ] ; then
                        # check mtime
                        NOW_UTS=`date +%s`
                        ITS_MTIME=`ls -l --time-style=+%s $SKYSERV_LOG_FILE|awk '{print $6}'`
                        MOD_PAST_TIME=`expr $NOW_UTS - $ITS_MTIME`
                        MOD_PAST_MIN=`expr $MOD_PAST_TIME / 60`
                        if [ $MOD_PAST_TIME -gt 30 ] ; then
                            echo "SKYSERV($idx,$SKYSERV_PID) :${SCRN} locked?, log file mtime expired $MOD_PAST_TIME: $SKYSERV_EXIST";
                            if [ x"$AUTOFIX" = x"1" ] ; then
                                echo "  SKYSERV($idx) :${SCRN} Tring restart ...";
                                stop_process $SKYSERV_PID_FILE
                                start_skyserv $SCRN $SKYSERV_PID_FILE
                                RET=$?
                            fi
                        else
                            echo "SKYSERV($idx) :$SCRN ..... OK";
                        fi
                    else
                        echo "SKYSERV($idx) :${SCRN} problem, log file size empty: $SKYSERV_EXIST";
                        if [ x"$AUTOFIX" = x"1" ] ; then
                            echo "  SKYSERV($idx) :${SCRN} Tring restart ...";
                            stop_process $SKYSERV_PID_FILE
                            start_skyserv $SCRN $SKYSERV_PID_FILE
                            RET=$?
                        fi
                    fi
                fi
            fi
            ;;
        fix)
            ;;
        incr)
            ;;
        decr)
            ;;
        *)
            echo "Usage: here";
            exit 1;
    esac

    idx=`expr $idx + 1`;    
done

## delta ctrl
## 问题在于如何记住当前真正要管理的数量,还是需要使用一个临时文件。
if [ x"$ACTION" = x"incr" ] ; then
    true
    INCRN=$2
elif [ x"$ACTION" = x"decr" ] ; then
    true
    DECRN=$2
else
    true
fi

#### omit below test code
exit;
# one instance of gateway
SCRN=2000
## start xvfb server
$XVFB :$SCRN -pixdepths 32 -screen :$SCRN 800x600x24 &
XVFB_PID=$!
echo $XVFB_PID > $PIDDIR/xvfb_$SCRN.pid

sleep 2 # wait for xvfb started
#start openbox wm
export DISPLAY=:$SCRN
$OBOX &
OBOX_PID=$!
echo $OBOX_PID > $PIDDIR/obox_$SCRN.pid

# start skype
mkdir -p $CFGDIR/sd${SCRN}
echo drswinghead 2032103 | $SKYPE --pipelogin --dbpath=$CFGDIR/sd${SCRN} &
SKYPE_PID=$!
echo $SKYPE_PID > $PIDDIR/skype_$SCRN.pid

# start x11vnc
LSN_IP=`/sbin/ifconfig -a|grep "inet addr"|egrep "(192.168|172.24)"|awk '{print $2}'|awk -F: '{print $2}'|head -n 1`
if [ x"$LSN_IP" = x"" ] ; then
    LSN_IP=localhost
fi

VNCPORT=`expr $VNCPORT_BASE + $SCRN`
$XVNC -forever -listen $LSN_IP -autoport $VNCPORT &
XVNC_PID=$!
echo $XVNC_PID > $PIDDIR/xvnc_$SCRN.pid

echo "DISPLAY=:${SCRN} ./skyserv"

echo "vncview localhost:${SCRN}"

echo "killall -9 Xvfb"


