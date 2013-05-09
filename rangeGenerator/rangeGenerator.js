var fs = require('fs');
var program = require('commander');
var colors = require('colors');

program
  .version('0.0.1')
  .usage('[options] <file ...>')
  .option('-f, --from <n>', 'An integer argument', parseInt)
  .option('-t, --to <n>', 'A integer argument', parseInt)
  .option('-s, --step <n>', 'A integer argument', parseInt)
  .option('-d, --dir <s>', 'directory to put files into')
  .parse(process.argv);

if (!program.from && program.from != 0) console.log ('please set a starting value with -f or --from'.yellow);
if (!program.to) console.log ('please set a finish value with -t or --to'.yellow);
if (!program.step) console.log ('please set the step with -s or --step'.yellow + ' eg: -s 100 (will generate 0 to 99)');

for (var x = program.from; x < program.to; x++){
	console.log('generating file: ' +  x + ' : ' + (x+1));
	var data = '';
	for (var z = x * program.step; z < ((x+1) * program.step); z++){
		data = data + z + '\n';
	}	
	fs.writeFileSync(program.dir + x + '-' + (x+1) + '.txt', data);
}