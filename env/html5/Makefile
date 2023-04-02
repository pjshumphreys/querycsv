# Fetch the latest registry of available tools.
#~/emsdk/emsdk update

# Download and install the latest SDK tools.
#~/emsdk/emsdk install 1.39.10-fastcomp

# Make the "latest" SDK "active"
#~/emsdk/emsdk activate 1.39.10-fastcomp

# Add the compiler env vars
#source ~/emsdk/emsdk_env.sh

querycsv.tar.gz: build/appcache.html.gz build/svcworker.min.js build/index.html build/index.html.gz build/worker.min.js build/worker.min.js.gz build/apple-touch-icon-114x114-precomposed.png build/apple-touch-icon-120x120-precomposed.png build/apple-touch-icon-144x144-precomposed.png build/apple-touch-icon-152x152-precomposed.png build/apple-touch-icon-180x180-precomposed.png build/apple-touch-icon-72x72-precomposed.png build/apple-touch-icon-76x76-precomposed.png build/apple-touch-icon-precomposed.png build/touch-icon-192x192.png build/fonts/icomoon.woff build/fonts/icomoon.ttf build/fonts/icomoon.eot build/fonts/icomoon.svg
	cd build;find * -type f -print0|sort -z| tar --null --owner=0 --group=0 -ncvf ../querycsv.tar.gz -T -

build/fonts/icomoon.woff: fonts/icomoon.woff
	mkdir -p build/fonts
	cp fonts/icomoon.woff build/fonts/icomoon.woff

build/fonts/icomoon.ttf: fonts/icomoon.ttf
	mkdir -p build/fonts
	cp fonts/icomoon.ttf build/fonts/icomoon.ttf

build/fonts/icomoon.eot: fonts/icomoon.eot
	mkdir -p build/fonts
	cp fonts/icomoon.eot build/fonts/icomoon.eot

build/fonts/icomoon.svg: fonts/icomoon.svg
	mkdir -p build/fonts
	cp fonts/icomoon.svg build/fonts/icomoon.svg

build/index.html.gz: build/index.html
	mkdir -p build
	zopfli --gzip -c build/index.html > build/index.html.gz

build/appcache.html.gz: appcache.html
	mkdir -p build
	cp appcache.html build/appcache.html
	zopfli --gzip -c build/appcache.html > build/appcache.html.gz

build/index.html: index-debug.html svcworker.js output.min.js style.min.css build/fonts/icomoon.woff build/fonts/icomoon.ttf build/fonts/icomoon.eot build/fonts/icomoon.svg build/worker.min.js build/apple-touch-icon-114x114-precomposed.png build/apple-touch-icon-120x120-precomposed.png build/apple-touch-icon-144x144-precomposed.png build/apple-touch-icon-152x152-precomposed.png build/apple-touch-icon-180x180-precomposed.png build/apple-touch-icon-72x72-precomposed.png build/apple-touch-icon-76x76-precomposed.png build/apple-touch-icon-precomposed.png  build/touch-icon-192x192.png
	mkdir -p build
	node build-html.js

build/worker.min.js.gz: build/worker.min.js
	mkdir -p build
	zopfli --gzip -c build/worker.min.js > build/worker.min.js.gz

style.min.css: style.css animations.css
	cat style.css animations.css | java -jar yuicompressor-2.4.8.jar --type css > style.min.css

build/worker.min.js: worker.js
	mkdir -p build
	java -jar compiler.jar worker.js > build/worker.min.js

output.min.js: extern/jquery-3.5.0.js extern/b64.js extern/jquery.history.js extern/iscroll-5.2.0-2017-01-03.js extern/picup-2.1.2.js extern/jquery.animate.enhanced-1.11.js extern/_.debounce.js main.js
	cat extern/jquery-3.5.0.js extern/b64.js extern/jquery.history.js extern/iscroll-5.2.0-2017-01-03.js extern/picup-2.1.2.js extern/jquery.animate.enhanced-1.11.js extern/_.debounce.js main.js | java -jar compiler.jar > output.min.js

worker.js: emcc.js workermain.js
	rm -f worker.js
	cat emcc.js workermain.js >> worker.js

emcc.js: prefix.js lexer.c sql.c hash2.c hash3.c hash4a.c hash4b.c hash4c.c querycsv.c emcc.c postfix.js
	source ~/emsdk/emsdk_env.sh; emcc -O2 --closure 0 -s WASM=0 --minify 0 -s ASM_JS=2 --memory-init-file 0 -s "NO_EXIT_RUNTIME=1" -s "ASSERTIONS=1" -s RESERVED_FUNCTION_POINTERS=2 -s "EXPORTED_FUNCTIONS=['_folderExists','_rmrf','_addFile','_getEntries','_wrapmain','_malloc','UTF8ToString']" -s "EXTRA_EXPORTED_RUNTIME_METHODS=['addFunction','ccall','UTF8ToString','print','printErr']" lexer.c sql.c hash2.c hash3.c hash4a.c hash4b.c hash4c.c querycsv.c emcc.c --pre-js prefix.js --post-js postfix.js -o emcc.js

build/apple-touch-icon-114x114-precomposed.png: icon.svg
	mkdir -p build
	inkscape --export-filename=build/apple-touch-icon-114x114-precomposed.png -w 114 -h 114 icon.svg

build/apple-touch-icon-120x120-precomposed.png: icon.svg
	mkdir -p build
	inkscape --export-filename=build/apple-touch-icon-120x120-precomposed.png -w 120 -h 120 icon.svg

build/apple-touch-icon-144x144-precomposed.png: icon.svg
	mkdir -p build
	inkscape --export-filename=build/apple-touch-icon-144x144-precomposed.png -w 144 -h 144 icon.svg

build/apple-touch-icon-152x152-precomposed.png: icon.svg
	mkdir -p build
	inkscape --export-filename=build/apple-touch-icon-152x152-precomposed.png -w 152 -h 152 icon.svg

build/apple-touch-icon-180x180-precomposed.png: icon.svg
	mkdir -p build
	inkscape --export-filename=build/apple-touch-icon-180x180-precomposed.png -w 180 -h 180 icon.svg

build/apple-touch-icon-72x72-precomposed.png: icon.svg
	mkdir -p build
	inkscape --export-filename=build/apple-touch-icon-72x72-precomposed.png -w 72 -h 72 icon.svg

build/apple-touch-icon-76x76-precomposed.png: icon.svg
	mkdir -p build
	inkscape --export-filename=build/apple-touch-icon-76x76-precomposed.png -w 76 -h 76 icon.svg

build/apple-touch-icon-precomposed.png: icon.svg
	mkdir -p build
	inkscape --export-filename=build/apple-touch-icon-precomposed.png -w 57 -h 57 icon.svg

build/touch-icon-192x192.png: icon.svg
	mkdir -p build
	inkscape --export-filename=build/touch-icon-192x192.png -w 192 -h 192 icon.svg

build/svcworker.min.js: build/index.html
	mkdir -p build
	java -jar compiler.jar svcworker.temp.js > build/svcworker.min.js
	zopfli --gzip -c build/svcworker.min.js > build/svcworker.min.js.gz

.PHONY: all
.PHONY: clean

clean:
	rm -rf build/ style.min.css output.min.js worker.js emcc.js querycsv.tar.gz
