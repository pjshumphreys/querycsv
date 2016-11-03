/*  Production steps of ECMA-262, Edition 5, 15.4.4.18 */
/*  Reference: http://es5.github.io/#x15.4.4.18 */
if (!Array.prototype.forEach) {

  Array.prototype.forEach = function(callback, thisArg) {

    var T, k;

    if (this == null) {
      throw new TypeError(' this is null or not defined');
    }

    /*  1. Let O be the result of calling ToObject passing the |this| value as the argument. */
    var O = Object(this);

    /*  2. Let lenValue be the result of calling the Get internal method of O with the argument "length". */
    /*  3. Let len be ToUint32(lenValue). */
    var len = O.length >>> 0;

    /*  4. If IsCallable(callback) is false, throw a TypeError exception. */
    /*  See: http:/* es5.github.com/#x9.11 */ */
    if (typeof callback !== "function") {
      throw new TypeError(callback + ' is not a function');
    }

    /*  5. If thisArg was supplied, let T be thisArg; else let T be undefined. */
    if (arguments.length > 1) {
      T = thisArg;
    }

    /*  6. Let k be 0 */
    k = 0;

    /*  7. Repeat, while k < len */
    while (k < len) {

      var kValue;

      /*  a. Let Pk be ToString(k). */
      /*    This is implicit for LHS operands of the in operator */
      /*  b. Let kPresent be the result of calling the HasProperty internal method of O with argument Pk. */
      /*    This step can be combined with c */
      /*  c. If kPresent is true, then */
      if (k in O) {

        /*  i. Let kValue be the result of calling the Get internal method of O with argument Pk. */
        kValue = O[k];

        /*  ii. Call the Call internal method of callback with T as the this value and */
        /*  argument list containing kValue, k, and O. */
        callback.call(T, kValue, k, O);
      }
      /*  d. Increase k by 1. */
      k++;
    }
    /*  8. return undefined */
  };
}

/*! http:/* mths.be/fromcodepoint v0.1.0 by @mathias */ */
if (!String.fromCodePoint) {
	(function() {
		var defineProperty = (function() {
			/*  IE 8 only supports `Object.defineProperty` on DOM elements */
			try {
				var object = {};
				var $defineProperty = Object.defineProperty;
				var result = $defineProperty(object, object, object) && $defineProperty;
			} catch(error) {}
			return result;
		}());
		var stringFromCharCode = String.fromCharCode;
		var floor = Math.floor;
		var fromCodePoint = function(codePoints) {
			var MAX_SIZE = 0x4000;
			var codeUnits = [];
			var highSurrogate;
			var lowSurrogate;
			var index = -1;
			var length = arguments.length;
			if (!length) {
				return '';
			}
			var result = '';
			while (++index < length) {
				var codePoint = Number(arguments[index]);
				if (
					!isFinite(codePoint) || /*  `NaN`, `+Infinity`, or `-Infinity` */
					codePoint < 0 || /*  not a valid Unicode code point */
					codePoint > 0x10FFFF || /*  not a valid Unicode code point */
					floor(codePoint) != codePoint /*  not an integer */
				) {
					throw RangeError('Invalid code point: ' + codePoint);
				}
				if (codePoint <= 0xFFFF) { /*  BMP code point */
					codeUnits.push(codePoint);
				} else { /*  Astral code point; split in surrogate halves */
					/*  http:/* mathiasbynens.be/notes/javascript-encoding#surrogate-formulae */ */
					codePoint -= 0x10000;
					highSurrogate = (codePoint >> 10) + 0xD800;
					lowSurrogate = (codePoint % 0x400) + 0xDC00;
					codeUnits.push(highSurrogate, lowSurrogate);
				}
				if (index + 1 == length || codeUnits.length > MAX_SIZE) {
					result += stringFromCharCode.apply(null, codeUnits);
					codeUnits.length = 0;
				}
			}
			return result;
		};
		if (defineProperty) {
			defineProperty(String, 'fromCodePoint', {
				'value': fromCodePoint,
				'configurable': true,
				'writable': true
			});
		} else {
			String.fromCodePoint = fromCodePoint;
		}
	}());
}

if (!String.prototype.ucLength) {
    String.prototype.ucLength = function() {
        /*  this solution was taken from  */
        /*  http:/* stackoverflow.com/questions/3744721/javascript-strings-outside-of-the-bmp */ */
        return this.length - this.split(/[\uD800-\uDBFF][\uDC00-\uDFFF]/g).length + 1;
    };
}

if (!String.prototype.codePointAt) {
    String.prototype.codePointAt = function (ucPos) {
        if (isNaN(ucPos)){
            ucPos = 0;
        }
        var str = String(this);
        var codePoint = null;
        var pairFound = false;
        var ucIndex = -1;
        var i = 0;  
        while (i < str.length){
            ucIndex += 1;
            var code = str.charCodeAt(i);
            var next = str.charCodeAt(i + 1);
            pairFound = (0xD800 <= code && code <= 0xDBFF && 0xDC00 <= next && next <= 0xDFFF);
            if (ucIndex == ucPos){
                codePoint = pairFound ? ((code - 0xD800) * 0x400) + (next - 0xDC00) + 0x10000 : code;
                break;
            } else{
                i += pairFound ? 2 : 1;
            }
        }
        return codePoint;
    };
}

if (!String.fromCodePoint) {
    String.fromCodePoint = function () {
        var strChars = [], codePoint, offset, codeValues, i;
        for (i = 0; i < arguments.length; ++i) {
            codePoint = arguments[i];
            offset = codePoint - 0x10000;
            if (codePoint > 0xFFFF){
                codeValues = [0xD800 + (offset >> 10), 0xDC00 + (offset & 0x3FF)];
            } else{
                codeValues = [codePoint];
            }
            strChars.push(String.fromCharCode.apply(null, codeValues));
        }
        return strChars.join("");
    };
}

