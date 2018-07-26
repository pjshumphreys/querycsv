
this["Module"] = (function() {
  Module["qcsvprint"] = 0;
  Module["qcsvprinterr"] = 0;

  Module["print"] = function(text) { return Module.qcsvprint && Module.qcsvprint(text) };
  Module["printErr"] = function(text) { return Module.qcsvprinterr && Module.qcsvprinterr(text) };
