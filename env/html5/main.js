+function (d) {
  'use strict';

  var
    base,
    oldclass,
    newclass,
    stateQueue = [],
    inactive = !0,
    wrapper,
    currentState,
    currentClass,
    currentFolderUl,
    currentPath = "/Documents/",
    currentName,
    editorScroll,
    editorWrapper,
    editorScroller,
    editorTextarea,
    editorSpan,
    saveDialog,
    cancelButton,
    targetElem,
    doTap = false,
    selectMode = false,
    fileModified = false,
    myScroll,
    myScroll3,
    myScroll2,
    myScroll5,
    addFileText,
    addFolderText,
    renameText,
    displayPathName,
    worker,
    usingFakeIDB,
    toEmscripten,
    commandsToRun = [],
    stringifyAndPost,
    consoleWrapper,
    consoleScroller,
    consolePre,
    addFolderNumber = 1,
    breadCrumbUl,
    oldSelectedCount = 0,
    newElems = [],
    db,
    settings = {
      id:"settings",
      sortBy:1, //1=name, 2 = size, 3 = date
      isDescending:0, //0 = no, 1 = yes
      usePicup:0
    },

    updateConsole = debounce(function() {
      consolePre.append(newElems);

      newElems = [];

      consoleRefresh(true);
    }, 30),

    addListEntry = Module.addFunction(function(name, type, modified, size) {
      var a, b;
      name = Module.Pointer_stringify(name);

      if(type === 1) {
        $('<li><button type="button" class="folder" data-filepath="'+currentPath+name+'/">'+
          '<span class="icon">&#xe911;</span>'+
          '<span class="filename">'+name+'</span><br>'+
          '<span class="hidden">.</span><span class="size">Folder</span>'+
          '</button></li>').appendTo(currentFolderUl);
      }
      else {
        a = new Date(modified*1000);
        b = a.getHours();
        $('<li><button type="button" data-filepath="'+currentPath+name+'">'+
          '<span class="icon">&#xe910;</span>'+
          '<span class="filename">'+name+'</span><br>'+
          '<span class="hidden">.</span><span class="date">'+
          a.getFullYear()+
          "-"+
          ("0"+(a.getMonth()+1)).slice(-2)+
          "-"+
          ("0"+a.getDate()).slice(-2)+
          '<span class="hidden">.</span> '+
          (b%12||12)+
          ":"+
          ("0"+a.getMinutes()).slice(-2)+
          (b<12?"am":"pm")+
          '<span class="hidden">.</span></span> '+
          '<span class="size">'+formatFileSize(size)+'</span>'+
          '</button></li>').appendTo(currentFolderUl);
      }
    }),
    xxx;

  function startTap(e) {
    e.preventDefault();

    $('.pushed').removeClass('pushed');
    targetElem = $(e.target).closest('button');

    doTap = true;

    if(selectMode == false) {
      $('.longTapping').removeClass('longTapping');
      targetElem.addClass('longTapping');
    }
    else {
      targetElem.addClass('pushed');
    }
  }

  function stopTap() {
    targetElem && targetElem.removeClass('longTapping').removeClass('pushed');
    doTap = false;
  }

  function longTap(e) {
    var a = targetElem;
    if(doTap == true && selectMode == false) {
      doTap = false;
      selectMode = true;

      History.pushState({
            type: "selection",
            path: currentPath,
            depth: currentPath.split("/").length-1
          },
          d.title,
          base+"/Select"
        );

      toggleButtonSelection(e);
      navigator.vibrate([58]);
    }
  }

  function updateSelection() {
    var b = $('.selected').length;

    if(b === 0) {
      selectMode = false;
      if(currentState === 'selection') {
        History.back();
      }
      $('#alt').fadeOut(100);
    }
    else {
      $('#selectionCount').text(b);
      if(b > 1) {
        $('#renameButton').fadeOut(100);
      }
      else {
        if(oldSelectedCount !== 0) {
          $('#renameButton').fadeIn(100);
        }
        else {
          $('#renameButton').show();
        }
      }

      if(oldSelectedCount === 0) {
        $('#alt').fadeIn(100);
      }
    }

    oldSelectedCount = b;
  }



  function toggleButtonSelection(e) {
    var a = $(e.target).closest('button').toggleClass('selected');

    e.stopPropagation();
    updateSelection();

    $('.pushed').removeClass('pushed');
    a.removeClass('longTapping');
  }

  function gotoSettings() {
    if(inactive) {
      History.pushState({
          type: "settings"
        },
        "Settings",
        base+"Settings"
      );
    }
  }

  function kbGotoActions(e) {
    if((e.keyCode || e.which) == 13) { //Enter keycode
      doTap = true;
      gotoActions(e);
    }
  }

  function bcGotoActions(e) {
    doTap = true;
    selectMode = false;
    gotoActions(e);
  }

  function gotoActions(e) {
    if(doTap == true) {
      doTap = false;

      if(selectMode == true) {
        toggleButtonSelection(e);
      }
      else if(inactive) {
        var btn = $(e.target).closest('button');
        var path = btn.attr('data-filepath');

        if(btn.hasClass('folder')) {
          History.pushState({
              type: "folder",
              path: path,
              depth: path.split("/").length-1
            },
            d.title,
            path.slice(0, -1).replace(/\//, base)
          );
        }
        else {
          History.pushState({
              type: "open",
              path: btn.attr('data-filepath'),
              name: btn.find('.filename').text()
            },
            "Actions",
            base+"Actions"
          );
        }

        btn.removeClass("longTapping");
      }
    }
  }

  function gotoEditor() {
    if(inactive) {
      History.pushState({
          type: "editor"
        },
        "Editor",
        base+"Editor"
      );
    }
  }

  function gotoConsole() {
    if(inactive) {
      History.pushState({
          type: "console"
        },
        "Console",
        base+"Console"
      );
    }
  }

  function goBack() {
    if(inactive) {
      History.back();
    }
  }

  function goForward() {
    if(inactive) {
      History.forward();
    }
  }

  function stateChange() {
    var state = {
        newState: History.getState(),
        currentState: currentState
    };

    if(stateQueue.length === 0) {
      stateQueue.push(state);

      currentState = state.newState.data.type;

      serviceStateQueue();
    }
    else {
      stateQueue.push(state);

      currentState = state.newState.data.type;
    }
  }

  function serviceStateQueue() {
    if(inactive) {
      inactive = false;

      var a = stateQueue.shift();

      if($(".pt-page-open").hasClass('changed') && (a.newState.data.type != 'open' && a.newState.data.type != 'editor')) {
        $(".pt-page-open").removeClass('changed');
        saveDialog.fadeOut(200);
        cancelButton.fadeOut(200);
      }

      switch(a.newState.data.type) {
        case "folder": {
          if(a.currentState === 'selection') {
            $('.selected').removeClass('selected');
            updateSelection();
            currentPath = a.newState.data.path;
            inactive = true;
          }
          else if(a.currentState === 'folder') {
            $('.selected').removeClass('selected');
            updateSelection();

            if(!Module.ccall(
                'folderExists',
                'number',
                ['string'],
                [a.newState.data.path]
            )) {
              do {
                a.newState.data.path = a.newState.data.path.replace(/[^\/]+\/$/, "");
              } while(!Module.ccall(
                  'folderExists',
                  'number',
                  ['string'],
                  [a.newState.data.path]
              ));

              a.newState.data.depth = a.newState.data.path.split("/").length-1;
            }

            if(currentPath != a.newState.data.path) {
              addFolderNumber = addFolderNumber == 1?2:1;

              if(a.newState.data.depth > currentPath.split("/").length-1) {
                currentPath = a.newState.data.path;
                populateFolder();
                updateBreadCrumb();
                myScroll2.refresh();
                myScroll2.scrollTo(myScroll2.maxScrollX, 0, 400);
                folderFromRight(true);
              }
              else {
                currentPath = a.newState.data.path;
                populateFolder();
                updateBreadCrumb();
                myScroll2.refresh();
                myScroll2.scrollTo(myScroll2.maxScrollX, 0, 400);
                folderFromLeft(false);
              }
            }
            else {
              inactive = true;
            }
          }
          else {
            if(!Module.ccall(
                'folderExists',
                'number',
                ['string'],
                [a.newState.data.path]
            )) {
              do {
                a.newState.data.path = a.newState.data.path.replace(/[^\/]+\/$/, "");
              } while(!Module.ccall(
                  'folderExists',
                  'number',
                  ['string'],
                  [a.newState.data.path]
              ));

              a.newState.data.depth = a.newState.data.path.split("/").length-1;
            }

            currentPath = a.newState.data.path;
            populateFolder();
            pageFromLeft('folder', false);
          }
        } break;

        case "selection": {
          if(a.currentState !== 'folder') {
            pageFromLeft('folder', false);
          }
          else {
            inactive = !0;
          }
        } break;

        case "settings": {
          pageFromRight('settings', true);
        } break;

        case "open": {
          currentPath = a.newState.data.path;
          currentName = a.newState.data.name;
          displayPathName.text(currentPath);

          if(a.currentState === "editor" || a.currentState === "console") {
            if($(".pt-page-open").hasClass('changed')) {
              navigator.vibrate([58]);
            }
            else {
              updateDownloadLink();
            }
            pageFromLeft('open', false);
          }
          else {
            updateDownloadLink();
            pageFromRight('open', true);
          }
        } break;

        case "editor": {
          editorTextarea.val(Module.FS.readFile(currentPath, {encoding:'utf8'}));
          pageFromRight('editor', true);
        } break;

        case "console": {
          $('#console .pre').text("");
          pageFromRight('console', true);
        } break;
      }
    }
  }

  function consoleRefresh(scrollToBottom) {
    var
        x = myScroll5.x,
        y = myScroll5.y,
        doX = consoleWrapper[0].scrollWidth == consoleWrapper.width(),
        doY = consoleWrapper[0].scrollHeight == consoleWrapper.height(),
        moo = 0, maa = 0, doo = false, obj = {};

    consolePre.scrollLeft(0);

    consoleScroller.removeAttr('style');
    moo = consolePre.width()+20;
    maa = consolePre.height()+20;

    if(moo>consoleScroller.width()) {
      obj['min-width'] = moo;
      doo = true;
    }

    if(maa>consoleScroller.height()) {
      obj['min-height'] = maa;
      doo = true;
    }

    if(doo) {
      consoleScroller.css(obj);
      doo = false;
    }

    if(doX) {
      x = consoleWrapper.width()-moo;
      doo = true;
    }
    if(doY) {
      y = consoleWrapper.height()-maa;
      doo = true;
    }


    if(!scrollToBottom && doo) {
      myScroll5.scrollTo(x, y, 0);
    }

    myScroll5.refresh();

    if(scrollToBottom) {
      myScroll5.scrollTo(0, myScroll5.maxScrollY, 0);
    }
  }

  function updateDownloadLink() {
    $('#downloadButton').
        attr(
          'href',
          "data:application/octet-stream;base64,"+
          base64js.
              fromByteArray(
                Module.FS.readFile(
                  currentPath,
                  {
                    encoding: 'binary'
                  }
                )
              )
        ).
        attr('download', currentName);
  }

  function pageFromLeft(cssClass, onTop) {
    oldclass = "pt-page-moveToRight";
    newclass = "pt-page-moveFromLeft";
    currentClass = "pt-page-current";
    $('.pt-page-current').addClass('pt-page-old pt-page-moveToRight' + ((!!onTop)? ' pt-page-ontop':''));
    $('.pt-page-'+cssClass).addClass('pt-page-new pt-page-current pt-page-moveFromLeft' + ((!onTop)? ' pt-page-ontop':''));
  }

  function pageFromRight(cssClass, onTop) {
    oldclass = "pt-page-moveToLeft";
    newclass = "pt-page-moveFromRight";
    currentClass = "pt-page-current";
    $('.pt-page-current').addClass('pt-page-old pt-page-moveToLeft' + ((!!onTop)? ' pt-page-ontop':''));
    $('.pt-page-'+cssClass).addClass('pt-page-new pt-page-current pt-page-moveFromRight' + ((!onTop)? ' pt-page-ontop':''));
  }

  function folderFromLeft(onTop) {
    oldclass = "pt-page-moveToRight";
    newclass = "pt-page-moveFromLeft";
    currentClass = "currentFolder";
    $('.currentFolder').addClass('pt-page-old pt-page-moveToRight' + ((!!onTop)? ' pt-page-ontop':''));
    $('.fileList').not('.currentFolder').addClass('pt-page-new currentFolder pt-page-moveFromLeft' + ((!onTop)? ' pt-page-ontop':''));
  }

  function folderFromRight(onTop) {
    oldclass = "pt-page-moveToLeft";
    newclass = "pt-page-moveFromRight";
    currentClass = "currentFolder";
    $('.currentFolder').addClass('pt-page-old pt-page-moveToLeft' + ((!!onTop)? ' pt-page-ontop':''));
    $('.fileList').not('.currentFolder').addClass('pt-page-new currentFolder pt-page-moveFromRight' + ((!onTop)? ' pt-page-ontop':''));
  }

  function animEnd() {
    var a = $('.pt-page-old');

    if(a.length === 1) {
      inactive = true;

      a
        .removeClass('pt-page-old')
        .removeClass(currentClass)
        .removeClass(oldclass)
        .removeClass('pt-page-ontop');

      $('.pt-page-new')
        .removeClass('pt-page-new')
        .removeClass(newclass)
        .removeClass('pt-page-ontop');

      switch(currentState) {
        case "folder":
          refreshIScroll();
        break;

        case "settings":
        break;

        case "open":
        break;

        case "editor":
          setTimeout(editorRefresh,1);
        break;

        case "console":
          consoleRefresh();
          runCommand(currentPath);
        break;
      }
    }

    if(inactive && stateQueue.length) {
      setTimeout(serviceStateQueue, 1);
    }
  }

  function editorInit() {
    editorScroll = new IScroll('#editor', {
        bounce : false,
        deceleration : 0.0001,
        mouseWheel : true,
        scrollX: true,
        freeScroll: true,
        tap:true
      });

    editorWrapper = $('#editor');
    editorScroller = editorWrapper.find('.scroller');
    editorTextarea = editorScroller.find('textarea');
    editorSpan = editorScroller.find('span');

    editorSpan.text(editorTextarea.val());
    editorTextarea.width(editorSpan[0].scrollWidth+20).height(editorSpan[0].scrollHeight+18).scrollTop(0);
    editorTextarea.scrollLeft(0);

    editorScroller.css({
        'min-height': editorTextarea.height()+20,
        'min-width': editorTextarea.width()+20
      });

    editorScroll.scrollTo(0, 0, 0);

    editorScroll.refresh();

    editorScroller.on('tap', editorFocus);

    editorTextarea.on({
        input: editorInput,
        keyup: editorKeyUp
      });

    editorWrapper.on('scroll', editorWrapperScroll);
  }

  function editorInput() {
    editorRefresh();

    $(".pt-page-open").addClass('changed');
    saveDialog.show();
    cancelButton.show();
  }

  function editorRefresh() {
    var
        x = editorScroll.x,
        y = editorScroll.y,
        doX = editorWrapper[0].scrollWidth == editorWrapper.width(),
        doY = editorWrapper[0].scrollHeight == editorWrapper.height();

    editorSpan.text(editorTextarea.val());
    editorTextarea.width(editorSpan[0].scrollWidth+20).height(editorSpan[0].scrollHeight+18).scrollTop(0);
    editorTextarea.scrollLeft(0);

    editorScroller.css({
        'min-height': editorTextarea.height()+20,
        'min-width': editorTextarea.width()+20
      });

    if(doX) {
      x = editorWrapper.width()-(editorTextarea.width()+20);
    }
    if(doY) {
      y = editorWrapper.height()-(editorTextarea.height()+20);
    }

    editorScroll.scrollTo(x, y, 0);
    editorScroll.refresh();
  }

  function editorWrapperScroll() {
    var scrollTop = editorWrapper.scrollTop();
    var scrollLeft = editorWrapper.scrollLeft();

    if(scrollTop != 0) {
      editorWrapper.scrollTop(0);
      editorScroll.scrollBy(0, -scrollTop, 0);
    }

    if(scrollLeft != 0) {
      editorWrapper.scrollLeft(0);
      editorScroll.scrollBy(-scrollLeft, 0, 0);
    }
  }

  function editorFocus() {
    editorTextarea.focus();
  }

  function editorKeyUp(e) {
    if ((e.keyCode ? e.keyCode : e.which) == 13) {
      editorScroll.scrollTo(0, editorScroll.y, 0);
    }
  }

  function doNothing() {
    //do nothing
  }

  function saveClick() {
    Module.FS.writeFile(currentPath, editorTextarea.val(), {encoding:'utf8'});
    Module.FS.syncfs(false, doNothing);
    saveDialog.hide();
    cancelButton.hide();
    $(".pt-page-open").removeClass('changed');
  }

  function closeSaveDialog(e) {
    if($(e.target).closest('button').is('#yes')) {
      Module.FS.writeFile(currentPath, editorTextarea.val(), {encoding:'utf8'});
      Module.FS.syncfs(false, closeSaveDialog2);
    }
    else {
      closeSaveDialog2();
    }
  }

  function closeSaveDialog2() {
    updateDownloadLink();
    $(".pt-page-open").removeClass('changed');
    saveDialog.fadeOut(200);
    cancelButton.fadeOut(200);
  }

  function toggleCheckbox() {
    var a = $(this).nextAll('span').first();

    a.html(a.attr(this.checked?'data-on':'data-off'));
  }

  function addFile() {
    var e, fileName = prompt(addFileText, '');

    if(fileName !== null) {
      if(fileName !== "" && fileName.indexOf('/') === -1) {
        try {
          e = Module.ccall(
            'addFile',
            'number',
            ['string'],
            [currentPath+fileName]
          );

          if (e) {
            throw new Module.FS.ErrnoError(e);
          }
        }
        catch(e) {
          alert(e.message);
        }
        Module.FS.syncfs(false, refreshFolder);
      }
      else {
        alert("failed");
      }
    }
  }

  function addFolder() {
    var e,folderName = prompt(addFolderText, '');

    if(folderName !== null) {
      if(folderName != "" && folderName.indexOf('/') === -1) {
        try {
          Module.FS.mkdir(currentPath+folderName);
        }
        catch(e) {
          alert(e.message);
        }
        Module.FS.syncfs(false, refreshFolder);
      }
      else {
        alert("failed");
      }
    }
  }

  function uploadFile(e) {
    var fileReader = new FileReader();
    fileReader.origFileName = this.files[0].name;
    fileReader.readAsArrayBuffer(this.files[0]);
    fileReader.onloadend = fileReaderOnLoadEnd;
  }

  function fileReaderOnLoadEnd(fileLoadedEvent) {
    Module.FS.writeFile(
        currentPath+this.origFileName,
        new Int8Array(fileLoadedEvent.target.result),
        {
          encoding:'binary'
        }
      );

    wrapper.html('<span><input type="file" title="Upload file"/></span>');

    Module.FS.syncfs(false, refreshFolder);
  }

  function selectAllClick() {
    $('.currentFolder button').addClass('selected');
    updateSelection();
  }

  function deleteEntry() {
    var a = $(this);

    if(a.hasClass('folder')) {
      Module.ccall(
        'rmrf',
        'number',
        ['string'],
        [currentPath+a.find('.filename').text()]
      );
    }
    else {
      Module.FS.unlink(currentPath+a.find('.filename').text());
    }
  }

  function deleteEntries() {
    var a = $('.selected').each(deleteEntry);

    $('.selected').removeClass('selected');
    updateSelection();

    Module.FS.syncfs(false, refreshFolder); //webfs to indexeddb
  }

  function renameEntry() {
    var
      a = $('.selected:first'),
      e,
      sucess = true,
      oldName = a.find('.filename').text(),
      newName = prompt(renameText.replace("{0}", oldName), '');

    if(newName !== null) {
      if(newName !== "" && newName.indexOf('/') === -1) {

        if(a.hasClass('folder')) {
          try {
            Module.FS.mkdir(currentPath+newName);
          }
          catch(e) {
            alert(e.message);
            sucess = false;
          }
        }
        else {
          try {
            e = Module.ccall(
              'addFile',
              'number',
              ['string'],
              [currentPath+newName]
            );

            if (e) {
              throw new Module.FS.ErrnoError(e);
            }
          }
          catch(e) {
            alert(e.message);
            sucess = false;
          }
        }

        if(sucess) {
          try {
            Module.FS.rename(currentPath+oldName, currentPath+newName);
          }
          catch(e) {
            alert(e.message);
          }
        }
      }
      else {
        alert("failed");
      }
    }

    $('.selected').removeClass('selected');
    updateSelection();

    Module.FS.syncfs(false, refreshFolder);
  }

  function populateFolder() {
    currentFolderUl = $('#folder'+addFolderNumber+' .scroller ul');
    currentFolderUl.empty();
    Module.ccall(
      'getEntries',
      'number',
      [
        'string',
        'number',
        'number',
        'number'
      ],
      [
        currentPath,
        settings.sortBy,
        settings.isDescending,
        addListEntry
      ]
    );
  }

  function updateBreadCrumb() {
    var
      i, len,
      removeRest = false,
      folderNames = currentPath.replace(/\/(\/)*/g, "/").replace(/(^\/|\/$)/g, "").split("/"),
      list = breadCrumbUl.find('li'),
      item,
      path = "/";

    list.each(function(index) {
      item = $(this);

      if(removeRest || (folderNames[0] != item.find('button').text() && (removeRest = true))) {   //yes the single equals is intentional
        item.remove();
      }
      else {
        path+=folderNames.shift()+"/";

        if(folderNames.length == 0) {
          item.addClass('active');
        }
        else {
          item.removeAttr('class');
        }
      }
    });

    if(len = folderNames.length) {
      for(i = 0, len-=1; i <= len; i++) {
        path+=folderNames[i]+"/";
        item = $('<li'+(i==len?' class="active"':'')+'><button type="button" class="folder" data-filepath="'+path+'"></button></li>');
        item.find('button').text(folderNames[i]);
        breadCrumbUl.append(item);
      }
    }

    var width = 0;

    breadCrumbUl.find('li').each(function() {
      width += $(this).outerWidth(true);
    });

    $('#scroller2').css("width","auto").width(width);
  }

  function refreshFolder() {
    populateFolder();
    refreshIScroll();
  }

  function refreshIScroll() {
    myScroll.refresh();
    myScroll2.refresh();
    myScroll2.scrollTo(myScroll2.maxScrollX, 0, 400);
    myScroll3.refresh();
  }

  function round(value, exp) {
    if (typeof exp === 'undefined' || +exp === 0)
      return Math.round(value);

    value = +value;
    exp = +exp;

    if (isNaN(value) || !(typeof exp === 'number' && exp % 1 === 0))
      return NaN;

    // Shift
    value = value.toString().split('e');
    value = Math.round(+(value[0] + 'e' + (value[1] ? (+value[1] + exp) : exp)));

    // Shift back
    value = value.toString().split('e');
    return +(value[0] + 'e' + (value[1] ? (+value[1] - exp) : -exp));
  }

  function formatFileSize(bytes) {
    var size = ['bytes','kB','MB','GB','TB','PB','EB','ZB','YB'];
    var factor = Math.floor(((""+bytes).length - 1) / 3);

    return ""+round(bytes / Math.pow(1000, factor), 1) + " " + size[factor];
  }

  function messageHandler(e) {
    e = JSON.parse(e.data);

    switch(e.messageType) {
      case MSGS.IDB_STATUS: { //returns whether IndexedDB is available in the worker
        workerIDBStatusRecieved(e.data);
      } break;

      case MSGS.FAKE_IDB_UPDATED: { //the copy of the indexedDB in the worker is now synchronized
        workerIDBUpdated();
      } break;

      case MSGS.OUTPUT_TEXT: {   //output to console
        output_text(e.data.text, e.data.isStderr);
      } break;

      case MSGS.COMMAND_FINISHED: {   //the command we asked the worker to run has completed. store any file contents returned  into IndexedDB
        commandFinished(e.data);
      } break;
    }
  }

  function output_text(text, isStderr) {
    newElems.push($('<span />').text(text).css("font-weight", isStderr?"bold":"normal"), $('<br/>'));

    updateConsole();
  }

  function fromEmscripten(create, remove) {
    stringifyAndPost(MSGS.UPDATE_FAKE_IDB, {
      create: create,
      remove: remove
    });
  }

  function toEmscriptenReciever(a) {
    toEmscripten = a;
  }

  function newWorker() {
    worker = new Worker(WORKERFILENAME);

    stringifyAndPost = stringifyAndPostFactory(worker, JSON);
    worker.addEventListener('message', messageHandler, false);
  }

  function runCommand(commandLine) {
    commandsToRun.push(commandLine);

    if(!!usingFakeIDB !== usingFakeIDB) {   //only do this on page load. The worker might be terminated and replaced later though
      stringifyAndPost(MSGS.TEST_FOR_IDB, null);
      return;
    }

    continueRunCommand();
  }

  function workerIDBStatusRecieved(IDBAvailable) {
    //serialise the indexedDB data then send it to the worker in a message
    usingFakeIDB = IDBAvailable;

    continueRunCommand();
  }

  function continueRunCommand() {
    if (usingFakeIDB) {
      Module.FS.syncfs(3, doNothing); //memfs to worker
    }
    else {
      Module.FS.syncfs(false, workerIDBUpdated); //memfs to indexedDB
    }
  }

  function workerIDBUpdated() {
    stringifyAndPost(MSGS.RUN_COMMAND, commandsToRun.shift());
  }

  function commandFinished(data) {
    if(usingFakeIDB) {
      toEmscripten(false, data.created, data.removed);
      Module.FS.syncfs(4, persistLocal); //worker to memfs
    }
    else {
      Module.FS.syncfs(true, populateFolder); //indexeddb to memfs
    }
  }

  function persistLocal() {
    Module.FS.syncfs(false, populateFolder); //memfs to indexeddb
  }

  function terminateWorker() {
    worker.terminate();

    newWorker();

    if(usingFakeIDB) {
      toEmscripten(true); //reset the cache of web worker files
    }
  }

  function loadSettings() {
    var openRequest = indexedDB.open("querycsv",1);
    openRequest.onupgradeneeded = dbUpgradeNeeded;
    openRequest.onsuccess = dbSuccess;
  }

  function dbUpgradeNeeded(e) {
    var thisDB = e.target.result;

    if(!thisDB.objectStoreNames.contains("settings")) {
      thisDB.createObjectStore("settings", {keyPath:"id"});
    }
  }

  function dbSuccess(e) {
    db = e.target.result;

    db.transaction(["settings"]).objectStore("settings").get("settings").onsuccess = getSettings;
  }

  function changeSortBy() {
    settings.sortBy = parseInt($(this).val(), 10);
    if(db) {
      db.transaction(["settings"],"readwrite").objectStore("settings").put(settings);
    }
  }

  function changePicup() {
    settings.usePicup = $(this).prop("checked")?1:0;
    if(db) {
      db.transaction(["settings"],"readwrite").objectStore("settings").put(settings);
    }
  }

  function changeAscending() {
    settings.isDescending = $(this).prop("checked")?1:0;
    if(db) {
      db.transaction(["settings"],"readwrite").objectStore("settings").put(settings);
    }
  }

  function getSettings(event) {
    if(event.target.result) {
      settings = event.target.result;
    }
    else {
      db.transaction(["settings"],"readwrite").objectStore("settings").add(settings);
    }

    $('#sortBy').on('change', changeSortBy).val(settings.sortBy);
    $('#ascending').on('change', changeAscending).prop('checked', settings.isDescending).each(toggleCheckbox);
    $('#usePicup').on('change', changePicup).prop('checked', settings.usePicup).each(toggleCheckbox);

    //if the url is "/", or doesn't begin with "/Documents" doesn't refer to a folder or doesn't exist then set it to "/Documents"
    var url = decodeURIComponent(History.getState().hash.replace(/\?.*/, "")).replace(/\/(\/)*/g, "/").replace(/\/$/, "");

    var text = "/Documents"+url.substring((base+'Documents').length);

    if(url.indexOf(base+'Documents') === 0 && Module.ccall(
        'folderExists',
        'number',
        ['string'],
        [text]
    )) {
      currentPath = text+"/";
    }
    else {
      url = base+"Documents"
    }

    updateBreadCrumb();
    refreshFolder();

    History.replaceState({
        type: "folder",
        path: currentPath,
        depth: currentPath.split("/").length-1
      },
      d.title,
      url
    );
  }

  function sw_success(registration) {
    // Registration was successful
    console.log('ServiceWorker registration successful with scope: ', registration.scope);

    if(registration.installing) {
      console.log('Service worker installing');
    }
    else if(registration.waiting) {
      console.log('Service worker waiting');
    }
    else if(registration.active) {
      console.log('Service worker active');
    }
  }

  function sw_controllerChange(event) {
    navigator.
      serviceWorker.
      controller.
      addEventListener('statechange', sw_stateChange);
  }

  function sw_stateChange() {
    if(this.state === 'activated') {
      window.location.reload(!0);
    }
  }

  function ready() {
    if(!(window.Worker && window.File && window.FileReader && window.FileList && window.Blob)) {
      return;
    }

    $(d.body).html($('#content').html());
    $('#content').remove();

    base = $('base').attr('href');

    if('serviceWorker' in navigator) {
      navigator.
        serviceWorker.
        register(SVCWORKERFILENAME).
        then(sw_success);

      // Listen for claiming our ServiceWorker
      navigator.
        serviceWorker.
        addEventListener('controllerchange', sw_controllerChange);
    }

    // Browsers not using Service Workers
    else if('applicationCache' in window) {
      $(d.body).append('<iframe style="display:none" src="'+APPCACHEFILENAME+'"></iframe>');
    }

    currentState = "folder";

    $(window).on('statechange', stateChange);

    //Set up the IScroll objects
    myScroll = new IScroll('#folder1', {
      bounce : false,
      deceleration : 0.0001,
      mouseWheel : true,
      tap: true
    }),

    myScroll3 = new IScroll('#folder2', {
      bounce : false,
      deceleration : 0.0001,
      mouseWheel : true,
      tap: true
    }),

    myScroll2 = new IScroll('#wrapper2', {
        bounce : false,
        deceleration : 0.0001,
        scrollX : true,
        scrollY : false,
        tap:true
      }),

    myScroll5 = new IScroll('#console', {
        bounce : false,
        deceleration : 0.0001,
        mouseWheel : true,
        scrollX: true,
        freeScroll: true,
        tap:true
      });

    $(window).resize(debounce(function() {
      switch(currentState) {
        case "console": {
          consoleRefresh();
        } break;

        case "folder": {
          myScroll2.scrollTo(myScroll2.maxScrollX, 0, 400);
        } break;
      }
    }, 420));

    consoleWrapper = $('#console');
    consoleScroller = consoleWrapper.find('.scroller');
    consolePre = consoleScroller.find('.pre');
    //start of event handlers

    $('.pt-perspective').on('animationend', animEnd);

    breadCrumbUl = $('#wrapper2 #scroller2 ul');
    breadCrumbUl.
      on('tap', 'button', bcGotoActions).
      on('keyup', 'button', kbGotoActions);

    myScroll.on('scrollCancel', stopTap);
    myScroll.on('scrollStart', stopTap);
    myScroll3.on('scrollCancel', stopTap);
    myScroll3.on('scrollStart', stopTap);

    $('#folder1, #folder2').
      on('mousedown pointerdown touchstart', 'button', startTap).
      on('animationend', 'button', longTap).
      on('tap', 'button', gotoActions).
      on('keyup', 'button', kbGotoActions);

    addFileText = $("#addFile").on('click', addFile).attr('data-text');
    addFolderText = $("#addFolder").on('click', addFolder).attr('data-text');
    renameText = $("#renameButton").on('click', renameEntry).attr('data-text');
    $("#delete").on('click', deleteEntries);
    $('#selectAllButton').on('click', selectAllClick);

    wrapper = $("#uploadFile");
    wrapper.on("change", "input", uploadFile);

    $('.swith').on('click', gotoSettings);
    $("input[type='checkbox']").on('change', toggleCheckbox);

    $('.backButton').on('click', goBack);

    saveDialog = $('.saveDialog');
    cancelButton = $('#cancelButton');
    cancelButton.on('click', goForward);
    displayPathName = $('#showPathName');

    $('#edit').on('click', gotoEditor);
    $('#launch').on('click', gotoConsole);
    $('#yes,#no').on('click', closeSaveDialog);
    $('#saveButton').on('click', saveClick);

    editorInit();

    //end of event handlers

    newWorker();

    Module.print = console.log;
    Module.printErr = console.error;

    Module.FS.mkdir('/Documents');

    Module.FS.mount(Module.FS.filesystems.IDBWFS, {
      fromEmscripten:       fromEmscripten,
      toEmscriptenReciever: toEmscriptenReciever
    }, '/Documents');

    // sync from persisted state into memory and then
    // refresh the folder view
    Module.FS.syncfs(true, loadSettings); //indexeddb to local
  }

  $(d).ready(ready);
}(document);
