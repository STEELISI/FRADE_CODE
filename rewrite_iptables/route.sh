#!/usr/bin/env bash
#== Remove IP Tables ==#
#for i in $( iptables -t nat --line-numbers -L | grep ^[0-9] | awk '{ print $1 }' | tac ); do iptables -t nat -D PREROUTING $i; done
declare -a theArray
declare -a theblacklisted
ip_limit=200
while true
do
        START=$(date +%s)
        OUTPUT="$(iptables -t nat --line-numbers -L | grep DNAT | wc -l)"
        #echo "IP TABLE CONTAINS $OUTPUT IPs"
        #if [ "$OUTPUT" -lt "$ip_limit" ]; then
                cur_ips="$(awk '{print $1}' /proj/FRADE/access.log | sort | uniq | grep \\. | shuf -n 100 )"
                ips_blacklisted="$(ipset list blacklist | grep 10\\. )" 
                #done        
                while read -r line; do
                    theArray+=($line)
		    #echo "Current... $line "
		done <<< "$cur_ips"

                while read -r line; do
                    ips_blacklisted+=($line)
                    #echo "Blklistd... $line "
                done <<< "$ips_blacklisted"
                
                
		for line in "${theArray[@]}"
		do
		    #echo "$line"
                    flag=0
                    for ip in "${ips_blacklisted[@]}"
                    do
                        #echo "$line $ip over"
                    	if [ "$ip" = "$line" ]; then 
                                 num="$(iptables -t nat --line-numbers -L | grep '$line ' | awk '{print $1}')"
                                 iptables -t nat -D PREROUTING $num &
                                 #echo "Found and Deleted $ip"
   				 flag=1
			         break
		    	fi;
                    done
                    #echo "Done with inside for"
                    if [ "$flag" = "1" ]; then
                    	continue
                    
                    elif [ "$OUTPUT" -lt "$ip_limit" ]; then
                        iptables -t nat -I PREROUTING 1 -s $line -p tcp --dport 80 -j DNAT --to-destination 10.100.1.14:80 &
                        #echo "Added $line"
                    fi;
                

		done


        	#echo "Inserting $cur_ips"
        #fi
        END=$(date +%s)
DIFF=$(( $END - $START ))
echo "It took $DIFF seconds"
	sleep 1

done
