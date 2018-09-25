# Unpolished
This project is unoptimized and unpolished currently. It is scheduled for some love and maintenace as soon as i find the time for it.

# Run and Compile
Create some html files (webpages), inside diffferent folders(websites) with the bash script webcrator.sh. All these pages will be created inside the root_directory. The contents of these pages will be randomized sections from the text_file and occasinal links to other pages either internal(same site/directory) either external(to a different site/directory).
```
./webcreator.sh root_directory text_file w p
```
w: number of sites to create
p: number of pages to create

Compile the Server and the Crawler using the provided Makefiles.

Then run the myhttpd server which will serve the sites and pages you created before on port 8888.
But will also listen for commands on port 9999.
```
./myhttpd  -p 8888 -c 9999 -t 4  -d ./root_directory/
```
You can use telnet to give commands(STATS or SHUTDOWN) to the server.
```
telnet localhost 9999
```

 When designing the webcreator.sh cript, precautions have been taken so that from any page of the server you can crawl to all other pages of it.
 
 Now when the server is up and running, you can start the crawler by specifying: the host,port,command port for the crawler,number of threads, where to save the crawled pages and from which page to start.
 ```
 ./mycrawler -h localhost -p 8888 -c 7777 -t 4 -d ./save_dir/ http://linux01.di.uoa.gr/site0/page0_230.html
```
