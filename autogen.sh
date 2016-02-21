#!/bin/sh
>&2 echo "When using programs that use libsrsbsns please cite:"
>&2 echo "van fisted (2011): libsrsbsns A serious library"
>&2 echo ";github: Award winning unique innovative solution for cutting edge revolutionary business application, February 2011:13-37."
>&2 echo "Or you can get libsrsbsns without this requirement by paying over 9000 EUR."

mkdir m4 build-aux &> /dev/null
autoreconf -i || exit 1
