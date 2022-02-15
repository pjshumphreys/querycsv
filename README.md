# QueryCSV

QueryCSV is a command line application to query files containing comma separated values \([csv files](https://en.wikipedia.org/wiki/Comma-separated_values)\) using files containing [sql](https://en.wikipedia.org/wiki/SQL)-like query syntax that supports Unicode text data.

Versions are available for POSIX based computer operating systems \(such as Linux and Mac OS X\), HTML5 devices, Microsoft Windows, DOS, Classic Mac OS, Commodore Amigas, Atari STs, RISC OS, the ARM Evaluation System add-on for the BBC Micro \(or something compatible with it like [BeebEm](http://www.mkw.me.uk/beebem/) or [PiTubeDirect](https://github.com/hoglet67/PiTubeDirect/wiki)\) the Commodore 64 \(with an easyflash cartridge\), the ZX Spectrum \(with either the ESXDOS, ResiDOS or plus3dos disk systems\) and z80 based CP/M computer systems \(with extra memory mapper support for the MSXDOS 1 and 2 operating systems on MSX computers, which are backwards compatible with CP/M\). The 8-bit builds are somewhat slow at present though.

## Example usage

Firstly, download the pre-built versions from here: [download link](https://github.com/pjshumphreys/files/raw/master/querycsv-latest.zip) or use the HTML5 build available here: [link](https://pjshumphreys.github.io/querycsv/)

Then open up notepad or your favourite text editor and place the following in a file called employees.csv:

```
LastName,DepartmentID
Rafferty,31
Jones,33
Heisenberg,33
Robinson,34
Smith,34
Williams,""
```

Now create a second file called departments.csv with the following contents:

```
DepartmentID,DepartmentName
31,Sales
33,Engineering
34,Clerical
35,Marketing
```

Then create a file called getEngineers.qry to specify the data you want to retrieve:

```
select a.lastName as surname, b.DepartmentName as department
from 'employees.csv' as a
join 'departments.csv' as b on b.DepartmentID = a.DepartmentID
where a.DepartmentID = 33
```

Finally, on the command line run the query against the data files using the following command:

```
querycsv getEngineers.qry
```

After this, the following output will be produced:

```
surname, department
Jones, Engineering
Heisenberg, Engineering
```

## Why?

My hobby is retro computing and I recently came across and bought a Toshiba T1100 plus, which looks like this:

<p align="center">
  <img alt="Toshiba T1100 Plus" src="https://raw.githubusercontent.com/pjshumphreys/files/master/t1100plus.png" width="400" />
</p>

This was one of the first mass market laptop PCs and was released by Toshiba in 1986. It has an Intel 80C86 processor \(a low power version of the 8086\), 640Kb of RAM and dual double density floppy drives but no hard disk.

I created a MS-DOS 6.22 boot disk, and began to wonder whether the machine could still be put to good use today? MS-DOS 6.22 comes with a nice text editor, so that would cover basic word processing. However, the ability to work with more structured data in a way that could be automated was lacking.

To fill this void, I briefly considered finding a copy of Lotus 123, as it would run on the machine. But this program stores its data in a binary file format that's now difficult for modern computer systems to work with. The QBASIC programming language DOS comes with can be used to read and write plain text data records, but it only really supports fixed length text fields and for each set of data I might want to produce it would require a complex, lengthy ad hoc program.

What I really wanted was to run Node.js \([https://nodejs.org/en/](https://nodejs.org/en/)\) on the machine as using javascript to manipulate such structured data would be comparatively easy, and the data could be stored in plain text json files. However, Node.js is far too large to fit in 640Kb of RAM and onto a floppy disk.

Another option might've been to compile Sqlite \([https://www.sqlite.org/](https://www.sqlite.org/)\) for the machine with a simple command line front-end. But when I tried this it compiled to a 960Kb executable - still too large. Sqlite also has the disadvantage that it also uses a binary file format. In addition, any data would still need to be imported and exported to and from the database file to be manipulated.

What I needed was a simple command line tool that could read and write structured data from a simple, plain text file format (such as CSV files) using a succinct query language to save disk space and the time needed to write each script (such as SQL).

So, I decided to write my own...

### Additional aims

After starting work on this program, It became tedious to transfer each compiled executable from my modern, Linux based laptop that I was using for development to the Toshiba. So as I had been attempting to use just the C language and its standard library, I produced a Linux version of the program to ease testing.

From that, I realised that I have several other old computer systems that could each benefit from a version of this program. I could also gain more knowledge of how to produce cross platform, embedded code from the endeavour. So more versions targeting different operating systems were produced.

Lastly, another problem with programs that were contemporary to MS-DOS such as lotus 123 is that they only really work with the ASCII character set, and I'd always wondered how Unicode collation and sorting worked. So finally I added the ability to convert the text data between Unicode and the various character sets that each supported target system uses natively.

## Coding style

As my day job is as a web developer, my C coding style is quite JavaScript-like with verbose function and variable names. I also indent with 2 spaces and mostly follow the Stroustrup variant of Kernighan and Ritchie style, so I code like this:

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if(argc == 2 && strcmp(argv[1], "foo") == 0) {
    fputs("bar\n", stdout);
  }
  else {
    fputs("baz\n", stdout);
  }

  return EXIT_SUCCESS;
}
```

I also avoid global and static variables when I can as I feel it eases testing for me.

## Contributing

I'd love it if you appreciate this program and would like to help in developing it. Like all projects, there are some bugs that need working on and also some new features I'd like to add.

I can be reached at [querycsv@gmail.com](mailto:querycsv@gmail.com)

## License

QueryCSV is MIT licensed and is copyright (c) 2015-2022 Paul Humphreys
