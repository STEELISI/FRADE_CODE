#!/bin/bash
sudo apt-get update
sudo apt-get install apache2
#=========================================================================#
#== Copy from deter: /mnt/zfs/Imgur-new-version/imgur-latest.tar.gz     ==#
#== Destination path : /var/www/html                                    ==#
cd /var/www/html
scp -r rajattan@steel.isi.edu:~/FRADE/imgur-latest.tar.gz ./
tar -xvzf imgur-latest.tar.gz
mv imgur-latest imgur
cp -r /proj/FRADE/installation/imgur/resources/apache2/ /etc/
#cp /proj/FRADE/installation/imgur/resources/apache2.conf /etc/apache2/
#cp /proj/FRADE/installation/imgur/resources/imgur.conf /etc/apache2/sites-available/
#cp /proj/FRADE/installation/imgur/resources/imgur.conf /etc/apache2/sites-enabled/
#sudo a2enmod rewrite && sudo service apache2 restart
wget https://dl-ssl.google.com/dl/linux/direct/mod-pagespeed-stable_current_amd64.deb
sudo dpkg -i mod-pagespeed-stable_current_amd64.deb
sudo apt-get install -f
sudo a2enmod rewrite && sudo service apache2 restart
#sudo systemctl restart apache2
#curl -D- localhost
#==   https://www.vultr.com/docs/install-mod-pagespeed-module-with-apache-on-ubuntu-16-04
