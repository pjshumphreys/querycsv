var WORKERFILENAME = 'worker.js';
+function(a) {
  var b = a.createElement("script");
  b.setAttribute("src","extern/script.js");
  b.setAttribute("async","true");
  a.body.appendChild(b);

  a = function(a) {
    var
      b = function() {
        a([
            'extern/b64.js',
            'extern/iscroll-5.1.3.js',
            'extern/picup-2.1.2.js',
            'extern/jquery.history.js',
            'extern/jquery.animate.enhanced-1.11.js',
            'extern/_.debounce.js',
            'helper.js'
          ], c);
      },
      c = function() {
        a('main.js');
      };
      
    a([
        'extern/jquery-1.11.3.js'
      ], b);
  },
  b = setInterval(function() {
    if(typeof($script) !== 'undefined') {
      clearInterval(b);
      a($script);
    }
  }, 1);
}(document);
