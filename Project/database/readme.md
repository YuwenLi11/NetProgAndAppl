Initial version of database for testing and viewing architechture

Testing environment: lunix

Configuration:
database username:root
passwd:dees
database name:project

please change those information in project.c before compile

to compile, use
gcc project.c -lmysqlclient -o project

before ./project, make sure these is a database named "peoject" exist.
