from selenium import webdriver
driver = webdriver.Chrome()
driver.get("http://imgur.ex.frade.emulab.net/")
driver.find_element_by_xpath("""//*[contains(@class,'scroll-top-text')]""").click()
#driver.find_element_by_xpath("""//*[contains(@script,'scrollTop')]""").click()
elements = driver.find_elements_by_tag_name("a")
for ele in elements:
    print(ele.get_attribute("href"))

driver.find_element_by_xpath("""//*[contains(@class,'scroll-top-text')]""").click()
elements = driver.find_elements_by_tag_name("a")
for ele in elements:
    print(ele.get_attribute("href"))


driver.find_element_by_xpath("""//*[contains(@class,'scroll-top-text')]""").click()
elements = driver.find_elements_by_tag_name("a")

for ele in elements:
    print(ele.get_attribute("href"))

driver.find_element_by_xpath("""//*[contains(@class,'scroll-top-text')]""").click()
elements = driver.find_elements_by_tag_name("a")
for ele in elements:
    print(ele.get_attribute("href"))
