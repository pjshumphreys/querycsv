+function () {
  'use strict';

  var
  currentMessageId = 0,
  usingFakeIDB,
  stringifyAndPost,
  toEmscripten,
  commandQueue = [],
  xxx;

  function doNothing() {

  }

  function fromEmscripten(create, remove) {
    stringifyAndPost(MSGS.COMMAND_FINISHED, {
      create: create,
      remove: remove
    });
  }

  function toEmscriptenReciever(a) {
    toEmscripten = a;
  }

  function messageHandler(e) { //set up the onmessage global function handler
    e = JSON.parse(e.data);

    switch(e.messageType) {
      case MSGS.TEST_FOR_IDB: {   //report whether IndexedDB is available in a web worker
        stringifyAndPost(MSGS.IDB_STATUS, usingFakeIDB);
      } break;

      case MSGS.UPDATE_FAKE_IDB: {   //update the fake indexedDB in the web worker with changes that have happened on the main thread
        toEmscripten(false, e.data.create, e.data.remove);
        Module.FS.syncfs(4, localPersisted);   //worker to memfs
      } break;

      case MSGS.RUN_COMMAND: {   //run the specified command line, may post back changed file contents if IndexedDB doesn't work in a web worker
        runCommand(e.data);
      }
    }
  }

  function localPersisted() {
    stringifyAndPost(MSGS.FAKE_IDB_UPDATED);
  }

  function runCommand(commandLine) {
    var path = commandLine.substring(0, commandLine.lastIndexOf("/"));

    stdout(path+"/>querycsv "+JSON.stringify(commandLine));

    commandQueue.push([path, commandLine]);

    if(!usingFakeIDB) {
      Module.FS.syncfs(true, runCommand2);  //indexeddb to memfs
    }
    else {
      runCommand2();
    }
  }

  function runCommand2() {
    //do whatever emscripten wants us to do to run the program.
    var e = Module.ccall(
            'wrapmain',
            'number',
            ['string','string'],
            commandQueue.shift()
          );

    if(usingFakeIDB) {
      //return the response code and contents of changed files
      Module.FS.syncfs(3, doNothing);   //memfs to worker
    }
    else {
      //if indexDB is functional in a web worker, then resync it then send the response code.
      Module.FS.syncfs(false, resynced); //local to indexeddb
    }
  }

  function resynced () {
    stringifyAndPost(MSGS.COMMAND_FINISHED);
  }

  //pseudo code thats called whenever stdout or strerr are flushed. the main thread will probably append this text to a pre tag or something like it
  function stdout(text) {
    stringifyAndPost(MSGS.OUTPUT_TEXT, {
      text:text,
      isStderr :false
    });
  }

  function stderr(text) {
    stringifyAndPost(MSGS.OUTPUT_TEXT, {
      text:text,
      isStderr :true
    });
  }

  function ready() {
    usingFakeIDB = !(self.indexeddb = self.indexedDB || self.mozIndexedDB || self.webkitIndexedDB || self.msIndexedDB),

    stringifyAndPost = stringifyAndPostFactory(self, JSON);
    self.onmessage = messageHandler;

    Module.print = stdout;
    Module.printErr = stderr;

    Module.FS.mkdir('/Documents');
    Module.FS.mount(Module.FS.filesystems.IDBWFS, {
      fromEmscripten:       fromEmscripten,
      toEmscriptenReciever: toEmscriptenReciever
    }, '/Documents');

    // sync from persisted state into memory and then
    // refresh the folder view
    Module.FS.syncfs(usingFakeIDB?4:true, doNothing);
  }

  ready();
}();
