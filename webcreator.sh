#!/bin/bash

#synatx: contains_element stringkey stringarray
#1 if stringkey is contained in string array, 0 if not
contains_element() {
  local key=$1
  shift #shift arguments to ignore the key
  local array=("$@")  #search this array for the key
  for i in "${array[@]}"; do
    if [ "$i" == "$key" ]; then
      echo 1
      return
    fi
  done
  echo 0
}

#CHECK COMMAND LINE ARGUMENTS**************************************************
if [ ! -d $1 ]; then
  echo "directory " $1 " doesn't exist"
  exit 3
fi

if [ ! -e $2 ]; then
  echo "file " $2 " doesn't exist"
  exit 4
fi

if [[ ! $3 =~ ^[0-9]+$ ]]; then
  echo $3 "not an integer"
  exit 5
fi

if [[ ! $4 =~ ^[0-9]+$ ]]; then
  echo $4 "not an integer"
  exit 6
fi

file_size=$(wc -l < "$2")
if [ $file_size -lt 10 ]; then
  echo "file size must be > 10000 lines"
  exit 7
fi

if [ $3 -lt 2 ]; then
  echo $3 "w must be >= 2"
  exit 8
fi

#purge
if [ "$(ls -A "$1")" ]; then
  echo "#Warning: directory is full, purging ..."
  rm -rf "$1"
  mkdir "$1"
fi

#Load textfile to memory*******************************************************
file=()
file_index=0
while IFS= read -r line; do
  file[$file_index]=$line
  file_index=$(($file_index+1))
done < $2

#Create w sites****************************************************************
declare -A page_names #2d array of page names per website
declare -A page_has_link #same 2d array with 0 for no incoming link
                        #and 1 if these is an incoming link to that page
for ((i=0;i<$3;i++)); do  #for each site
  site_name="site$i"
  mkdir "$1/$site_name"
  #name p pages in each site
  for ((j=0;j<$4;j++)); do  #for each page of the site
    #pick a random name for the page, dont pick an existing one
    rand=$((RANDOM%10000))
    page_names[$i,$j]="$site_name/page${i}_${rand}.html"
    while [ -e ${page_names[$i,$j]} ]; do
      rand=$((RANDOM%10000))
      page_names[$i,$j]="$site_name/page${i}_${rand}.html"
    done
    page_has_link[$i,$j]=0
  done
done

#Arithmetics as designated by the assingment
f=$((($4/2)+1)) #num of internal links
q=$((($3/2)+1)) #num of external links
num_links=$(($f+$q))  #total number of links
if [ $q -gt $((($3-1)*$4)) ]; then
  echo "q=$q but there can only be $((($3-1)*$4)) external links. Aborting..."
  exit 8
fi

#Create p pages in each of w sites
for ((i=0;i<$3;i++)); do  #for each site
  echo "  #Creating web site$i ..."
  for ((j=0;j<$4;j++)); do  #for each page of the site
    echo "    #Creating web page ${page_names[$i,$j]}"

    #choose random numbers
    k=$((RANDOM % $(($file_size-2000))+1))  #from line k
    m=$(((RANDOM%999)+1001))
    num_lines=$(($m/$num_links)) #num of lines before each link
    l=$(($k+$num_lines))  #till line l

    #create internal and external link sets****************************
    links=()
    links_index=0
    for ((a=0;a<$f;a++)); do
      #pick link to a random page...
      randpage=$((RANDOM%$4))
      link=${page_names[$i,$randpage]}
      #...as long as its not the current page
      while [ $randpage -eq $j ]; do
        #allow self. internal links if num_pages=f
        if [ $4 -eq $f ]; then
          break
        fi
        randpage=$((RANDOM%$4))
        link=${page_names[$i,$randpage]}
      done
      #... and has not already been selected
      while [ $(contains_element $link "${links[@]}") -eq 1 ]; do
        randpage=$((RANDOM%$4))
        link=${page_names[$i,$randpage]}
      done
      links[$links_index]=$link
      page_has_link[$i,$randpage]=1  #this page now has an incoming link
      links_index=$(($links_index+1))
    done
    for ((a=0;a<$q;a++)); do
      #pick link to a random site and a random page from that site...
      randsite=$((RANDOM%$3))
      randpage=$((RANDOM%$4))
      link=${page_names[$randsite,$randpage]}
      #...as long as the link is external and has not already been used
      while [ $randsite -eq $i ] ||
            [ $(contains_element $link "${links[@]}") -eq 1 ]; do
          randsite=$((RANDOM%$3))
          randpage=$((RANDOM%$4))
          link=${page_names[$randsite,$randpage]}
      done
      links[$links_index]=$link
      page_has_link[$randsite,$randpage]=1  #this page now has an incoming link
      links_index=$(($links_index+1))
    done

    #open fd to write to the page*********************
    exec 3<> "$1/${page_names[$i,$j]}"
    echo "<!DOCTYPE html><html><body>" >&3
    #write contents
    links_index=0
    for ((a=0;a<$num_links;a++)); do  #for every link
      #write n lines from textfile to the page
      for ((b=$k;b<$l;b++)); do
          echo ${file[$b]} >&3
      done
      #look at the next set of lines
      k=$l
      l=$(($l+$num_lines))
      #add the link
      echo "      Adding link to ${links[$links_index]}"
      echo "<a href=\"../${links[$links_index]}\">${links[$links_index]}</a>" >&3
      links_index=$(($links_index+1))
    done
    #close fd to the page
    echo "<body></html>" >&3
    exec 3>&-
  done
done

#Check if all pages have incoming links****************************************
flag=0
for ((i=0;i<$3;i++)); do  #for each site
  for ((j=0;j<$4;j++)); do  #for each page of the site
    if [ ${page_has_link[$i,$j]} -eq 0 ]; then
      flag=1
      echo "Page ${page_names[$i,$j]} has no incoming links."
    fi
  done
done
if [ $flag -eq 0 ]; then
  echo "All pages have at least one incoming link."
fi
