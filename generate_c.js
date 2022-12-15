const walk = require('walk');

const list = [];

const putFirst = [
  'getGroupedColumns.c',
  'getValue.c',
  'parse_params.c',
  'runCommand.c',
  'updateRunningCounts.c',
  'walkRejectRecord.c'
];

const walker = walk.walk('./split', {});

walker.on('file', (root, fileStats, next) => {
  list.push(fileStats.name);

  next();
});

walker.on('errors', (root, nodeStatsArray, next) => {
  next();
});

walker.on('end', () => {
  console.log(list.sort((a, b) => {
    if(putFirst.includes(a) && !putFirst.includes(b)) {
      return -1;
    }

    else if(putFirst.includes(b)) {
      return 1;
    }
    
    if(b > a) {
      return -1;
    }
    
    return !!(b<a);
  }).reduce((acc, item) => acc + ' split/' + item, ''));
});
