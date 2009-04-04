This directory needs to contain the external libraries used by e.
They can be downloaded from the following sites:

  libcurl-7.18.2
  http://curl.haxx.se/download.html

  libtomcrypt-1.11
  http://libtom.org/?page=download&newsitems=5&whatfile=crypt
  Alternate URL: http://libtomcrypt.com/download.html

  libtommath-0.39
  http://libtom.org/?page=download&newsitems=5&whatfile=ltm
  
  metakit
  http://www.equi4.com/metakit/overview.html
  
  pcre-7.6
  http://pcre.org/
  
  tinyxml
  http://sourceforge.net/projects/tinyxml

They all need to be built as static libraries. If you have problems
with any of these libs, you should be able to upgrade to a later version.

A few libaries need to be patched to be used in e. The patches can be
found in the "patches" subdirectory.
