var
  fs = require('fs'),
  cheerio = require('cheerio'),
  normalizeWhitespace = require("normalize-html-whitespace"),
  $,
  script,
  style,
  xxx;

function htmlRead(err,data) {
  if (err) {
    console.log(err);
    process.exit(1);
  }
  
  $ = cheerio.load(data);

  $('link[rel=stylesheet]').remove();

  $('head').append('<style />');
  $('style').text("@@@style@@@");
  $('script').removeAttr('src').text("@@@script@@@");

  
  fs.readFile('output.min.js', 'utf8', jsRead);
};

function jsRead(err, data) {
  if (err) {
    console.log(err);
    process.exit(1);
  }

  script = data.toString().replace(/\/\*([\s\S]*?)\*\//g,"").replace(/\n\n*/g, "\n").replace("WORKERFILENAME", '"worker.min.js"');

  fs.readFile('style.min.css', 'utf8', cssRead);
}

function cssRead(err, data) {
  if (err) {
    console.log(err);
    process.exit(1);
  }

  style = data.toString();
  
  fs.writeFile('build/index.html', normalizeWhitespace($.html()).replace('@@@style@@@', style).replace('@@@script@@@', script).replace('<!DOCTYPE html> <', '<!DOCTYPE html>\n<'), fileWritten);
}

function fileWritten(err) {
  if(err) {
    console.log(err);
    process.exit(1);
  }

  console.log("index.html generated!");
}
 
fs.readFile('index-debug.html', 'utf8', htmlRead);
