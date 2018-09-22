# Unpolished

This project is unoptimized and unpolished currently. It is scheduled for some love and maintenace as soon as i find the time for it.

# Web-Creator_Server_Crawler

RUN SERVER
valgrind --leak-check=full --show-leak-kinds=all ./myhttpd  -p 8888 -c 9999 -t 4  -d ./root_directory/

SERVER ON BROWSER
localhost:8888/site0/page_name.html

SEND COMMANDS TO SERVER
telnet localhost 9999

RUN CRAWLER
valgrind --leak-check=full --show-leak-kinds=all ./mycrawler -h localhost -p 8888 -c 7777 -t 4 -d ./save_dir/ http://linux01.com/site0/page0_230.html
