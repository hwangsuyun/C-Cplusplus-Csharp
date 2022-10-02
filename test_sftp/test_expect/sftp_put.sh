#!/usr/bin/expect -f

spawn sftp -oPort=2222 smw@133.9.100.5
expect "password:" { send "smw\r" }
expect "sftp>" { send "cd syhwang\r" }
expect "sftp>" { send "put 1.txt\r" }
expect "sftp>" { send "quit\r" }
interact
