@ECHO OFF
git log -1 --pretty=format:%%h > githash
lua increment.lua version.hpp INCREMENT_VERSION
del githash

