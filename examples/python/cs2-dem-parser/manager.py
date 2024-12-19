from getgud_cs2_parser import *
import os
import time
import signal
import shutil
import sys
import threading
import queue

from dbscanner import DatabaseDemScanner

current_script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_script_path, "../"))
from getgudsdk_wrapper import GetgudSDK

GETGUD_TITLE_ID = os.environ.get("GETGUD_TITLE_ID")
GETGUD_PRIVATE_KEY = os.environ.get("GETGUD_PRIVATE_KEY")
SCAN_FOLDER_PATH = os.environ.get("SCAN_FOLDER_PATH")
SCRIPT_INVOKE_INTERVAL_MS = int(os.environ.get("SCRIPT_INVOKE_INTERVAL_MS"))
FILE_SCANNER_INVOKE_EVERY_MS = SCRIPT_INVOKE_INTERVAL_MS
SCANNER_SLEEP_TIME_BETWEEN_GAMES = 0



class GetgudParserManager:
    def __init__(self):
        self.sdk = GetgudSDK()
        self.is_manager_active = True

    def process_files(self):
        """
        Continuously scan for .dem files, process them one by one in FIFO order, and delete when done.
        Skips files that have a corresponding .bz2 file, indicating they're not ready for processing.
        """
        while self.is_manager_active:
            try:
                # Get all .dem files and sort them by creation time
                dem_files = [f for f in os.listdir(SCAN_FOLDER_PATH) if f.endswith(".dem")]
                dem_files.sort(key=lambda x: os.stat(os.path.join(SCAN_FOLDER_PATH, x)).st_ctime)

                for filename in dem_files:
                    filepath = os.path.join(SCAN_FOLDER_PATH, filename)
                    bz2_filepath = filepath + '.bz2'
                    
                    # Skip if corresponding .bz2 file exists
                    if os.path.exists(bz2_filepath):
                        print(f'[Manager] Skipping {filepath} as {bz2_filepath} still exists.')
                        continue

                    print(f'[Manager] Processing file: {filepath}')
                    success = False
                    try:
                        parser = GetgudCS2Parser(self.sdk, filepath, [])
                        game_guid = parser.start()
                        print(f"[Manager] {game_guid} game sent to Getgud")
                        success = True
                    except BaseException as e:
                        print(f'[Manager] Error processing {filepath}: {e}')
                    
                    if success:
                        print(f'[Manager] Sleeping for {SCANNER_SLEEP_TIME_BETWEEN_GAMES} seconds before processing the next game.')
                        time.sleep(SCANNER_SLEEP_TIME_BETWEEN_GAMES)
                    
                    # Delete the file after processing, regardless of success or failure
                    try:
                        os.remove(filepath)
                        print(f'[Manager] Deleted file {filepath} after processing.')
                    except Exception as e:
                        print(f'[Manager] Failed to delete file {filepath}: {e}')
                        
            except Exception as e:
                print(f"[Manager] Error scanning directory {SCAN_FOLDER_PATH}: {e}")
            
            time.sleep(SCRIPT_INVOKE_INTERVAL_MS / 1000)

    def start(self):
        self.process_files()
        self.sdk.dispose()

    def stop(self):
        self.is_manager_active = False


if __name__ == "__main__":
    getgud_sdk_manager = GetgudParserManager()
    
    # Initialize and start the DatabaseDemScanner
    # db_scanner = DatabaseDemScanner()
    # db_scanner.start()

    def signal_handler(sig, frame):
        print("Signal received:", sig)
        getgud_sdk_manager.stop()
        # db_scanner.stop()

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    getgud_sdk_manager.start()
