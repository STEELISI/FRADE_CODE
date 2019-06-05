from selenium import webdriver
from selenium.common.exceptions import StaleElementReferenceException
import time
driver = webdriver.Chrome()
#driver.get("https://imgur.com/")
driver.get("https://www.reddit.com/")
#driver.get("https://www.nytimes.com/")
#driver.find_element_by_xpath("""//*[contains(@class,'scroll-top-text')]""").click()
#driver.find_element_by_xpath("""//*[contains(@script,'scrollTop')]""").click()
count=10000
links = set()

for i in range(1,50000,10000):
    driver.execute_script("window.scrollTo(0," + str(count) + ")")
    #print("Here")
    elements = driver.find_elements_by_tag_name("img")
    #time.sleep(2)
    for ele in elements:
        try:
            links.add(ele.get_attribute("src"))
        except StaleElementReferenceException as Exception:
            continue
    count = count + 10000
for ele in links:
    print(ele)
