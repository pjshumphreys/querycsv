function ___syscall220(which, varargs) {
  SYSCALLS.varargs = varargs;

  try {
   // SYS_getdents64
    var stream = SYSCALLS.getStreamFromFD(), dirp = SYSCALLS.get(), count = SYSCALLS.get(), pos = 0;

    if (!stream.getdents) {
      stream.getdents = FS.readdir(stream.path);
    }

    while (stream.getdents.length > 0 && pos + 268 < count) {
      var id;
      var type;
      var name = stream.getdents.pop();
      var utf8Name = new Int8Array(256);

      assert(name.length < 256); // limit of dirent struct
      stringToUTF8Array(name, utf8Name, 0, 256);

      if (name[0] === '.') {
        id = 1;
        type = 4; // DT_DIR
      }
      else {
        var child = FS.lookupNode(stream.node, name);
        id = child.id;
        type = FS.isChrdev(child.mode) ? 2 :  // DT_CHR, character device.
               FS.isDir(child.mode) ? 4 :     // DT_DIR, directory.
               FS.isLink(child.mode) ? 10 :   // DT_LNK, symbolic link.
               8;                             // DT_REG, regular file.
      }

      HEAP32[((dirp + pos)>>2)]=id;
      HEAP32[(((dirp + pos)+(4))>>2)]=stream.position;
      HEAP16[(((dirp + pos)+(8))>>1)]=268;
      HEAP8[(((dirp + pos)+(10))>>0)]=type;

      for (var i = 0; i < utf8Name.length; i++) {
        HEAP8[(((dirp + pos)+(11 + i))>>0)]=utf8Name[i];
        if(utf8Name[i] ===0) break;
      }

      pos += 268;
    }

    return pos;
  }
  catch (e) {

    if (typeof FS === 'undefined' || !(e instanceof FS.ErrnoError)) {
      abort(e);
    }

    return -e.errno;
  }
}

