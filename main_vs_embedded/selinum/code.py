from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import Select
from selenium.webdriver.support.ui import WebDriverWait
from selenium.common.exceptions import TimeoutException
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import NoSuchElementException
from selenium.common.exceptions import NoAlertPresentException
#from selenium.webdriver.common.desired_capabilities import DesiredCapabilities
import sys
import unittest, time, re

class Sel(unittest.TestCase):
    def setUp(self):
        #self.driver = webdriver.Firefox(executable_path='/usr/local/bin/geckodriver')
        #cap = DesiredCapabilities().Firefox
        #cap["marionette"] = False
        #self.driver = webdriver.Firefox(capabilities=cap, executable_path="./geckodriver")
        #self.driver = webdriver.Firefox()
        self.driver = webdriver.Chrome()
        print("Here")

        self.driver.implicitly_wait(30)
        self.base_url = "http://imgur.ex.frade.emulab.net/"
        self.verificationErrors = []
        self.accept_next_alert = True
    def test_sel(self):
        driver = self.driver
        delay = 3
        driver.get(self.base_url)
        driver.find_element_by_link_text("All").click()
        for i in range(1,100):
            self.driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")
            time.sleep(4)
        html_source = driver.page_source
        data = html_source.encode('utf-8')
        print("Hi")

if __name__ == "__main__":
    unittest.main()
