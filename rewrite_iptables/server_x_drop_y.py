import subprocess
import time

ip_limit=100
#call = subprocess.check_output("iptables -F", shell=True)
#call = subprocess.check_output("iptables -A INPUT -i eth4 -j DROP", shell=True)
#call = subprocess.check_output("iptables -I INPUT 1 -i eth4 -s 10.1.1.4 -j ACCEPT", shell=True)
while True:
    arr=set()
    blk=set()
    start_time = time.time()
    call = subprocess.check_output("iptables --line-numbers -L -n | grep 'ACCEPT' | grep '10.' | wc -l", shell=True)
    print(int(call))
    ips = subprocess.check_output("tcpdump -c 200 -i eth4 -nn -tt | awk '{print $3}' | awk -F '.' '{print $1\".\"$2\".\"$3\".\"$4}'", shell=True)
    #ips = subprocess.check_output("tcpdump -r check.pcap -nn -tt | tail -1000 | grep \"IP\" | awk '{print $3}' | awk -F '.' '{print $1\".\"$2\".\"$3\".\"$4}'", shell=True)
    st = ""
    for i in ips:        
        if(i == '\n'):
            if(not(st == "10.1.1.4" or st == "10.1.1.2")):
                arr.add(st)
                print(st)
            st = ""
        else:
            st = st + i
          

    #print(arr)
    already = ""
    try:
        already = subprocess.check_output("ipset list blacklist | grep '\.' ", shell=True)
    except subprocess.CalledProcessError as e:
        print("Ignore")
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
    rule_exists = ""
    try:
        rule_exists = subprocess.check_output("iptables --line-numbers -L -n | grep 'ACCEPT' | grep '10.' | awk '{print $5}'", shell=True)
    except subprocess.CalledProcessError as e:
        print("Ignore")
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
            num =""
            try:
                num = subprocess.check_output("iptables --line-numbers -L -n | grep " +ele + " | awk '{print $1}'", shell=True)
            except subprocess.CalledProcessError as e:
                print("Ignore")
            print(ele, num)
            if(not(num == '')):
                st = ""
                for row in num:
                    if(row == '\n'):
                        delt = ""
                        try:
                            delt = subprocess.check_output("iptables -D INPUT " + st , shell=True) 
                        except subprocess.CalledProcessError as e:
                            print("Ignore")
                        print(delt)
                        st = ""
                    else:
                        st = st + row
    call="2000"
    try:
        call = subprocess.check_output("iptables --line-numbers -L -n | grep 'ACCEPT' | grep '10.' | wc -l", shell=True) 
    except subprocess.CalledProcessError as e:
        print("Ignore")
    print(int(call))
    if(int(call) <= ip_limit):
        count = ip_limit - int(call)
        print("Count",count,len(arr))
        for i in arr:
            if(i not in common and count >=0 ):
                print("Inserting", i)
                try:
                    x = subprocess.check_output("iptables -I INPUT 1 -i eth4 -s "+ i + " -j ACCEPT", shell=True)
                except subprocess.CalledProcessError as e:
                    print("Ignore")
                print("Inserted")
            count = count - 1

    print("--- %s seconds ---" % (time.time() - start_time))


