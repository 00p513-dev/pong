#! /bin/sh
make clean all pong.3ds pong.cia

mkdir artifacts -p
rm artifacts/* -rf

mv pong.* artifacts
