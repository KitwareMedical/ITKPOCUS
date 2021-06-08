$zippath = '.\v8.0.1.zip'
$unzippath = '.\v8.0.1'
$finalpath = '.\mobileapi-8.0.1'

if (Test-Path $zippath) {
	rm -r $zippath
}
if (Test-Path $unzippath) {
	rm -r $unzippath
}
if (Test-Path $finalpath) {
	rm -r $finalpath
}

wget https://github.com/clariusdev/mobileapi/archive/refs/tags/v8.0.1.zip -OutFile $zippath
Expand-Archive $zippath
mv .\v8.0.1\mobileapi-8.0.1\ .
rm -r $unzippath
rm -r $zippath

