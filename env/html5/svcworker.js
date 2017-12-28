//VERSION 
+function (){
  var version = 'sVERSION';

  var filesToCache = FILESLIST;

function sw_install(event) {
	console.log('[ServiceWorker] Installing....');

	event.waitUntil(
    caches
      .open(version)
      .then(cacheOpen)
  ); 
};

function cacheOpen(cache) {
  console.log('[ServiceWorker] Caching files');
  cache.addAll(filesToCache);
}

function sw_fetch(event) {
  console.log("getting url:"+event.request.url);

  var request = event.request;
  var acceptHeader = request.headers.get('Accept');

  event.respondWith(
    fetchFromCache(event).
    catch(() => {
      if(acceptHeader.indexOf('text/html') !== -1) {
        console.log("Service worker serving html");
        return caches.match(self.location.origin+'/index.html').
          then(response => {
            if(!response) {
              throw Error(`${self.location.origin}/index.html not found in cache`);
            }

            return response;
          });
      }
      else {
        return offlineResponse();
      }
    })
  );
};

function fetchFromCache(event) {
  return caches.match(event.request.url).then(response => {
    if(!response) {
      throw Error(`${event.request.url} not found in cache`);
    }

    return response;
  });
}

function offlineResponse() {
  return new Response('', {
    headers: {
      'Content-Type': 'text/plain'
    }
  });
}

function sw_activate(event) {
  console.log('Service worker activating');

  event.waitUntil(caches.keys().then(cacheKeys));
};

function cacheKeys(keylist) {
  return Promise.all(keylist.map(oneKey));
}

function oneKey(key) {
  if(key != version) {
    console.log('Service worker removing old cache ', key);

    return caches.delete(key);
  }
}

self.addEventListener("install", sw_install);
self.addEventListener("fetch", sw_fetch);
self.addEventListener('activate', sw_activate);
}();
