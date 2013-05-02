#!/bin/sh

# ��������
WORKING_DIR=$PWD

# ��ȡ��ǰ�ű�Ŀ¼
function get_script_dir()
{
	echo "$( cd "$( dirname "$0" )" && pwd )";
}

# ���ñ�������
function set_local_lang()
{
	TARGET_LANG="zh_CN.UTF-8";
	if [ $# -gt 0 ]; then
		TARGET_LANG="$1";
	fi
	
	export LANG="$TARGET_LANG"
	export LC_CTYPE="$TARGET_LANG"
	export LC_NUMERIC="$TARGET_LANG"
	export LC_TIME="$TARGET_LANG"
	export LC_COLLATE="$TARGET_LANG"
	export LC_MONETARY="$TARGET_LANG"
	export LC_MESSAGES="$TARGET_LANG"
	export LC_PAPER="$TARGET_LANG"
	export LC_NAME="$TARGET_LANG"
	export LC_ADDRESS="$TARGET_LANG"
	export LC_TELEPHONE="$TARGET_LANG"
	export LC_MEASUREMENT="$TARGET_LANG"
	export LC_IDENTIFICATION="$TARGET_LANG"
	export LC_ALL="$TARGET_LANG"
	export RC_LANG="$TARGET_LANG"
	export RC_LC_CTYPE="$TARGET_LANG"
	export AUTO_DETECT_UTF8="yes"
}

# ����ָ���������ļ�
function remove_more_than()
{
    filter="$1";
    number_left=$2
    
    FILE_LIST=( $(ls -dt $filter) );
    for (( i=$number_left; i<${#FILE_LIST[@]}; i++)); do
    	rm -rf "${FILE_LIST[$i]}";
    done
}

# Զ��ָ��
function auto_scp()
{
    src=$1
    dst=$2
    pass=$3

    expect -c "set timeout -1;
            spawn scp -p -o StrictHostKeyChecking=no -r $src $dst;
            expect "*assword:*" { send $pass\r\n; };
            expect eof {exit;};
            "
}

function auto_ssh_exec()
{
    host_ip=$1
    host_port=$2
    host_user=$3
    host_pwd=$4
    cmd=$5
    expect -c "set timeout -1;
            spawn ssh -o StrictHostKeyChecking=no ${host_user}@${host_ip} -p ${host_port} \"${cmd}\";
            expect "*assword:*" { send $host_pwd\r\n; };
            expect eof {exit;};
            "
}

# ���Linux����
function free_useless_memory()
{
	CURRENT_USER_NAME=$(whoami)
	if [ "$CURRENT_USER_NAME" != "root" ]; then
		echo "Must run as root";
		exit -1;
	fi

	sync
	echo 3 > /proc/sys/vm/drop_caches
}

# ���δ�����õ��û������ڴ�
function remove_user_empty_ipc()
{
	CURRENT_USER_NAME=$(whoami)
	for i in $(ipcs | grep $CURRENT_USER_NAME | awk '{ if( $6 == 0 ) print $2}'); do
		ipcrm -m $i
		ipcrm -s $i
	done
}

# ����û������ڴ�
function remove_user_ipc()
{
	CURRENT_USER_NAME=$(whoami)
	for i in $(ipcs | grep $CURRENT_USER_NAME | awk '{print $2}'); do
		ipcrm -m $i
		ipcrm -s $i
	done
}


