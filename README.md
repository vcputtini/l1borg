## l1borg

The [1]BorgBackup is an amazing program to backup both local and remote using deduplication techniques. It is very simple to use and its features allow us to create scripts to customize and automate your operation.

<b>l1borg</b> is a utility designed to solve some of my needs for automating the use of BorgBackup.<br>
The <b>l1borg</b> load the settings for the backup running from the XML file <i>l1borg.xml</i>, and run the BorgBackup with the necessary parameters.

<b>l1borg</b> doesn't handle remote repositories, but only local.<br>
At thist time, I don't really intend to make it a utility with many functions, and its evolution is linked to my usage needs, but feel free to use it as a starting point for a fork or new ideas.

<b>l1borg</b> was coded entirely in C++. As I consider QtCreator(tm) to be an excellent development environment, I use it for my projects, even if these don't directly involve using the Qt(tm) tools.

### Dependencies for compilation:
gcc/g++ which meets the c++17
Libs: tinyxml2 and Boost

### My environment<br>
Fedora 33<br>
gcc (GCC) 10.2.1 20201125<br>
QtCreator 4.14 (Qt 5.15.2)<br>
cmake version 3.18.4<br>
Boost 1.73<br>
tinyxml2-7.0.1-5<br>

### Flowchart
![](flowchart.png)

### Usage<br>

__l1borg__ --help<br>
Valid options are:<br>
--init-repo   : Initialize all repositories.<br>
--make-backup : Make backup.<br>
--cmd         : Execute a BorgBackup command.<br>
&emsp;&emsp;&emsp;&emsp;Double quotes are mandatory:<br>
&emsp;&emsp;&emsp;&emsp;e.g.: l1borg --cmd info PATH/REPONAME --prefix=srv1<br>
(the _--list_, _--info_ and _--diff_ commands are only simplifications of analogous commands in BorgBackup.)<br>
--info        : all | REPONAME[::FILENAME]<br>
&emsp;&emsp;&emsp;&emsp;NOTE: Enter only the repository name without the path: e.g: --info R1 | --info R1::archive<br>
--list        : all | REPONAME[::FILENAME]"<br>
&emsp;&emsp;&emsp;&emsp;NOTE: Enter only the repository name without the path: e.g: --list R1 | --list R1::archive<br>
--diff        : REPONAME::ARCHIVE1 ARCHIVE2<br>
&emsp;&emsp;&emsp;&emsp;NOTE: Enter only the repository name without the path<br><br>

__l1borg.xml__ Sample: See docs/l1borg.xml


## References
[1] BorgBackup <https://borgbackup.readthedocs.io/en/stable/index.html><br>
Copyright (C) 2015-2020 The Borg Collective
<https://borgbackup.readthedocs.io/en/stable/authors.html><br>
Copyright (C) 2010-2014 Jonas Borgström<br> <jonas@borgstrom.se> All rights reserved.<br>
