+function() {
  var version = 'sVERSION',
  base = (function(){
    var a = self.location.href; return a.substring(0, a.lastIndexOf("/"))
  })(),

  filesToCache = ['/index.html', '/worker.min.js', '/fonts/icomoon.woff'].map(element => base+element);

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
    var request = event.request;
    console.log("getting url:" + request.url);

    event.respondWith(
      fetchFromCache(request.url).
      catch(err => {
        if(request.headers.get('Accept').indexOf('text/html') !== -1) {
          console.log("Service worker serving html");
          return fetchFromCache(base+'/index.html');
        }

        throw err;
      }).
      catch(() => fetch(request)).
      then(response => addToCache(request, response))
    );
  };

function addToCache(request, response) {
  if(response.ok) {
    var copy = response.clone();

    caches.open(version).then(cache => {
      console.log(request.url);
      cache.put(request, copy);
    });
  }
  return response;
}

function blankResponse() {
  return new Response('', {
    headers: {
      'Content-Type': 'text/plain'
    }
  });
}


  function fetchFromCache(url) {
    return caches.match(url).then(response => {
      if(!response) {
        throw Error(`${url} not found in cache`);
      }

      return response;
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
}()
