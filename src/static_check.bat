@echo off

set Wildcard=*.h *.cpp *.ini *.c

echo ------------
echo ------------
echo ------------

echo STATICS FOUND:
findstr -s -n -i -l "static" %Wildcard%

echo ------------
echo ------------

echo GLOBALS FOUND: 
findstr -s -n -i -l "local_persist" %Wildcard%
echo --
findstr -s -n -i -l "global_variable" %Wildcard%

echo ------------
echo ------------
echo ------------