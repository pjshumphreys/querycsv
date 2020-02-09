var fs = require('fs');
var data = require('./stripped.js');

var header = `#include "querycsv.h"

extern long codepoints[18];
extern struct hash2Entry entry;
extern struct hash2Entry* retval;
extern int i;

/*unicode normalization mapping table*/
  #define HASH2SIZE ${data.length}

  static const long
`;

var middle = `;

static const struct hash2Entry hash2[HASH2SIZE] = {
`;

var end = `
};`;
async function f() {
  await (new Promise(resolve => {
    fs.writeFile("test.c", header+(data.map((elem, index, arr) => {
      var abc = `hash2_${index+1}[] = {${elem[1].map((elem2, index2, arr2) => '0x'+(elem2.toString(16).toUpperCase())).join(", ")}}`;
//      console.log(abc);
      return abc;
    }).join(",\n"))+middle+((data.map((elem, index, arr) => {
      var abc = `  { ${'0x'+(((elem[2]*256)+elem[3]).toString(16)).toUpperCase()}, ${elem[1].length}, hash2_${index+1} }`
//      console.log(abc);
      return abc;
    })).join(",\n"))+end, function(err) {
        if(err) {
            return console.log(err);
        }
        resolve();
    });
  }));

  data = data.sort((a,b) => {
    if(a[0] === b[0]) {
      return a[2]<b[2]?-1:(a[2]>b[2]);
    }

    return a[0]<b[0]?-1:(a[0]>b[0]);  
  });
  
  var dedup = {};
  var data2 = [];
  var dups = {};
  
  data.forEach((item, index, arr) => {
    if(dedup.hasOwnProperty("aaa"+item[0])) {
      //console.log(item);
      //console.log(arr[index-1]);      
      
      dups["aaa"+item[0]] = true;
    }
    else {
      dedup["aaa"+item[0]] = true;
      data2.push(item);
    }
  });
  
  /*
  console.log(data.reduce((acc, item) => {
    if(dups.hasOwnProperty("aaa"+item[0])) {
      acc.push([item, JSON.stringify(item)]);
    }
    return acc;
  }, []).sort((a,b) => a[0][0]<b[0][0]?-1:(a[0][0]>b[0][0])).map(item => item[1]).join('\n'));
*/
  await (new Promise(resolve => {
    fs.writeFile("test2.c", header+(data2.map((elem, index, arr) => {
      var abc = `hash2_${index+1}[] = {${(elem[2]?[elem[2],elem[3]]:[elem[3]]).map((elem2, index2, arr2) => '0x'+(elem2.toString(16).toUpperCase())).join(", ")}}`;
//      console.log(abc);
      return abc;
    }).join(",\n"))+middle+((data2.map((elem, index, arr) => {
      var abc = `  { ${'0x'+(elem[0].toString(16)).toUpperCase()}, ${elem[2]?2:1}, hash2_${index+1} }`
//      console.log(abc);
      return abc;
    })).join(",\n"))+end, function(err) {
        if(err) {
            return console.log(err);
        }
        resolve();
    });
  }));
}

f();
