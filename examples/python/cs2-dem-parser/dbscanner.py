import os
import time
import threading
import mysql.connector
import requests
import shutil
import bz2

# Configuration constants
DB_HOST = os.environ.get("HOST")
DB_USER = os.environ.get("DB_USER")
DB_PASSWORD = os.environ.get("DB_PASSWORD")
DB_NAME = os.environ.get("DB")
DB_TABLE_NAME = os.environ.get("DB_TABLE_NAME")
SCAN_FOLDER_PATH = os.environ.get("SCAN_FOLDER_PATH")
LOOP_SLEEP_TIME_IN_SECONDS = 30  # Adjust as needed
SLEEP_SECONDS_BETWEEN_DOWNLOADS = 10  # Adjust as needed

class DatabaseDemScanner:
    def __init__(self):
        self.is_scanner_active = True
        self.mydb = mysql.connector.connect(
            host=DB_HOST,       # Your MySQL host
            user=DB_USER,       # Your MySQL user
            password=DB_PASSWORD,   # Your MySQL password
            database=DB_NAME    # Your MySQL database name
        )
        os.makedirs(SCAN_FOLDER_PATH, exist_ok=True)

    def get_data_to_process(self):
        mycursor = self.mydb.cursor()
        mycursor.execute(f'''
        SELECT id, resolvedUrl
        FROM `{DB_TABLE_NAME}` 
        WHERE 
            isProcessed IS NULL 
            AND resolvedUrl IS NOT NULL
        ''')
        data = mycursor.fetchall()
        mycursor.close()
        return data

    def update_is_processed(self, _id):
        mycursor = self.mydb.cursor()
        query = f"UPDATE `{DB_TABLE_NAME}` SET isProcessed = 1 WHERE id = %s"
        values = (_id,)
        mycursor.execute(query, values)
        self.mydb.commit()
        mycursor.close()

    def unzip_bz2_file(self, source_path, dest_path):
        """
        Unzips a .bz2 file.
        """
        try:
            with bz2.BZ2File(source_path, 'rb') as fr, open(dest_path, 'wb') as fw:
                shutil.copyfileobj(fr, fw)
            return True
        except Exception as e:
            print(f"[DBScanner] Failed to unzip {source_path}: {e}")
            return False

    def download_dem_file(self, url, local_filename):
        """
        Downloads the .dem file from the given URL, unzips if necessary, and saves it to SCAN_FOLDER_PATH.
        """
        temp_output_folder = SCAN_FOLDER_PATH  # You can adjust this if needed
        os.makedirs(temp_output_folder, exist_ok=True)

        local_filepath = os.path.join(temp_output_folder, local_filename)
        try:
            # Download the file
            with requests.get(url, stream=True) as r:
                r.raise_for_status()
                with open(local_filepath, 'wb') as f:
                    shutil.copyfileobj(r.raw, f)
            print(f"[DBScanner] Downloaded file {local_filepath}")

            # Check if the file is a .bz2 compressed file
            if local_filename.endswith('.bz2'):
                output_filename = '.'.join(local_filename.split('.')[:-1])  # Remove the '.bz2' extension
                output_filepath = os.path.join(temp_output_folder, output_filename)
                # Unzip the .bz2 file
                success = self.unzip_bz2_file(local_filepath, output_filepath)
                if success:
                    # Remove the original .bz2 file
                    os.remove(local_filepath)
                    print(f"[DBScanner] Unzipped file to {output_filepath}")
                    return output_filepath
                else:
                    return None
            else:
                # No need to unzip
                return local_filepath

        except Exception as e:
            print(f"[DBScanner] Failed to download {url}: {e}")
            return None

    def scanner_loop(self):
        while self.is_scanner_active:
            data = self.get_data_to_process()
            for row in data:
                _id, resolved_url = row
                print(f"[DBScanner] Processing entry ID {_id}")
                local_filename = f"{_id}_{resolved_url.split('/')[-1]}"
                dem_file_path = self.download_dem_file(resolved_url, local_filename)
                if dem_file_path:
                    print(f"[DBScanner] DEM file ready for ID {_id} at {dem_file_path}")
                    self.update_is_processed(_id)
                else:
                    print(f"[DBScanner] Failed to prepare DEM file for ID {_id}")
                if not self.is_scanner_active:
                    break
                time.sleep(SLEEP_SECONDS_BETWEEN_DOWNLOADS)
            if not self.is_scanner_active:
                break
            time.sleep(LOOP_SLEEP_TIME_IN_SECONDS)
        self.mydb.close()

    def start(self):
        self.scanner_thread = threading.Thread(target=self.scanner_loop)
        self.scanner_thread.start()

    def stop(self):
        self.is_scanner_active = False
        self.scanner_thread.join()