FS.filesystems.IDBWFS = (function() {
  var IDBFS = {
    DB_VERSION: 21,
    DB_STORE_NAME: "FILE_DATA",
    dbs: {},
    workerIO: {},
    workerFiles: {},
    indexedDB: function() {
      if (typeof self.indexeddb !== 'undefined') {
        return self.indexeddb;
      }

      var ret = null;

      if (typeof window === 'object') {
        ret = window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
      }

      //assert(ret, 'IDBFS used, but indexedDB not supported');

      return ret;
    },
    mount: function(mount) {
      var mountIO = newMountIO(mount.mountpoint);
      var retvar;

      if(mount.opts.fromEmscripten instanceof Function) {
        mountIO.fromEmscripten = mount.opts.fromEmscripten;
      }

      IDBFS.workerIO[mount.mountpoint] = mountIO;
      mountIO.toEmscripten(true);

      // reuse all of the core MEMFS functionality
      retvar = MEMFS.mount.apply(null, arguments);

      if(mount.opts.toEmscriptenReciever instanceof Function) {
        mount.opts.toEmscriptenReciever(mountIO.toEmscripten);
      }

      return retvar;
    },
    syncfs: function(mount, populate, callback) {
      switch (populate) {
        case 3:     //memfs to worker
        case 4: {   //worker to memfs
          IDBFS.getLocalSet(mount, function(err, local) {
            if (err) {
              return callback(err);
            }

            var worker = IDBFS.workerFiles[mount.mountpoint];

            var src = populate===4 ? worker : local;
            var dst = populate===4 ? local : worker;

            IDBFS.reconcile(src, dst, callback);
          });
        } break;

        default:{ //idb to memfs or memfs to idb and worker
          IDBFS.getLocalSet(mount, function(err, local) {
            if (err) {
              return callback(err);
            }

            IDBFS.getRemoteSet(mount, function(err, remote) {
              if (err) {
                return callback(err);
              }

              var src = populate ? remote : local;
              var dst = populate ? local : remote;

              IDBFS.reconcile(src, dst, callback);
            });
          });
        } break;
      }
    },
    getDB: function(name, callback) {
      // check the cache first
      var db = IDBFS.dbs[name];

      if (db) {
        return callback(null, db);
      }

      var req;

      try {
        req = IDBFS.indexedDB().open(name, IDBFS.DB_VERSION);
      }
      catch (e) {
        return callback(e);
      }

      req.onupgradeneeded = function(e) {
        var db = e.target.result;
        var transaction = e.target.transaction;

        var fileStore;

        if (db.objectStoreNames.contains(IDBFS.DB_STORE_NAME)) {
          fileStore = transaction.objectStore(IDBFS.DB_STORE_NAME);
        }
        else {
          fileStore = db.createObjectStore(IDBFS.DB_STORE_NAME);
        }

        if (!fileStore.indexNames.contains('timestamp')) {
          fileStore.createIndex('timestamp', 'timestamp', {
            unique: false
          });
        }
      };
      req.onsuccess = function() {
        db = req.result;

        // add to the cache
        IDBFS.dbs[name] = db;
        callback(null, db);
      };
      req.onerror = function(e) {
        callback(this.error);
        e.preventDefault();
      };
    },
    getLocalSet: function(mount, callback) {
      var entries = {};

      function isRealDir(p) {
        return p !== '.' && p !== '..';
      };

      function toAbsolute(root) {
        return function(p) {
          return PATH.join2(root, p);
        }
      };

      var check = FS.readdir(mount.mountpoint).filter(isRealDir).map(toAbsolute(mount.mountpoint));

      while (check.length) {
        var path = check.pop();
        var stat;

        try {
          stat = FS.stat(path);
        }
        catch (e) {
          return callback(e);
        }

        if (FS.isDir(stat.mode)) {
          check.push.apply(check, FS.readdir(path).filter(isRealDir).map(toAbsolute(path)));
        }

        entries[path] = {
          timestamp: stat.mtime
        };
      }

      return callback(null, {
        type: 'local',
        entries: entries
      });
    },
    getRemoteSet: function(mount, callback) {
      var entries = {};

      IDBFS.getDB(mount.mountpoint, function(err, db) {
        if (err) {
          return callback(err);
        }

        var transaction = db.transaction([IDBFS.DB_STORE_NAME], 'readonly');

        transaction.onerror = function(e) {
          callback(this.error);
          e.preventDefault();
        };

        var store = transaction.objectStore(IDBFS.DB_STORE_NAME);
        var index = store.index('timestamp');

        index.openKeyCursor().onsuccess = function(event) {
          var cursor = event.target.result;

          if (!cursor) {
            return callback(null, {
              type: 'remote',
              db: db,
              entries: entries
            });
          }

          entries[cursor.primaryKey] = {
            timestamp: cursor.key
          };

          cursor.continue();
        };
      });
    },
    loadLocalEntry: function(path, callback) {
      var stat, node;

      try {
        var lookup = FS.lookupPath(path);
        node = lookup.node;
        stat = FS.stat(path);
      }
      catch (e) {
        return callback(e);
      }

      if (FS.isDir(stat.mode)) {
        return callback(null, {
          timestamp: stat.mtime,
          mode: stat.mode
        });
      }
      else if (FS.isFile(stat.mode)) {
        // Performance consideration: storing a normal JavaScript array to an IndexedDB is much slower than storing a typed array.
        // Therefore always convert the file contents to a typed array first before writing the data to IndexedDB.
        node.contents = MEMFS.getFileDataAsTypedArray(node);
        return callback(null, {
          timestamp: stat.mtime,
          mode: stat.mode,
          contents: node.contents
        });
      }
      else {
        return callback(new Error('node type not supported'));
      }
    },
    storeLocalEntry: function(path, entry, callback) {
      try {
        if (FS.isDir(entry.mode)) {
          FS.mkdir(path, entry.mode);
        }
        else if (FS.isFile(entry.mode)) {
          FS.writeFile(path, entry.contents, {
            encoding: 'binary',
            canOwn: true
          });
        }
        else {
          return callback(new Error('node type not supported'));
        }

        FS.chmod(path, entry.mode);
        FS.utime(path, entry.timestamp, entry.timestamp);
      }
      catch (e) {
        return callback(e);
      }

      callback(null);
    },
    removeLocalEntry: function(path, callback) {
      try {
        var lookup = FS.lookupPath(path);
        var stat = FS.stat(path);

        if (FS.isDir(stat.mode)) {
          FS.rmdir(path);
        }
        else if (FS.isFile(stat.mode)) {
          FS.unlink(path);
        }
      }
      catch (e) {
        return callback(e);
      }

      callback(null);
    },
    loadRemoteEntry: function(store, path, callback) {
      var req = store.get(path);

      req.onsuccess = function(event) {
        callback(null, event.target.result);
      };

      req.onerror = function(e) {
        callback(this.error);
        e.preventDefault();
      };
    },
    storeRemoteEntry: function(store, path, entry, callback) {
      var req = store.put(entry, path);

      req.onsuccess = function() {
        callback(null);
      };

      req.onerror = function(e) {
        callback(this.error);
        e.preventDefault();
      };
    },
    removeRemoteEntry: function(store, path, callback) {
      var req = store.delete(path);

      req.onsuccess = function() {
        callback(null);
      };

      req.onerror = function(e) {
        callback(this.error);
        e.preventDefault();
      };
    },
    loadWorkerEntry: function(store, path, callback) {
      if(store.entries.hasOwnProperty(path)) {
        callback(null, store.entries[path]);
      }
      else {
        callback(true);
      }
    },
    storeWorkerEntry: function(store, path, entry, callback) {
      store.entries[path] = {timestamp: entry.timestamp};
      callback(null);
    },
    removeWorkerEntry: function(store, path, callback) {
      delete store.entries[path];
      callback(null);
    },
    reconcile: function(src, dst, callback, worker) {
      var store;
      var total = 0;

      var create = [],create2 = [];
      Object.keys(src.entries).forEach(function(key) {
        var e = src.entries[key];
        var e2 = dst.entries[key];

        if (!e2 || e.timestamp > e2.timestamp) {
          create.push(key);
          total++;
        }
      });

      var remove = [];
      Object.keys(dst.entries).forEach(function(key) {
        var e = dst.entries[key];
        var e2 = src.entries[key];
        if (!e2) {
          remove.push(key);
          total++;
        }
      });

      if (!total) {
        if(dst.type === 'worker' && IDBFS.workerIO[dst.mountpoint].fromEmscripten instanceof Function) {  IDBFS.workerIO[dst.mountpoint].fromEmscripten([], []);
        }

        return callback(null);
      }

      var errored = false;
      var completed = 0;

      if(src.type === 'remote' || dst.type === 'remote') {
        var db = src.type === 'remote' ? src.db : dst.db;
        var transaction = db.transaction([IDBFS.DB_STORE_NAME], 'readwrite');
        store = transaction.objectStore(IDBFS.DB_STORE_NAME);

        transaction.onerror = function(e) {
          done(this.error);
          e.preventDefault();
        };
      }
      else {
        store = src.type === 'worker' ? src : dst;
      }

      function done(err) {
        if (err) {
          if (!done.errored) {
            done.errored = true;
            return callback(err);
          }
          return;
        }
        if (++completed >= total) {
          if(dst.type === 'worker' && IDBFS.workerIO[dst.mountpoint].fromEmscripten instanceof Function) {
            IDBFS.workerIO[dst.mountpoint].fromEmscripten(
              create2,
              remove);

            create2.forEach(function(entry){
              delete entry.path
            });
          }

          return callback(null);
        }
      }

      // sort paths in ascending order so directory entries are created
      // before the files inside them
      create.sort().forEach(function(path) {
        if (dst.type === 'local') {
          if(src.type == 'worker'){
            IDBFS.loadWorkerEntry(store, path, function(err, entry) {
              if (err) {
                return done(err);
              }

              IDBFS.storeLocalEntry(path, entry, done);
            });
          }
          else {
            IDBFS.loadRemoteEntry(store, path, function(err, entry) {
              if (err) {
                return done(err);
              }

              IDBFS.storeLocalEntry(path, entry, done);
            });
          }
        }
        else if (dst.type === 'worker') {
          IDBFS.loadLocalEntry(path, function(err, entry) {
            if (err) {
              return done(err);
            }

            IDBFS.storeWorkerEntry(dst, path, entry, done);

            entry.path = path;

            create2.push(entry);
          });
        }
        else {
          IDBFS.loadLocalEntry(path, function(err, entry) {
            if (err) {
              return done(err);
            }

            IDBFS.storeRemoteEntry(store, path, entry, done);
          });
        }
      });

      // sort paths in descending order so files are deleted before their
      // parent directories
      remove.sort().reverse().forEach(function(path) {
        if (dst.type === 'local') {
          IDBFS.removeLocalEntry(path, done);
        }
        else if (dst.type === 'worker') {
          IDBFS.removeWorkerEntry(dst, path, done);
        }
        else {
          IDBFS.removeRemoteEntry(store, path, done);
        }
      });
    }
  };

  function isArray(a) {
    return Object.prototype.toString.call(a) === '[object Array]';
  }

  function sortEntries(a,b) {
    if ( a.path < b.path )
      return -1;
    if ( a.path > b.path )
      return 1;
    return 0;
  }

  function newMountIO(mountpoint) {
    var mountIO = {
      toEmscripten: function(reset, create, remove) {
        var a, i, len;

        if(reset === true) {  //clear out the cache of files used to figure out what to send to the worker
          IDBFS.workerFiles[mountpoint] = {
            mountpoint: mountpoint,
            type: 'worker',
            entries: {}
          };
        }

        if(isArray(create) && isArray(remove)) {
          create.forEach(function(entry) {
            if(
                !IDBFS.workerFiles[mountpoint].entries[entry.path] ||
                IDBFS.workerFiles[mountpoint].entries[entry.path].timestamp <= entry.timestamp
              ) {
              IDBFS.workerFiles[mountpoint].entries[entry.path] = entry;
              delete entry.path;
            }
          });

          // sort paths in descending order so files are deleted before their
          // parent directories
          remove.forEach(function(path) {
            if(IDBFS.workerFiles[mountpoint].entries[path]) {
              delete IDBFS.workerFiles[mountpoint].entries[path];
            }
          });
        }
      }
    };

    return mountIO;
  }

  return IDBFS;
})();

Module["FS"] = FS;

return Module;

})();

var stringifyAndPostFactory = function(object, JSON) {
  return function(messageType, data) {
    object.postMessage(JSON.stringify({
      messageType:  messageType,
      data:         data
    }));
  };
};

var MSGS = "TEST_FOR_IDB IDB_STATUS MAKE_FAKE_IDB UPDATE_FAKE_IDB FAKE_IDB_UPDATED RUN_COMMAND OUTPUT_TEXT COMMAND_FINISHED".
  split(" ").
  reduce(function(a,b,c){a[b]=c;return a},{});
