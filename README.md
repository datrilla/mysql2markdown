# mysql2markdown
generate mysql table description to markdown format

# environment
Linux

# compile
```
gcc -g mysql2markdown.c  -lrt -L/usr/lib64/mysql  -lmysqlclient  -o  mysql2markdown
```

# run
all set by command line 
```
./mysql2markdown -uUSER  -pPASSWORD -hIP -PPORT -DDBNAME
```

default port
```
./mysql2markdown -uUSER  -pPASSWORD -hIP  -DDBNAME

```

# reference
[GO Version](https://github.com/alicfeng/mysql_markdown)


