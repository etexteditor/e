devenv curllib\curllib.sln /Build Debug
devenv libtommath\libtommath.sln /Build Debug
devenv libtomcrypt\libtomcrypt.sln /Build Debug
devenv metakit\win\msvc90\mksrc.sln /Build Debug
devenv pcre\pcre.sln /Build Debug
devenv tinyxml\tinyxml.sln /Build Debug

pushd wxwidgets\build\msw
nmake -f makefile.vc BUILD=debug UNICODE=1
popd
