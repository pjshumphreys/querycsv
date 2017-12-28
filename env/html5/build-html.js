
var
  walk = require('walk'),
  fs = require('fs'),
  cheerio = require('cheerio'),
  normalizeWhitespace = require("normalize-html-whitespace"),
  filesList = [],
  $,
  script,
  style,
  filenames = {
    WORKERFILENAME: "\'worker.min.js\'",
    SVCWORKERFILENAME: "\'svcworker.min.js\'",
    APPCACHEFILENAME: "\'appcache.html\'"
  },
  offline = {
    VERSION: +new Date(),
    FILESLIST: []
  },
  offline2 = {
    VERSION: +new Date(),
    FILESLIST: []
  },
  matchOperatorsRe = /[|\\{}()\[\]^$+*?.]/g,
  xxx;

function replaceAll(str, mapObj) {
  return str.replace(
    new RegExp(
        Object.
          keys(mapObj).
          map(x => x.replace(matchOperatorsRe, "\\$&")).
          join("|"),
        "gi"
      ),
    matched => mapObj[matched]
  );
}

function htmlRead(err,data) {
  if (err) {
    console.log(err);
    process.exit(1);
  }

  $ = cheerio.load(data);

  $('link[rel=stylesheet]').remove();

  $('head').append('<style />');
  $('style').text("@@@style@@@");
  $('#code').removeAttr('src').text("@@@script@@@");


  fs.readFile('output.min.js', 'utf8', jsRead);
};

function jsRead(err, data) {
  if (err) {
    console.log(err);
    process.exit(1);
  }

  script = replaceAll(data.toString().replace(/\/\*([\s\S]*?)\*\//g,"").replace(/\n\n*/g, "\n"), filenames);

  fs.readFile('style.min.css', 'utf8', cssRead);
}

function cssRead(err, data) {
  if (err) {
    console.log(err);
    process.exit(1);
  }

  var extras = {
    '@@@style@@@': data.toString(),
    '@@@script@@@': script,
    '<!DOCTYPE html> <': '<!DOCTYPE html>\n<'
  };

  fs.writeFile(
      'build/index.html',
      replaceAll(normalizeWhitespace($.html()), extras),
      fileWritten
    );
}

function fileWritten(err) {
  if(err) {
    console.log(err);
    process.exit(1);
  }

  console.log("index.html generated!");

  walker = walk.walk("build", {
    followLinks: false,
    filters: [".gz"]
  });

  walker.on("file", function (root, fileStats, next) {
    if(
        !/\.(appcache|gz)$/.test(fileStats.name) &&
        fileStats.name !== 'svcworker.min.js' &&
        fileStats.name !== 'appcache.html'
    ) {
      filesList.push(root.replace(/^build/, '')+'/'+fileStats.name);
    }

    next();
  });

  walker.on("end", function () {
    fs.readFile("svcworker.js", 'utf8', svcRead);
  });
}

function svcRead(err, data) {
  offline.FILESLIST = JSON.stringify(filesList);
  var content = replaceAll(data.toString(), offline);

  fs.writeFile('svcworker.temp.js', content, svcWritten);
}

function svcWritten(err) {
  fs.readFile("offline.appcache", 'utf8', cacheRead);
}

function cacheRead(err, data) {
  offline2.FILESLIST = filesList.join("\n");
  var content = replaceAll(data.toString(), offline2);

  fs.writeFile('build/offline.appcache', content, () =>{});
}

fs.readFile('index-debug.html', 'utf8', htmlRead);
