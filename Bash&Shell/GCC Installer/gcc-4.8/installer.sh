#!/bin/sh
 
# ======================================= ���� ======================================= 
PREFIX_DIR=/usr/local/gcc-4.8.0
 
# ======================= �ǽ������ ======================= 
BUILD_TARGET_CONF_OPTION=""
BUILD_OTHER_CONF_OPTION=""
 
# ======================= �����������ʾ�� ======================= 
# BUILD_TARGET_CONF_OPTION="--target=arm-linux --enable-multilib --enable-interwork --disable-shared"
# BUILD_OTHER_CONF_OPTION="--disable-shared"
  
# ======================================= ��� ======================================= 
 
# ======================= ������ȴ�ʱ�� ======================= 
CHECK_INFO_SLEEP=3
 
# ======================= ��װĿ¼��ʼ�� ======================= 
if [ $# -gt 0 ]; then
    PREFIX_DIR=$1
fi
 
mkdir -p "$PREFIX_DIR"
PREFIX_DIR="$( cd "$PREFIX_DIR" && pwd )";
 
# ======================= ת���ű�Ŀ¼ ======================= 
WORKING_DIR="$PWD";
 
# ======================= �����64λϵͳ��û��װ32λ�Ŀ������������Ҫgcc���� --disable-multilib ����, ������32λ�� ======================= 
SYS_LONG_BIT=$(getconf LONG_BIT)
GCC_OPT_DISABLE_MULTILIB=""
if [ $SYS_LONG_BIT == "64" ]; then
    GCC_OPT_DISABLE_MULTILIB="--disable-multilib"
    GCC_OPT_DISABLE_MULTILIB_DEV_LIBS=($(whereis librt))
    if [ ${#GCC_OPT_DISABLE_MULTILIB_DEV_LIBS[@]} -lt 2 ]; then
        echo -e "\\033[31;1mglibc-devel is required.\\033[39;49;0m"
        exit -1
    fi
    for FILE_PATH in ${GCC_OPT_DISABLE_MULTILIB_DEV_LIBS[@]}; do
        if [ ${FILE_PATH:0:9} == "/usr/lib/" ]; then
            echo -e "\\033[32;1mnotice: librt x86_64 found multilib enabled.\\033[39;49;0m"
            GCC_OPT_DISABLE_MULTILIB=""
        fi
    done
    GCC_OPT_DISABLE_MULTILIB_DEV_LIBS=""
    if [ "$GCC_OPT_DISABLE_MULTILIB" == "--disable-multilib" ]; then
        echo -e "\\033[32;1mwarning: --disable-multilib is added when build gcc.\\033[39;49;0m"
        let CHECK_INFO_SLEEP=$CHECK_INFO_SLEEP+1
    fi
fi
 
# ======================= ���ǿ�ƿ��������� ======================= 
if [ ! -z "$GCC_OPT_DISABLE_MULTILIB" ]; then
    for opt in $BUILD_TARGET_CONF_OPTION ; do
        if [ "$opt" == "--enable-multilib" ]; then
            echo -e "\\033[32;1mwarning: librt not found in [*]/lib64 but --enable-multilib enabled in GCC_OPT_DISABLE_MULTILIB.\\033[39;49;0m"
            GCC_OPT_DISABLE_MULTILIB="";
            break;
        fi
        echo $f; 
    done
fi
 
# ======================= ���CPU�����������߳������ܴ���CPU������2����������ܳ����� ======================= 
BUILD_THREAD_OPT=8;
BUILD_CPU_NUMBER=$(cat /proc/cpuinfo | grep -c "^processor[[:space:]]*:[[:space:]]*[0-9]*");
let BUILD_THREAD_OPT=$BUILD_CPU_NUMBER*2;
if [ $BUILD_THREAD_OPT -gt 8 ]; then
    BUILD_THREAD_OPT=8;
fi
BUILD_THREAD_OPT="-j$BUILD_THREAD_OPT";
echo -e "\\033[32;1mnotice: $BUILD_CPU_NUMBER cpu(s) detected. use $BUILD_THREAD_OPT for multi-thread compile."
 
# ======================= ͳһ�İ��������غ��� ======================= 
function check_and_download(){
    PKG_NAME="$1";
    PKG_MATCH_EXPR="$2";
    PKG_URL="$3";
     
    PKG_VAR_VAL=$(ls -d $PKG_MATCH_EXPR);
    if [ ! -z "$PKG_VAR_VAL" ]; then
        echo "$PKG_VAR_VAL"
        return 0;
    fi
     
    if [ -z "$PKG_URL" ]; then
        echo -e "\\033[31;1m$PKG_NAME not found.\\033[39;49;0m" 
        exit -1;
    fi
     
    wget -c "$PKG_URL";
    PKG_VAR_VAL=$(ls -d $PKG_MATCH_EXPR);
     
    if [ -z "$PKG_VAR_VAL" ]; then
        echo -e "\\033[31;1m$PKG_NAME not found.\\033[39;49;0m" 
        exit -1;
    fi
     
    echo "$PKG_VAR_VAL";
}
 
# ======================================= ���� ======================================= 
echo -e "\\033[31;1mcheck complete.\\033[39;49;0m"
 
# ======================= ׼������, �ѿ�Ͷ�����Ŀ¼���룬���������Ҳ�������ļ� ======================= 
export LD_LIBRARY_PATH=$PREFIX_DIR/lib:$PREFIX_DIR/lib64:$LD_LIBRARY_PATH
export PATH=$PREFIX_DIR/bin:$PATH
 
echo -e "\\033[32;1mnotice: reset env LD_LIBRARY_PATH=$LD_LIBRARY_PATH\\033[39;49;0m";
echo -e "\\033[32;1mnotice: reset env PATH=$PATH\\033[39;49;0m";
 
echo "WORKING_DIR = $WORKING_DIR"
echo "PREFIX_DIR = $PREFIX_DIR"
echo "BUILD_TARGET_CONF_OPTION = $BUILD_TARGET_CONF_OPTION"
echo "BUILD_OTHER_CONF_OPTION = $BUILD_OTHER_CONF_OPTION"
echo "CHECK_INFO_SLEEP = $CHECK_INFO_SLEEP"
echo "BUILD_CPU_NUMBER = $BUILD_CPU_NUMBER"
echo "BUILD_THREAD_OPT = $BUILD_THREAD_OPT"
echo "GCC_OPT_DISABLE_MULTILIB = $GCC_OPT_DISABLE_MULTILIB"
echo "SYS_LONG_BIT = $SYS_LONG_BIT"
 
echo -e "\\033[32;1mnotice: now, sleep for $CHECK_INFO_SLEEP seconds.\\033[39;49;0m"; 
sleep $CHECK_INFO_SLEEP
  
# ======================= �رս��������������ˬ���� ======================= 
swapoff -a
 
# install gmp
GMP_PKG=$(check_and_download "gmp" "gmp-*.tar.bz2" "ftp://ftp.gmplib.org/pub/gmp/gmp-5.1.1.tar.bz2" );
if [ $? -ne 0 ]; then
    echo -e "$GMP_PKG"
    exit -1;
fi
tar -jxvf $GMP_PKG
GMP_DIR=$(ls -d gmp-* | grep -v \.tar\.bz2)
cd $GMP_DIR
CPPFLAGS=-fexceptions ./configure --prefix=$PREFIX_DIR --enable-cxx --enable-assert $BUILD_OTHER_CONF_OPTION
make $BUILD_THREAD_OPT && make check && make install
cd "$WORKING_DIR"
   
# install mpfr
MPFR_PKG=$(check_and_download "mpfr" "mpfr-*.tar.bz2" "http://www.mpfr.org/mpfr-current/mpfr-3.1.2.tar.bz2" );
if [ $? -ne 0 ]; then
    echo -e "$MPFR_PKG"
    exit -1;
fi
tar -jxvf $MPFR_PKG
MPFR_DIR=$(ls -d mpfr-* | grep -v \.tar\.bz2)
cd $MPFR_DIR
./configure --prefix=$PREFIX_DIR --with-gmp=$PREFIX_DIR --enable-assert $BUILD_OTHER_CONF_OPTION
make $BUILD_THREAD_OPT && make install
cd "$WORKING_DIR"
   
# install mpc
MPC_PKG=$(check_and_download "mpc" "mpc-*.tar.gz" "http://www.multiprecision.org/mpc/download/mpc-1.0.1.tar.gz" );
if [ $? -ne 0 ]; then
    echo -e "$MPC_PKG"
    exit -1;
fi
tar -zxvf $MPC_PKG
MPC_DIR=$(ls -d mpc-* | grep -v \.tar\.gz)
cd $MPC_DIR
./configure --prefix=$PREFIX_DIR --with-gmp=$PREFIX_DIR --with-mpfr=$PREFIX_DIR $BUILD_OTHER_CONF_OPTION
make $BUILD_THREAD_OPT && make install
cd "$WORKING_DIR"
  
# install isl
ISL_PKG=$(check_and_download "isl-0.11" "isl-*.tar.bz2" "ftp://gcc.gnu.org/pub/gcc/infrastructure/isl-0.11.1.tar.bz2" );
if [ $? -ne 0 ]; then
    echo -e "$ISL_PKG"
    exit -1;
fi
tar -jxvf $ISL_PKG
ISL_DIR=$(ls -d isl-* | grep -v \.tar\.bz2)
cd $ISL_DIR
./configure --prefix=$PREFIX_DIR --with-gmp-prefix=$PREFIX_DIR $BUILD_OTHER_CONF_OPTION
make $BUILD_THREAD_OPT && make install
cd "$WORKING_DIR"
  
# install cloog
CLOOG_PKG=$(check_and_download "cloog-0.18" "cloog-0.*.tar.gz" "ftp://gcc.gnu.org/pub/gcc/infrastructure/cloog-0.18.0.tar.gz" );
if [ $? -ne 0 ]; then
    echo -e "$CLOOG_PKG"
    exit -1;
fi
tar -zxvf $CLOOG_PKG
CLOOG_DIR=$(ls -d cloog-0.* | grep -v \.tar\.gz)
cd $CLOOG_DIR
./configure --prefix=$PREFIX_DIR --with-gmp=system --with-gmp-prefix=$PREFIX_DIR --with-bits=gmp --with-isl=$PREFIX_DIR $BUILD_OTHER_CONF_OPTION
make $BUILD_THREAD_OPT && make check && make install
cd "$WORKING_DIR"
   
# ======================= install gcc ======================= 
# ======================= ������ISL��ʱ���ʧЧ������ ======================= 
cd $ISL_DIR
make install
cd "$WORKING_DIR"
 
# ======================= gcc�� ======================= 
GCC_PKG=$(check_and_download "gcc" "gcc-*.tar.bz2" "ftp://gcc.gnu.org/pub/gcc/releases/gcc-4.8.0/gcc-4.8.0.tar.bz2" );
if [ $? -ne 0 ]; then
    echo -e "$GCC_PKG"
    exit -1;
fi
tar -jxvf $GCC_PKG
GCC_DIR=$(ls -d gcc-* | grep -v \.tar\.bz2)
mkdir objdir
cd objdir
# ======================= ��һ�е����һ��������ע�⣬���Ҫ֧����������Ҫ��װ�����Ⲣ�򿪶Ը����Ե�֧�� ======================= 
GCC_CONF_OPTION_ALL="--prefix=$PREFIX_DIR --with-gmp=$PREFIX_DIR --with-mpc=$PREFIX_DIR --with-mpfr=$PREFIX_DIR --with-isl=$PREFIX_DIR --with-cloog=$PREFIX_DIR --enable-bootstrap --enable-build-with-cxx --enable-cloog-backend=isl --disable-libjava-multilib --enable-checking=release $GCC_OPT_DISABLE_MULTILIB $BUILD_TARGET_CONF_OPTION";
../$GCC_DIR/configure $GCC_CONF_OPTION_ALL
make $BUILD_THREAD_OPT && make install
cd "$WORKING_DIR"
 
ls $PREFIX_DIR/bin/*gcc
if [ $? -ne 0 ]; then
    echo -e "\\033[31;1mError: build gcc failed.\\033[39;49;0m";
    exit -1;
fi
 
# ======================= ����cc������ ======================= 
ln -s $PREFIX_DIR/bin/gcc $PREFIX_DIR/bin/cc
 
CHECK_PPL_PATH="$(whereis libppl.so.* | awk '{print $2;}')";
# ##### [���������������PPL�⣬ �����°�GCC�Ѿ�ȥ����PPL�����������������������Ǳ�����������û�о�����] #####
if [ -z "$CHECK_PPL_PATH" ]; then
    echo -e "\\033[32;1mwarning: ppl not found, skip build [binutils] and [gdb].\\033[39;49;0m"
else
    # ======================= install binutils(������,����� ��) ======================= 
    BINUTILS_PKG=$(check_and_download "binutils" "binutils-*.tar.bz2" "http://ftp.gnu.org/gnu/binutils/binutils-2.23.2.tar.bz2" );
    if [ $? -ne 0 ]; then
        echo -e "$BINUTILS_PKG"
        exit -1;
    fi
    tar -jxvf $BINUTILS_PKG
    BINUTILS_DIR=$(ls -d binutils-* | grep -v \.tar\.bz2)
    cd $BINUTILS_DIR
    ./configure --prefix=$PREFIX_DIR --with-gmp=$PREFIX_DIR --with-mpc=$PREFIX_DIR --with-mpfr=$PREFIX_DIR --enable-build-with-cxx $BUILD_TARGET_CONF_OPTION
    make $BUILD_THREAD_OPT && make check && make install
    cd "$WORKING_DIR"
 
    # ======================= install gdb(������) [����PPL��, libtermcap��] ======================= 
    CHECK_TERMCAP_PATH="$(whereis libtermcap | awk '{print $2;}')";
    if [ -z "$CHECK_TERMCAP_PATH" ]; then
        echo -e "\\033[32;1mwarning: libtermcap not found, skip build [gdb].\\033[39;49;0m"
    else
        GDB_PKG=$(check_and_download "gdb" "gdb-*.tar.bz2" "http://ftp.gnu.org/gnu/gdb/gdb-7.5.1.tar.bz2" );
        if [ $? -ne 0 ]; then
            echo -e "$GDB_PKG"
            exit -1;
        fi
        tar -jxvf $GDB_PKG
        GDB_DIR=$(ls -d gdb-* | grep -v \.tar\.bz2)
        cd $GDB_DIR
        ./configure --prefix=$PREFIX_DIR --with-gmp=$PREFIX_DIR --with-mpc=$PREFIX_DIR --with-mpfr=$PREFIX_DIR --enable-build-with-cxx $BUILD_TARGET_CONF_OPTION
        make $BUILD_THREAD_OPT && make install
        cd "$WORKING_DIR"
    fi
fi
 
 
# Ӧ�þͱ�������
# 64λϵͳ�ڣ��������java֧�ֵĻ�������gmp�ϻ������⣬���������ѡ��ر�java֧�� --enable-languages=c,c++,objc,obj-c++,fortran,ada
# �ٰ�$PREFIX_DIR/bin�ŵ�PATH
# $PREFIX_DIR/lib �������64λ��������$PREFIX_DIR/lib64��[���⻹��$PREFIX_DIR/libexec��Ҳ��֪��Ҫ��Ҫ�ӣ������Ҽ���]�ŵ�LD_LIBRARY_PATH����/etc/ld.so.conf��
# ��ִ��ldconfig�Ϳ������µ�gcc��  
 
echo -e "\\033[33;1mAddition, run the cmds below to add environment var(s).\\033[39;49;0m"
echo -e "\\033[31;1mexport PATH=$PREFIX_DIR/bin:$PATH\\033[39;49;0m"
echo -e "\\033[31;1mexport LD_LIBRARY_PATH=$PREFIX_DIR/lib:$PREFIX_DIR/libexec:$LD_LIBRARY_PATH\\033[39;49;0m"
echo -e "\\033[31;1mexport LD_LIBRARY_PATH=$PREFIX_DIR/lib64:$LD_LIBRARY_PATH # run this if in a x86_64 arch.\\033[39;49;0m"
echo -e "\tor you can add $PREFIX_DIR/lib, $PREFIX_DIR/lib64 (if in x86_64) and $PREFIX_DIR/libexec to file [/etc/ld.so.conf] and then run [ldconfig]"
echo -e "\\033[33;1mBuild Gnu Compile Collection done.\\033[39;49;0m"
