from subprocess import Popen, PIPE
import socket, struct
import time
import argparse
import re
import os
import logging
logging.basicConfig(filename='/mnt/detector.log',level=logging.DEBUG)

## the data dictionary keeps track of every user
# sample: { 167837965 : (list of starttimes for five windows, list of corresponding counts, list of already seen ports) }
# The key is long format of IP address, to make lookups efficient.

intDet = 1000
intDet_PSH = 1000
attackThresh = 4
stopThresh = 2
alpha = 0.9
serverip = ""
curSeqs = 0
curSeqs_PSH = 0
avgSeqs = 0
avgSeqs_PSH = 0
lastDet = 0
lastDet_PSH = 0
stop_tb_flag = 0 

def triggerFrade():
        if args.fradeArgs is not None:
                print("Starting FRADE ",args.fradeArgs[0])
                os.system("bash start_take_a_break.sh " + " &")
                os.system("bash start_frade.sh " + args.fradeArgs[0] + " &")                
        if args.dyn4Args is not None:
                print("Starting dyn4 ",args.dyn4Args[0])
                os.system("bash start_dyn4.sh " + args.dyn4Args[0] + " &")
                #os.system("bash stop_take_a_break.sh " + " &")

def detect(time1):
        global isAttack
        global lastDet
        global intDet
        global avgSeqs
        global curSeqs
        global stop_tb_flag

        curSeqs += 1
        logging.info("On no updation" + str(time1) +  "curSeqs " +  str(curSeqs) + " avgSeqs " + str(avgSeqs))
        if (lastDet == 0):
                lastDet = time1
        else:
           if (time1 - lastDet > intDet):
                lastDet = time1
                logging.info( str(time1) +  "curSeqs " +  str(curSeqs) + " avgSeqs " + str(avgSeqs))
                logging.info(" isAttack " + str(isAttack))
                if (curSeqs > attackThresh*avgSeqs and avgSeqs > 0):
                        if not isAttack:
                                logging.info("Attack detected 1 :" + str(time1))
                                triggerFrade()
                                stop_tb_flag = 0
                        isAttack = True
                else:
                        if isAttack and (curSeqs < stopThresh*avgSeqs and curSeqs > 0):
                                logging.info("Attack has stopped 1", time1)
                                if(stop_tb_flag == 0):
                                    os.system("bash stop_frade.sh &")
                                    os.system("bash stop_take_a_break.sh " + " &")
                                    stop_tb_flag = 1
                                isAttack = False
                if not isAttack:
                        if (avgSeqs == 0):
                                avgSeqs = curSeqs
                        else:
                                avgSeqs = alpha*avgSeqs + (1-alpha)*curSeqs
                        logging.info("Updated avgSeqs to " + str(avgSeqs) +  " alpha " +  str(alpha) + " curseqs " + str(curSeqs))
                curSeqs = 0

def detect_stop(time1):
        global isAttack
        global lastDet_PSH
        global intDet_PSH
        global avgSeqs_PSH
        global curSeqs_PSH
        global stop_tb_flag

        curSeqs_PSH += 1
        logging.info("On no updation" + str(time1) + "curSeqs_PSH" + str(curSeqs_PSH) + " avgSeqs_PSH " + str(avgSeqs_PSH))
        if (lastDet_PSH == 0):
                lastDet_PSH = time1
        else:
           if (time1 - lastDet_PSH > intDet_PSH):
                lastDet_PSH = time1
                logging.info(str(time1) + "curSeqs_PSH " + str(curSeqs_PSH) + " avgSeqs_PSH " + str(avgSeqs_PSH))
                logging.info(" isAttack PSH " + str(isAttack) + str(attackThresh*avgSeqs_PSH))
                if (curSeqs_PSH > attackThresh*avgSeqs_PSH and avgSeqs_PSH > 0 ):
                        if not isAttack:
                                logging.info("Attack detected 2 ",time1)
                                triggerFrade()
                                stop_tb_flag = 0
                        isAttack = True
                else:
                        logging.info("Checking for Blk over at PSH avgPSH * stopThresh" + str(attackThresh*avgSeqs_PSH))
                        if isAttack and (curSeqs_PSH < stopThresh*avgSeqs_PSH and curSeqs_PSH > 0):
                                logging.info("Blacklisting over 2"+ str(time1))
                                if(stop_tb_flag == 0):
                                    os.system("bash stop_frade.sh &")
                                    os.system("bash stop_take_a_break.sh " + " &")
                                    stop_tb_flag = 1
                                #isAttack = False
                if not isAttack:
                        if (avgSeqs_PSH == 0):
                                avgSeqs_PSH = curSeqs_PSH
                        else:
                                avgSeqs_PSH = alpha*avgSeqs_PSH + (1-alpha)*curSeqs_PSH
                        logging.info("Updated avgSeqs_PSH to " + str(avgSeqs_PSH) + " alpha " +  str(alpha) +  " curseqs_PSH " + str(curSeqs_PSH))
                curSeqs_PSH = 0

	

parser = argparse.ArgumentParser(description='Detector module for FRADE. Detect an attack when number of new connection requests spikes.')
parser.add_argument('--ip', '-i', action='store', dest='serverip', required=True, help='Server to monitor.')
parser.add_argument('--nic', '-n', action='store', dest='servereth', required=True, help='Network interface to monitor (e.g., eth4).')
parser.add_argument('--frade', '-f', nargs='+', action='store', dest='fradeArgs', required=False, help='Arguments needed to start FRADE.')
parser.add_argument('--dyn4', '-d', nargs='+', action='store', dest='dyn4Args', required=False, help='Arguments needed to start dyn4.')

args = parser.parse_args()

isAttack=False


if __name__ == "__main__":
	
# Arguments (again, being bad with globals)                                    

        serverip=args.serverip

        p = Popen(["tcpdump", "-i",args.servereth,"-n", "-tt" ,"port", "80", "-s", "68"], stdout=PIPE, bufsize=1, universal_newlines=True)
        #p = Popen(["tcpdump", "-r","/zfs/FRADE/log.flood.wikipedia.100.1.dyn4","-nn", "-tt" ,"port", "80", "-s", "68"], stdout=PIPE, bufsize=1, universal_newlines=True)
        with p.stdout:
                for line in iter(p.stdout.readline, b''):
                #Format: "1486008562.808828 IP 10.1.1.13.47661 > 10.1.1.3.80: Flags [S], seq 3937307324, win 29200, options [mss 1460,sackOK,TS[|tcp]>"
                        sl  = line.split(" ")

			#print sl
                        try:
                                timeinmilli,sipport,dipport = float(sl[0])*1000 , sl[2], sl[4]
                                dip = ".".join(dipport.split(".")[0:-1])
                                sport = int(sipport.split(".")[-1])
                                sip = ".".join(sipport.split(".")[0:-1])
                                flags = sl[6]
                                seq = sl[8]
                        except:
                                continue
                        if dip == serverip:
                                if (flags == "[S],"):
                                        detect(timeinmilli)
                                if (flags == "[P]," or flags == "[P.],"):
                                        detect_stop(timeinmilli)
				



        p.wait() # wait for the subprocess to exit

