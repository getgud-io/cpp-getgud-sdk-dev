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
        self.file_queue = queue.Queue()
        self.is_manager_active = True

    def file_scanner(self):
        """
        Continuously scan for new .dem files and add them to the queue.
        """
        while self.is_manager_active:
            file_min_add_time = (
                time.time() - FILE_SCANNER_INVOKE_EVERY_MS / 1000
            )  # Current time minus X seconds
            try:
                for filename in os.listdir(SCAN_FOLDER_PATH):
                    filepath = os.path.join(SCAN_FOLDER_PATH, filename)
                    if (
                        filename.endswith(".dem")
                        and os.path.isfile(filepath)
                        and os.stat(filepath).st_ctime > file_min_add_time
                    ):
                        # Check if corresponding .dem.bz2 file exists
                        dem_bz2_filename = filename + '.bz2'
                        dem_bz2_filepath = os.path.join(SCAN_FOLDER_PATH, dem_bz2_filename)
                        if os.path.exists(dem_bz2_filepath):
                            # If both .dem and .dem.bz2 files exist, skip processing
                            print(f"[File Scanner] Skipping {filepath} because {dem_bz2_filepath} exists.")
                            continue
                        # Check if file is still being written to (optional)
                        if self.is_file_in_use(filepath):
                            print(f"[File Scanner] Skipping {filepath} because it's still being written to.")
                            continue
                        print(f'[File Scanner] Pushed new file to the queue: {filepath}')
                        self.file_queue.put(filepath)
            except Exception as e:
                print(f"[File Scanner] Failed to scan directory {SCAN_FOLDER_PATH}: {e}")
            time.sleep(FILE_SCANNER_INVOKE_EVERY_MS / 1000)

    def is_file_in_use(self, filepath):
        """
        Checks if a file is currently being written to.
        """
        try:
            with open(filepath, 'rb'):
                return False
        except IOError:
            return True

    def start(self):
        scanner_thread = threading.Thread(target=self.file_scanner)
        scanner_thread.start()
        while self.is_manager_active:
            try:
                # Wait for a new file path from the queue
                dem_file_path = self.file_queue.get(
                    timeout=SCRIPT_INVOKE_INTERVAL_MS / 1000
                )
                print(f'[Manager] Took file {dem_file_path}')
                game_guid = None
                try:
                    parser = GetgudCS2Parser(self.sdk, dem_file_path, [])
                    game_guid = parser.start()
                    print(f"[Manager] {game_guid} game sent to Getgud")
                except Exception as e:
                    print(f'[Manager] Dem file is corrupted for game {dem_file_path}, skipping')
                finally:
                    # Delete the file after processing, regardless of success or failure
                    try:
                        os.remove(dem_file_path)
                        print(f'[Manager] Deleted file {dem_file_path} after processing.')
                    except Exception as e:
                        print(f'[Manager] Failed to delete file {dem_file_path}: {e}')
            except queue.Empty:
                continue  # No file was found in the queue, continue the loop

        scanner_thread.join()
        self.sdk.dispose()

    def stop(self):
        self.is_manager_active = False


if __name__ == "__main__":
    getgud_sdk_manager = GetgudParserManager()
    
    # Initialize and start the DatabaseDemScanner
    db_scanner = DatabaseDemScanner()
    db_scanner.start()

    def signal_handler(sig, frame):
        print("Signal received:", sig)
        getgud_sdk_manager.stop()
        db_scanner.stop()

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    getgud_sdk_manager.start()