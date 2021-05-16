#!/bin/sh

build_set_old()
{
  setnum=$1
  ch_max=$2
  for i in $(seq 1 $ch_max) ; do
    file="s${setnum}cp$i"
    cfile="$file.c"
    if [ ! -f $cfile ] ; then
      echo "$cfile doesnot exist"
      continue
    fi
    echo "Compiling $cfile to $file"
    clang -o $file -I . helper.c $cfile
    if [ $? -ne 0 ] ; then 
      echo "Error compiling $cfile"
      return
    fi
  done
}

build_set()
{
  setnum=$1
  ch_max=$2
  for i in $(seq 1 $ch_max) ; do
    build_one ${setnum} $i
  done
}

clean_set()
{
  setnum=$1
  ch_max=$2
  for i in $(seq 1 $ch_max) ; do
    file="s${setnum}cp$i"
    if [ ! -f $file ] ; then
      echo "$file doesnot exist"
      continue
    fi
    echo "Deleting $file"
    rm -rf $file
    if [ $? -ne 0 ] ; then 
      echo "Error deleting $file"
      return
    fi
  done
}

build_one()
{
  setnum=$1
  ch=$2
  file="s${setnum}cp${ch}"
  cfile="$file.c"
  if [ ! -f $cfile ] ; then
    echo "$cfile doesnot exist"
    return
   fi
  echo "Compiling $cfile to $file"
  clang -o $file -I . helper.c $cfile
    if [ $? -ne 0 ] ; then 
      echo "Error compiling $cfile"
      return
    fi
}

SETNUM=1
CH_END=8
build_set $SETNUM $CH_END
#clean_set $SETNUM $CH_END
#build_one $SETNUM 6