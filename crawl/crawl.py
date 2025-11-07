import os
import time
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException

try:
    from dotenv import load_dotenv
    load_dotenv()
except Exception:
    pass

JFK_URL = "https://www.jfkairport.com" ### JFK Airport Website
SECURITY_WAIT_FORM_XPATH = '//*[@id="security-wait"]/form'

def create_driver(headless: bool = True) -> webdriver.Chrome:
    chrome_options = Options()
    if headless:
        chrome_options.add_argument("--headless=new")
    chrome_options.add_argument("--no-sandbox")
    chrome_options.add_argument("--disable-dev-shm-usage")
    
    # Force using the bundled ChromeDriver (v141) instead of PATH
    driver_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "chromedriver-mac-x64", "chromedriver")
    service = Service(executable_path=driver_path)
    return webdriver.Chrome(service=service, options=chrome_options)

def get_security_wait_time():

    driver = create_driver(headless=False)
    wait = WebDriverWait(driver, 10)

    try:
        driver.get(JFK_URL)

        form_element = wait.until(
            EC.presence_of_element_located((By.XPATH, SECURITY_WAIT_FORM_XPATH))
        )
        
        driver.execute_script("arguments[0].scrollIntoView({behavior: 'smooth', block: 'center'});", form_element)
        time.sleep(1)
        
        print("\n=== Security Wait Times (JFK) ===\n")

        try:
            tables = form_element.find_elements(By.TAG_NAME, "table")
            if tables:
                for idx, table in enumerate(tables, 1):
                    rows = table.find_elements(By.TAG_NAME, "tr")
                    table_data = []
                    tsa_pre_col_index = None
                    
                    for row in rows:
                        headers = row.find_elements(By.TAG_NAME, "th")
                        if headers:
                            header_texts = [header.text.strip() for header in headers]

                            for i, header_text in enumerate(header_texts):
                                if "TSA Pre" in header_text:
                                    tsa_pre_col_index = i
                                    break
                                
                            filtered_headers = []
                            for i, header_text in enumerate(header_texts):
                                if i != tsa_pre_col_index:
                                    filtered_headers.append(header_text)
                            if filtered_headers:
                                table_data.append(filtered_headers)
                            break
                    
                    for row in rows:
                        cells = row.find_elements(By.TAG_NAME, "td")
                        if cells:
                            cell_texts = [cell.text.strip() for cell in cells]
                            
                            filtered_cells = []
                            for i, cell_text in enumerate(cell_texts):
                                if i != tsa_pre_col_index:
                                    filtered_cells.append(cell_text)
                            if filtered_cells:
                                table_data.append(filtered_cells)
                    
                    if table_data:
                        
                        max_cols = max(len(row) for row in table_data) if table_data else 0
                        col_widths = [0] * max_cols
                        
                        for row in table_data:
                            for i, cell in enumerate(row):
                                if i < max_cols:
                                    col_widths[i] = max(col_widths[i], len(cell))
                        
                        for row in table_data:
                            formatted_row = []
                            for i in range(max_cols):
                                if i < len(row):
                                    formatted_row.append(row[i].ljust(col_widths[i]))
                                else:
                                    formatted_row.append("".ljust(col_widths[i]))
                            print(" | ".join(formatted_row) + " |")
                        
                print("\n" + "="*50 + "\n")
        except Exception as e:
            print(f"Error parsing table: {str(e)}\n")
        
        try:
            inputs = form_element.find_elements(By.TAG_NAME, "input")
            selects = form_element.find_elements(By.TAG_NAME, "select")
            if inputs or selects:
                print("Input element information:")
                for idx, inp in enumerate(inputs, 1):
                    inp_type = inp.get_attribute("type") or "text"
                    inp_value = inp.get_attribute("value") or ""
                    inp_name = inp.get_attribute("name") or ""
                    print(f"Input {idx}: type={inp_type}, name={inp_name}, value={inp_value}")
                for idx, sel in enumerate(selects, 1):
                    sel_name = sel.get_attribute("name") or ""
                    selected_option = sel.find_elements(By.CSS_SELECTOR, "option[selected]")
                    if selected_option:
                        sel_value = selected_option[0].text.strip()
                    else:
                        sel_value = "No selection"
                    print(f"Select {idx}: name={sel_name}, selected={sel_value}")
                print("\n" + "="*50 + "\n")
        except Exception as e:
            print(f"Error parsing input elements: {str(e)}\n")

        print("\nKeep the browser open.")
        print("Press Ctrl+C to exit...")
        
        try:
            while True:
                input()
                
        except KeyboardInterrupt:
            print("\nUser requested exit.")
            
    except TimeoutException:
        print(f"Could not find the security wait time element. XPath: {SECURITY_WAIT_FORM_XPATH}")
    except Exception as e:
        print(f"Error: {str(e)}")
    finally:
        if 'driver' in locals():
            driver.quit()

if __name__ == "__main__":
    try:
        get_security_wait_time()
        
    except KeyboardInterrupt:
        print("\nProgram terminated.")