if (!String.prototype.ucCharAt) {
    String.prototype.ucCharAt = function (ucIndex) {
        var str = String(this);
        var codePoint = str.codePointAt(ucIndex);
        var ucChar = String.fromCodePoint(codePoint);
        return ucChar;
    };
}

if (!String.prototype.ucIndexOf) {
    String.prototype.ucIndexOf = function (searchStr, ucStart) {
        if (isNaN(ucStart)){
            ucStart = 0;
        }
        if (ucStart < 0){
            ucStart = 0;
        }
        var str = String(this);
        var strUCLength = str.ucLength();
        searchStr = String(searchStr);
        var ucSearchLength = searchStr.ucLength();
        var i = ucStart;
        while (i < strUCLength){
            var ucSlice = str.ucSlice(i,i+ucSearchLength);
            if (ucSlice == searchStr){
                return i;
            }
            i++;
        }
        return -1;
    };
}

if (!String.prototype.ucLastIndexOf) {
    String.prototype.ucLastIndexOf = function (searchStr, ucStart) {
        var str = String(this);
        var strUCLength = str.ucLength();
        if (isNaN(ucStart)){
            ucStart = strUCLength - 1;
        }
        if (ucStart >= strUCLength){
            ucStart = strUCLength - 1;
        }
        searchStr = String(searchStr);
        var ucSearchLength = searchStr.ucLength();
        var i = ucStart;
        while (i >= 0){
            var ucSlice = str.ucSlice(i,i+ucSearchLength);
            if (ucSlice == searchStr){
                return i;
            }
            i--;
        }
        return -1;
    };
}

if (!String.prototype.ucSlice) {
    String.prototype.ucSlice = function (ucStart, ucStop) {
        var str = String(this);
        var strUCLength = str.ucLength();
        if (isNaN(ucStart)){
            ucStart = 0;
        }
        if (ucStart < 0){
            ucStart = strUCLength + ucStart;
            if (ucStart < 0){ ucStart = 0;}
        }
        if (typeof(ucStop) == 'undefined'){
            ucStop = strUCLength - 1;
        }
        if (ucStop < 0){
            ucStop = strUCLength + ucStop;
            if (ucStop < 0){ ucStop = 0;}
        }
        var ucChars = [];
        var i = ucStart;
        while (i < ucStop){
            ucChars.push(str.ucCharAt(i));
            i++;
        }
        return ucChars.join("");
    };
}

if (!String.prototype.ucSplit) {
    String.prototype.ucSplit = function (delimeter, limit) {
        var str = String(this);
        var strUCLength = str.ucLength();
        var ucChars = [];
        if (delimeter == ''){
            for (var i = 0; i < strUCLength; i++){
                ucChars.push(str.ucCharAt(i));
            }
            ucChars = ucChars.slice(0, 0 + limit);
        } else{
            ucChars = str.split(delimeter, limit);
        }
        return ucChars;
    };
}



if (!Array.prototype.filter) {
  Array.prototype.filter = function(fun/*, thisArg*/) {
    'use strict';

    if (this === void 0 || this === null) {
      throw new TypeError();
    }

    var t = Object(this);
    var len = t.length >>> 0;
    if (typeof fun !== 'function') {
      throw new TypeError();
    }

    var res = [];
    var thisArg = arguments.length >= 2 ? arguments[1] : void 0;
    for (var i = 0; i < len; i++) {
      if (i in t) {
        var val = t[i];

        /*  NOTE: Technically this should Object.defineProperty at */
        /*        the next index, as push can be affected by */
        /*        properties on Object.prototype and Array.prototype. */
        /*        But that method's new, and collisions should be */
        /*        rare, so use the more-compatible alternative. */
        if (fun.call(thisArg, val, i, t)) {
          res.push(val);
        }
      }
    }

    return res;
  };
}

/* read file as JSON, parse it */
var fs = require('fs');

var data = JSON.parse(fs.readFileSync("./weired2.txt", 'utf8'));

var i, len;

var string = "[\n";

/*
data = data.filter(function(elem){
return (elem[2].indexOf("ARABIC") === -1 && elem[2].indexOf("MATHEMATICAL") === -1 && elem[2].indexOf("CJK COMPATIBILITY") === -1);
});



*/

/* use fromcodepoint to write to the file in utf-8 */
for(i = 0, len = data.length; i < len; i++) {
  var temp =  data[i][1].split("");
  /* string+= '[' + (i+5722) + "[] = {"; */
    data[i][2] = [];
  for(j =0;j < temp.length; j++) {
    var str = "" + temp[j].codePointAt(0).toString(16).toUpperCase();
    var pad = "0000";
    data[i][2].push(pad.substring(0, pad.length - str.length) + str);
    if(j !== 0) {
   /*    string+= ", " */
    }
   /*  string+= '0x' + data[i][2][j]; */
  }

 /*  string+= '},  /* '+ data[i][0] + "\n"; */ */
}

/* string += "static const hash1Entry[] = {\n"; */
for(i = 0, len = data.length; i < len; i++) {
var str = "" + data[i][0].codePointAt(0).toString(16).toUpperCase();
    var pad = "0000";
  string += '["' +
    /* String.fromCodePoint(parseInt( */
    pad.substring(0, pad.length - str.length) + str +/* , 16)) + */
    '", "' +
    data[i][2].join(" ") +
    '", "' +data[i][0] + '"],\n';
}
string += "]";

fs.writeFileSync('./weired.json', string, 'utf8');



