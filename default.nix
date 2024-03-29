{stdenv, curl, gcc, urweb, icu}:
  stdenv.mkDerivation rec {
    version = "0.0.1";
    name = "urweb-curl-${version}";

    src = ./.;

    buildInputs = [gcc];
    # TODO: Improve like urweb.nix c, cflags etc
    configurePhase = "
      sed -e s~@LIBCURL@~${curl.out}/lib/libcurl.so~g -e s~@LIBCURLDEV@~${curl.dev}/include~g lib.urp.in > lib.urp
    ";
    buildPhase = ''
      ${gcc}/bin/gcc -c -I${urweb}/include/urweb -I${curl.dev}/include -I${icu.dev}/include -Isrc/c -o src/c/curl.o -Wimplicit -Wall -Werror -Wno-deprecated-declarations src/c/curl.c
  '';
    installPhase = "
    mkdir $out
    cp -r . $out
  ";
  }
