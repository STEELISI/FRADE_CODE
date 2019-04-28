#=======================================================#
#       Install lynx before running                     #
#=======================================================#
from subprocess import Popen, PIPE, STDOUT
import sys 
import os


#============================#
#   Website                  #
#============================#
domain = 'localhost'
website = 'http://localhost'
main_req_file = 'main.txt'
embedded_req_file = 'embedded.txt'
approved_object_list_file = 'aol.txt'

length = len(website)
main=set()
embedded_set=set()
main_all=set()
embedded = {}

#=======================================================================================================#
# This function fetches all the requires files to load a page                                           #
#=======================================================================================================#

def embedded_urls_in_webpage(url):
    #============================#
    #   Extract Links Command    #
    #============================#
    cmd = 'wget -p ' + str(url) 
    urls_list = []

    p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)  
    for line in p.stdout:  
        line = line.rstrip()  
        if("http" in line):
            link = line.split("--")
            if(len(link) >= 3 and link[2].strip().endswith("/")):
                link[2] = link[2].strip()
                link[2] = link[2][0:len(link[2])-1]
            if(len(link) >= 3 and (not(link[2].strip() == website or link[2].strip() == url.strip() ))):
                urls_list.append(link[2].strip())            
    return urls_list

#==============================================================================================================#
# This function fetches all the main requests plus checks if the url is a leaf/non-leaf on the website graph   #
#==============================================================================================================#

def main_urls_in_webpage(url):
    #============================#
    #   Extract Links Command    #
    #============================#
    cmd = 'lynx -dump -listonly ' + str(url)
    urls_list = []

    p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    for line in p.stdout:
        line = line.rstrip()
        #print(line)
        if("http" in line):
            link = line.split()
            if(len(link) >= 2 and link[1].strip().endswith("/")):
                link[1] = link[1].strip()
                link[1] = link[1][0:len(link[1])-1]
            if( len(link) >= 2 and (not(link[1].strip() == website or link[1].strip() == url.strip()  ))):
                urls_list.append(link[1].strip())
    return urls_list



# ***************************************#
# ===== The main driver program =========#
# ***************************************#

if __name__ == '__main__':

     links = []
     main_links = []
     links = embedded_urls_in_webpage(website)
     main.add(website)
     main_all.add(website)
     embedded[website] = links
     main_links = main_urls_in_webpage(website)
     flag=0
     if(len(main_links) > 0):
         flag=1

     while True:
         main_links_tmp = []
         length_of_main_before = len(main) 
         if(flag == 0):
             break
         for link in main_links:
             main_links1 = []
             #print(link)
             if(link.strip().endswith("/")):
                 link = link.strip()
                 link = link[0:len(link)-1]

             if(domain in link and (link not in main)):
                 list_of_links = []
                 list_of_links = embedded_urls_in_webpage(link)
                 if(len(list_of_links) == 0):
                     main_all.add(link)
                     continue
                 main.add(link)
                 embedded[link] = list_of_links
                 main_links1 = main_urls_in_webpage(link)
                 main_links_tmp.extend(main_links1)
         main_links = []
         main_links.extend(main_links_tmp)
         length_of_main_after = len(main)
         if(len(main_links) == 0 or length_of_main_before == length_of_main_after):
             flag=0
     #print("============================================================")
     #print("         ALL POSSIBLE MAIN ELEMENTS                         ")
     #print("============================================================")
     #for element in main_all:
     #    print(element)

     print("============================================================")
     print("         WRITING ALL MAIN REQUESTS                          ")
     print("============================================================")
     file = open(main_req_file,'w') 
     for element in main:
         if element ==  website:
             file.write("\n/")
         else:
             file.write("\n" + element[length:])
     file.close()

     file = open(approved_object_list_file,'w')
     print("============================================================")
     print(" WRITING ALL EMBEDDED REQUESTS  FOR THE MAIN REQUESTS       ")
     print("============================================================")
     for k,v in embedded.iteritems():
         str_aol = "\n"
         if k ==  website:
             str_aol = str_aol + "/"
         else:
             str_aol = str_aol + k[length:]
         for val in v:
             str_aol = str_aol + " "
             str_aol = str_aol + val[length:]
             embedded_set.add(val[length:])
         file.write(str_aol)
     file.close()


     print("============================================================")
     print("         WRITING ALL EMBEDDED REQUESTS                      ")
     print("============================================================")
     file = open(embedded_req_file,'w')
     for val in embedded_set:
         file.write("\n" + val)
     file.close()


         

             
           



