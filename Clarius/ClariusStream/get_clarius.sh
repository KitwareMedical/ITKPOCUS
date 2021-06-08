
ZIPFILE='v8.0.1.zip'
UNZIPFILE='v8.0.1'
FINALFILE='mobileapi-8.0.1'

if test -f "$ZIPFILE"; then
    rm "$ZIPFILE"
fi

if test -f "$UNZIPFILE"; then
    rm -r "$UNZIPFILE"
fi

if test -f "$FINALFILE"; then
    rm -r "$FINALFILE"
fi

wget https://github.com/clariusdev/mobileapi/archive/refs/tags/v8.0.1.zip -O "$ZIPFILE"
unzip "$ZIPFILE" -d "$UNZIPFILE"
mv ./v8.0.1/mobileapi-8.0.1/ .
rm -r "$UNZIPFILE" "$ZIPFILE"