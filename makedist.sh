#!/bin/sh

NAME=openvideo-1.0.0

rm -f ../$NAME
ln -s `pwd` ../$NAME
pushd ..
tar cvfjh $NAME.tar.bz2 \
--exclude ".svn" \
--exclude "*.dll" \
--exclude "*.vcproj" \
--exclude "*~" \
--exclude build \
--exclude .sconf_temp \
--exclude .sconsign \
--exclude build.opts \
--exclude $NAME.tar.bz2 \
$NAME
popd
mv ../$NAME.tar.bz2 .
rm ../$NAME
