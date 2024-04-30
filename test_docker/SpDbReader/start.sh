#!/bin/sh

JAVA_HOME=/usr

if [ "$1" = "start" ] && [ "$2" != "" ]
then
    PROCESS_CHECKER=`ps -ef | grep "$2" | grep -v 'grep' | grep -v 'tail' | grep -v 'vi' | awk '{print $2}'`
    if [ "${PROCESS_CHECKER}" = "" ]
    then
        echo "(INFO) MyJava start!"
    else
        echo - `ps -ef | grep "$2"`
        echo "(INFO) $2 is already."
        echo ""
        exit
    fi
fi

if [ "$JAVA_HOME" = "" ]
then
  echo "input JAVA_HOME path : "
  read JAVA_HOME
fi

JAVA_EXE=$JAVA_HOME/bin/java

CURRENT_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

BOOTSTRAP_MAIN_CLASS=com.main.Bootstrap
BOOTSTRAP_CLASSPATH=$CURRENT_PATH/MyJava.jar:$CURRENT_PATH/lib/*

"$JAVA_EXE" -cp "$BOOTSTRAP_CLASSPATH" -Dfile.encoding=utf8 -Dpath.to.java="$JAVA_HOME" $"$BOOTSTRAP_MAIN_CLASS" "$@"