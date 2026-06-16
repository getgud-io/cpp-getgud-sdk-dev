from getgud_cs2_parser import *
import os
import time
import signal
import shutil
import sys
import threading
import queue
import math
import random
from datetime import datetime

from dbscanner import DatabaseDemScanner

current_script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_script_path, "../"))
from getgudsdk_wrapper import GetgudSDK

GETGUD_TITLE_ID = os.environ.get("GETGUD_TITLE_ID")
GETGUD_PRIVATE_KEY = os.environ.get("GETGUD_PRIVATE_KEY")
SCAN_FOLDER_PATH = os.environ.get("SCAN_FOLDER_PATH")
SCRIPT_INVOKE_INTERVAL_MS = int(os.environ.get("SCRIPT_INVOKE_INTERVAL_MS"))
FILE_SCANNER_INVOKE_EVERY_MS = SCRIPT_INVOKE_INTERVAL_MS

# Throttle game submissions to mimic a real player-activity curve.
# Cosine-shaped rate peaks at PEAK_HOUR_UTC (evening), trough 12h earlier; ±30% jitter.
TARGET_GAMES_PER_DAY = 120  # ~5/hr average
PEAK_HOUR_UTC = 21
RATE_AMPLITUDE = 0.85  # 1+A at peak (~9/hr), 1-A at trough (~0.75/hr)


def compute_inter_game_sleep_seconds():
    avg_rate_per_hour = TARGET_GAMES_PER_DAY / 24.0
    hour = datetime.utcnow().hour + datetime.utcnow().minute / 60.0
    phase = (hour - PEAK_HOUR_UTC) / 24.0 * 2 * math.pi
    rate_multiplier = 1 + RATE_AMPLITUDE * math.cos(phase)
    games_per_hour = max(avg_rate_per_hour * rate_multiplier, 1.0)
    return (3600.0 / games_per_hour) * random.uniform(0.7, 1.3)



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

                    # Extract banned player ID from filename if present
                    banned_players = []
                    if '_banned_' in filename:
                        try:
                            banned_player_id = filename.split('_banned_')[1].replace('.dem', '')
                            banned_players.append(banned_player_id)
                            print(f'[Manager] Detected banned player ID: {banned_player_id}')
                        except Exception as e:
                            print(f'[Manager] Error extracting banned player ID from filename: {e}')

                    print(f'[Manager] Processing file: {filepath}')
                    success = False
                    try:
                        parser = GetgudCS2Parser(self.sdk, filepath, banned_players)
                        game_guid = parser.start()
                        print(f"[Manager] {game_guid} game sent to Getgud")
                        success = True
                    except BaseException as e:
                        print(f'[Manager] Error processing {filepath}: {e}')
                    
                    if success:
                        sleep_seconds = compute_inter_game_sleep_seconds()
                        print(f'[Manager] Sleeping for {sleep_seconds:.1f}s before next game (target {TARGET_GAMES_PER_DAY}/day, peak {PEAK_HOUR_UTC}:00 UTC).')
                        time.sleep(sleep_seconds)
                    
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
