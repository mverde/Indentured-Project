Code Sample:
Look at test.cpp for structure, make sure to copy over the include headers.

Sample Compile Instructions:
1. Place your cpp file in /home/ubuntu/cppscripts using WinSCP
2. Sign into the ec2 Ubuntu server via PuTTY (Instructions in the AWS folder's README)
3. $ cd /home/ubuntu/cppscripts
4. $ sudo g++ -Wall -I/usr/include/cppconn -o test test.cpp -L/usr/lib -lmysqlcppconn
5. $ ./test
     SELECT * FROM pois
     