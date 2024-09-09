from getgud_cs2_parser import *
import os
import time
import signal
import shutil
import sys
import threading
import queue

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
            )  # Current time minus X minutes
            try:
                for filename in os.listdir(SCAN_FOLDER_PATH):
                    filepath = os.path.join(SCAN_FOLDER_PATH, filename)
                    if (
                        filepath.endswith(".dem")
                        and os.stat(filepath).st_ctime > file_min_add_time
                    ):
                        print(f'[File Scanner] Pushed new file to the queue: {filepath}')
                        self.file_queue.put(filepath)
            except Exception as e:
                print(f"[File Scanner] Failed to scan directory {SCAN_FOLDER_PATH}: {e}")
            time.sleep(FILE_SCANNER_INVOKE_EVERY_MS / 1000)

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
            except queue.Empty:
                continue  # No file was found in the queue, continue the loop

        scanner_thread.join()
        self.sdk.dispose()

    def stop(self):
        self.is_manager_active = False


if __name__ == "__main__":
    getgud_sdk_manager = GetgudParserManager()

    def signal_handler(sig, frame):
        print("Signal received:", sig)
        getgud_sdk_manager.stop()

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    getgud_sdk_manager.start()