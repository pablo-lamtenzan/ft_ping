#! /bin/sh

DEST=srcs.mk

SRCDIR=${1:-srcs}
INCDIR=${2:-includes}

# $0="target dir", $1="depth"
function index_dir()
{
    local DEPTH=${2:-1}
    local OLD_DEPTH=${DEPTH}
    local DEPTH_CP=${DEPTH}
    while [ ${DEPTH_CP} -ge 2 ] ; do
        echo -en "\\t" >> ${DEST}
        let DEPTH_CP--
    done
    local BASENAME=`basename ${1}`
    echo -e "\$(addprefix ${BASENAME}/,\\" >> ${DEST}
    for i in ${1}/* ; do
        BASENAME=`basename ${i}`
        if [[ -d ${i} ]] ; then
            let DEPTH++
            index_dir ${i} ${DEPTH}
        elif [[ -f ${i} ]] ; then
            DEPTH_CP=${OLD_DEPTH}
            while [ $DEPTH_CP -ge 2 ] ; do
                echo -en "\\t" >> ${DEST}
                let DEPTH_CP--
            done
            echo -e "\\t${BASENAME}\\" >> ${DEST}
        fi
    done
    DEPTH_CP=${OLD_DEPTH}
    while [ $DEPTH_CP -ge 2 ] ; do
        echo -en "\\t" >> ${DEST}
        let DEPTH_CP--
    done
    echo -en ")" >> ${DEST}
    if [ $DEPTH -ge 2 ] ; then
        echo -en "\\" >> ${DEST}
    fi
    echo -e >> ${DEST}
}

if [ -f ${DEST} ] ; then
    rm ${DEST}
fi
    
echo -e "INCDIR\\t=\\t${INCDIR}\nSRCDIR\t=\\t${SRCDIR}\\n" >> ${DEST}

echo -e "HDRS\\t=\\" >> ${DEST}
index_dir ${INCDIR}

echo -e "SRCS\\t=\\" >> ${DEST}
index_dir ${SRCDIR}
