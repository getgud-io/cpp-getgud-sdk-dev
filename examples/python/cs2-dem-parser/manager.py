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


class GetgudParserManager:
    def __init__(self):
        self.sdk = GetgudSDK()
        self.is_manager_active = True

    def process_files(self):
        """
        Continuously scan for .dem files, process them one by one, and delete when done.
        """
        while self.is_manager_active:
            try:
                for filename in os.listdir(SCAN_FOLDER_PATH):
                    if filename.endswith(".dem"):
                        filepath = os.path.join(SCAN_FOLDER_PATH, filename)
                        print(f'[Manager] Processing file: {filepath}')
                        try:
                            parser = GetgudCS2Parser(self.sdk, filepath, [])
                            game_guid = parser.start()
                            print(f"[Manager] {game_guid} game sent to Getgud")
                        except Exception as e:
                            print(f'[Manager] Error processing {filepath}: {e}')
                        finally:
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
