#!/bin/bash
sudo apt-get update
sudo apt-get install apache2 php5 libapache2-mod-php5 mysql-server mysql-client php5-mysql phpmyadmin
###Note:  Please use sql password as "wiki_installation" or change in auto_install_part2.sh          #####
cd /var/www/html
cp /proj/FRADE/installation/wikipedia/resources/mediawiki-1.27.3.tar.gz ./
tar -xvzf mediawiki-1.27.3.tar.gz
mv mediawiki-1.27.3 mediawiki
chown -R root mediawiki
#mysql -u root -proot < /proj/FRADE/installation/wikipedia/resources/dump.sql
chmod 777 cache
#========================================================================================================#
#==Now go to the browser, type http://wikipedia.wiki-testing.frade.emulab.net/mediawiki/ ================#
#=========Follow the instructions =======================================================================#
#========= Put memcached parameters as "127.0.0.1:11211" ================================================#
#========At the end, download and put LocalSettings.php inside mediawiki folder    ======================#
#========================================================================================================#
cp /proj/FRADE/installation/wikipedia/resources/page.txt /var/lib/mysql-files/
cp /proj/FRADE/installation/wikipedia/resources/revision.txt /var/lib/mysql-files/
cp /proj/FRADE/installation/wikipedia/resources/text_table.txt /var/lib/mysql-files/
#mysql -u root -proot < /proj/FRADE/installation/wikipedia/sql/insert.sql
sudo apt-get update
sudo apt-get install memcached
MYSQL_PW="root"
cp /proj/FRADE/installation/wikipedia/resources/page.txt /var/lib/mysql-files/
cp /proj/FRADE/installation/wikipedia/resources/revision.txt /var/lib/mysql-files/
cp /proj/FRADE/installation/wikipedia/resources/text_table.txt /var/lib/mysql-files/
mysql -u root -p$MYSQL_PW < sql/insert.sql
sudo apt-get update
sudo apt-get install memcached
cp /proj/FRADE/installation/wikipedia/resources/Scribunto-REL1_28-a665621.tar.gz /var/www/html/mediawiki/extensions
cd /var/www/html/mediawiki/extensions
tar -xvzf Scribunto-REL1_28-a665621.tar.gz
cp /proj/FRADE/installation/wikipedia/resources/decp-wiki/dummy.html /var/www/html/
cp -r /proj/FRADE/installation/wikipedia/resources/decp-wiki/index4.php /var/www/html/mediawiki/
cp -r /proj/FRADE/installation/wikipedia/resources/decp-wiki/jpegs /var/www/html/mediawiki/
cd /var/www/html/mediawiki/index4.php
./makelinks.sh
cd /var/www/html/mediawiki/jpegs
./makelinks.sh
cp /proj/FRADE/installation/wikipedia/resources/Parser.php /var/www/html/mediawiki/includes/parser/
#===== Now wikipedia is ready, modify LocalSettings.php as the one in resources to make it work if it doesn't ==========#
#======================Nginx part ===========#
service apache2 stop
sudo apt-get update
sudo apt-get install nginx
sudo apt-get install php5-fpm
sudo service nginx start
cp /proj/FRADE/installation/wikipedia/resources/nginx_conf/nginx/nginx.conf /etc/nginx/
cp /proj/FRADE/installation/wikipedia/resources/nginx_conf/nginx/sites-available/default /etc/nginx/sites-available/
#cp /proj/FRADE/installation/wikipedia/resources/nginx_conf/nginx/sites-available/fpm/php.ini /etc/php5/fpm/
#cp /proj/FRADE/installation/wikipedia/resources/nginx_conf/nginx/sites-available/fpm/pool.d/www.conf /etc/php5/fpm/pool.d/
cp /proj/FRADE/nginx_conf/latest/php5/fpm/php.ini /etc/php5/fpm/
cp /proj/FRADE/nginx_conf/latest/php5/fpm/pool.d/www.conf /etc/php5/fpm/pool.d/
sudo service php5-fpm restart
sudo service nginx restart
