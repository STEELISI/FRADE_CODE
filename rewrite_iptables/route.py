import subprocess
import time

ip_limit=200
while True:
    arr=set()
    blk=set()
    start_time = time.time()
    call = subprocess.check_output("iptables -t nat --line-numbers -L | grep DNAT | wc -l", shell=True)
    print(int(call))
    ips = subprocess.check_output("awk '{print $1}' /proj/FRADE/access.log | grep '\.' | shuf -n 200", shell=True)
    st = ""
    for i in ips:        
        if(i == '\n'):
            arr.add(st)
            st = ""
        else:
            st = st + i
          

    #print(arr)
    already = subprocess.check_output("ipset list blacklist | grep '\.' ", shell=True)
    st = ""
    for i in already:        
        if(i == '\n'):
            blk.add(st)
            st = ""
        else:
            st = st + i

    #print(blk)
    common = set()
    common  = arr.intersection(blk)
    #iptables -t nat --line-numbers -L -n | awk '{print $5}' | grep '10\.2\.'

    rules = set()
    rule_exists = subprocess.check_output("iptables -t nat --line-numbers -L -n | awk '{print $5}' | grep '10\.'", shell=True)
    st = ""
    for i in rule_exists:
        if(i == '\n'):
            rules.add(st)
            st = ""
        else:
            st = st + i



    print(len(common))
    print("--- %s seconds ---" % (time.time() - start_time))
    for ele in common:
        if(ele in rules):
            num = subprocess.check_output("iptables -t nat --line-numbers -L | grep " +ele + " | awk '{print $1}'", shell=True)
            print(ele, num)
            if(not(num == '')):
                st = ""
                for row in num:
                    if(row == '\n'):
                        delt = subprocess.check_output("iptables -t nat -D PREROUTING " + st , shell=True) 
                        print(delt)
                        st = ""
                    else:
                        st = st + row
    call = subprocess.check_output("iptables -t nat --line-numbers -L | grep DNAT | wc -l", shell=True) 
    if(int(call) <= 200):
        count = 200 - int(call)
        print("Count",count,len(arr))
        for i in arr:
            if(i not in common and count <=0 ):
                print("Inserting", i)
                x = subprocess.check_output("iptables -t nat -I PREROUTING 1 -s "+ i + " -p tcp --dport 80 -j DNAT --to-destination 10.100.1.14:80", shell=True)
            count = count - 1

    print("--- %s seconds ---" % (time.time() - start_time))


