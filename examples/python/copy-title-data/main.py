import os
import requests
from utils import Utils
import sys
import logging
import time
import pandas as pd
import argparse
from title_config import get_all_title_pairs, get_title_pair

current_script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_script_path, "../"))
from getgudsdk_wrapper import GetgudSDK

# Configure logging
import os
from datetime import datetime

def read_matches_from_csv(csv_file_path):
    """
    Read and parse a CSV file containing match data to copy using pandas.
    
    Expected CSV format:
    title_id,game_id,match_id
    133,game_123,match_456
    ...
    
    Args:
        csv_file_path (str): Path to the CSV file
        
    Returns:
        dict: Dictionary of matches organized by title_id and game_id
              Format: {title_id: {game_id: [match_id1, match_id2, ...]}}
    """
    logger.info(f"Reading matches from CSV file: {csv_file_path}")
    matches_by_title = {}
    
    try:
        # Read CSV using pandas
        df = pd.read_csv(csv_file_path, skipinitialspace=True)
        
        # Check required columns
        required_columns = ['title_id', 'game_id', 'match_id']
        
        # Normalize column names (handle case sensitivity and whitespace)
        df.columns = [col.strip().lower() for col in df.columns]
        
        # Check if all required columns exist
        missing_columns = [col for col in required_columns if col not in df.columns]
        if missing_columns:
            logger.error(f"Missing required columns in CSV: {', '.join(missing_columns)}")
            return {}
        
        # Remove any rows with NaN or empty values in required columns
        df = df.dropna(subset=required_columns)
        
        # Convert all column values to strings and strip whitespace
        for col in required_columns:
            df[col] = df[col].astype(str).str.strip()
            
        # Filter out any rows with empty values
        df = df[df[required_columns].apply(lambda row: all(row.values), axis=1)]
        
        # Group by title_id and game_id
        for title_id, title_group in df.groupby('title_id'):
            matches_by_title[title_id] = {}
            
            for game_id, game_group in title_group.groupby('game_id'):
                matches_by_title[title_id][game_id] = game_group['match_id'].tolist()
        
        # Log statistics
        row_count = len(df)
        logger.info(f"Successfully read {row_count} matches from CSV file")
        
        # Log summary of matches by title
        for title_id, games in matches_by_title.items():
            game_count = len(games)
            match_count = sum(len(matches) for matches in games.values())
            logger.info(f"Title {title_id}: {game_count} games, {match_count} matches")
                
    except Exception as e:
        logger.error(f"Error reading CSV file: {str(e)}")
        import traceback
        logger.error(traceback.format_exc())
        return {}
        
    return matches_by_title

# Create logs directory if it doesn't exist
log_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "logs")
os.makedirs(log_dir, exist_ok=True)

# Set up file and console logging
log_filename = os.path.join(log_dir, f"copy_title_data_{datetime.now().strftime('%Y%m%d_%H%M%S')}.log")

# Configure root logger with file and console handlers
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

# File handler with detailed logging
file_handler = logging.FileHandler(log_filename)
file_handler.setLevel(logging.DEBUG)
file_handler.setFormatter(logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s'))

# Console handler with more concise output
console_handler = logging.StreamHandler()
console_handler.setLevel(logging.INFO)
console_handler.setFormatter(logging.Formatter('%(asctime)s - %(levelname)s - %(message)s'))

# Add both handlers to logger
logger.addHandler(file_handler)
logger.addHandler(console_handler)

logger.info(f"Logging configured. Log file: {log_filename}")

# Will get title configuration from command line arguments
access_token = None
id_token = None
target_title_id = None
destination_title_id = None
destination_private_key = None
headers = {}

AFFECT_STATE_MAPPING = {
    0: GetgudSDK.AffectState.Attach,
    1: GetgudSDK.AffectState.Activate,
    2: GetgudSDK.AffectState.Deactivate,
    3: GetgudSDK.AffectState.Detach 
}

# Fetch matches for the specific title
def fetch_matches(specific_matches=None):
    """
    Fetch matches for the specific title.
    
    Args:
        specific_matches (dict, optional): Dictionary of specific matches to fetch, organized by game_id.
                                           Format: {game_id: [match_id1, match_id2, ...]}
                                           If None, will fetch all matches in the time window.
    
    Returns:
        dict: Match data response from the API
    """
    logger.info(f"Fetching matches for title ID: {target_title_id}")
    url = "https://api.getgud.io/api/match_data/find_matches"
    
    # Set search window to 365 days (365 * 24 * 60 * 60 * 1000 ms)
    search_epoch = 365 * 24 * 60 * 60 * 1000  # 365 days in milliseconds
    logger.info(f"Searching for matches in the past 365 days ({search_epoch} ms)")
    
    body = {
        "titleIds": [target_title_id],
        "searchLastEpoch": search_epoch,
        "results": {
            "count": True,
            "rows": {"isDESC": True},
            "timeBins": {"timeBinDurationEpoch": 3600000},
            "activityTimeBins": {"timeBinDurationEpoch": 3600000},
            "segmentBy": {"matchMode": True},
            "orderBy": {"matchStartTimeEpoch": True}
        }
    }
    
    # If specific matches are provided, add matchCriteria with match IDs
    if specific_matches:
        # Collect all match IDs from the specific_matches dictionary
        all_match_ids = []
        for game_id, match_ids in specific_matches.items():
            all_match_ids.extend(match_ids)
        
        # Remove any duplicates
        all_match_ids = list(set(all_match_ids))
        
        logger.info(f"Filtering for {len(all_match_ids)} specific match IDs")
        
        # Add matchCriteria with match IDs to the query
        body["matchCriteria"] = {
            "matchIds": all_match_ids
        }
    
    try:
        logger.debug(f"API request body: {body}")
        response = requests.post(url, headers=headers, json=body)
        response.raise_for_status()
        response_data = response.json()
        
        # Count matches for logging
        match_count = 0
        game_count = 0
        if response_data and "titleDic" in response_data and target_title_id in response_data["titleDic"]:
            title_data = response_data["titleDic"][target_title_id]
            if "gameDic" in title_data:
                game_count = len(title_data["gameDic"])
                for game_id, game_data in title_data["gameDic"].items():
                    if "matchDic" in game_data:
                        match_count += len(game_data["matchDic"])
        
        logger.info(f"Successfully fetched {match_count} matches across {game_count} games")
        return response_data
    except requests.RequestException as e:
        logger.error(f"Error fetching matches: {e}")
        logger.debug(f"Request details: URL={url}, Headers={headers}")
        return None

# Fetch action stream for a match
def fetch_action_stream(title_id, game_id, match_id):
    logger.info(f"Fetching action stream for title_id: {title_id}, game_id: {game_id}, match_id: {match_id}")
    url = f"https://api.getgud.io/api/match_data/get_action_data/{title_id}/{game_id}/{match_id}"
    try:
        start_time = time.time()
        response = requests.get(url, headers=headers)
        response.raise_for_status()
        response_time = time.time() - start_time
        logger.debug(f"API response time: {response_time:.2f} seconds")
        
        data = response.json().get("matchActionData", {})
        if data:
            action_stream_size = sum(len(str(item)) for item in data.values())
            logger.info(f"Successfully fetched action stream. Size: {action_stream_size/1024:.2f} KB")
        else:
            logger.warning("Action stream data is empty")
        return data
    except requests.RequestException as e:
        logger.error(f"Error fetching action stream: {e}")
        logger.debug(f"Request details: URL={url}, Headers={headers}")
        return None

# Process the action stream (inflate and parse)
def process_action_stream(encoded_action_stream):
    logger.info("Processing action stream")
    try:
        start_time = time.time()
        action_stream = Utils.inflate_string(encoded_action_stream)
        inflate_time = time.time() - start_time
        logger.debug(f"Inflation time: {inflate_time:.2f} seconds")
        
        parse_start_time = time.time()
        parsed_data = Utils.new_parse_action_stream(action_stream)
        parse_time = time.time() - parse_start_time
        
        # Log parse statistics
        if parsed_data:
            player_count = len(parsed_data.get("player_dict", {}))
            action_count = len(parsed_data.get("full_action_stream", []))
            logger.info(f"Action stream processed successfully in {parse_time:.2f} seconds")
            logger.info(f"Stream contains {action_count} actions for {player_count} players")
        else:
            logger.warning("Parsed data is empty")
        
        return parsed_data
    except Exception as e:
        logger.error(f"Error processing action stream: {str(e)}")
        raise

# Extract and process chat messages from match data
def extract_chat_messages(match_data, parsed_action_stream_data):
    """
    Extract chat messages from the match data.
    
    Args:
        match_data: The raw match data containing potential chat messages
        parsed_action_stream_data: The parsed action stream data containing player mappings
        
    Returns:
        List[Dict[str, Any]]: List of chat message objects with player GUIDs and messages
    """
    try:
        # Get match action data dictionary
        match_action_data = match_data if isinstance(match_data, dict) else {}
        
        # Find the chat key in the match action data
        chat_keys = [key for key in match_action_data.keys() if "chat" in key.lower()]
        if not chat_keys:
            return []
            
        # Get and decompress the chat data
        chat_key = chat_keys[0]
        encoded_chat = match_action_data[chat_key]
        
        # Check if encoded_chat is empty or null
        if not encoded_chat:
            logger.info("Chat data is empty")
            return []
            
        # Decode/decompress chat data
        chat_data = Utils.inflate_json(encoded_chat)
        
        # Get the player map for GUID translation
        player_map = parsed_action_stream_data.get("player_map", {})
        
        # Process into format for analysis
        formatted_messages = []
        for msg in chat_data:
            # Get player GUID from the message
            player_id = msg.get("playerGuid", "")
            # Map the player ID to the actual GUID if there's a mapping
            player_guid = player_map.get(player_id, player_id) if player_id else ""
            
            if not player_guid:
                continue  # Skip messages from unidentified players
                
            formatted_messages.append({
                "player_guid": player_guid,
                "message": msg.get("message", ""),
                "timestamp": msg.get("messageTimeEpoch", 0)
            })
            
        logger.info(f"Extracted {len(formatted_messages)} chat messages")
        return formatted_messages
        
    except Exception as e:
        logger.error(f"Error extracting chat messages: {str(e)}")
        import traceback
        logger.error(traceback.format_exc())
        return []

# Send action stream to a different title via SDK
def send_action_stream_via_sdk(sdk, game_guid, match_guid, action_data):
    logger.info(f"Sending action stream via SDK for game: {game_guid}, match: {match_guid}")
    for player, actions in action_data.items():
        for action in actions.get("match", {}).get("actionStream", []):
            sdk.send_action_stream(game_guid, match_guid, action)
    logger.info("Finished sending action stream via SDK")

# Main function to orchestrate fetching, processing, and streaming
def stream_matches_to_different_title(title_pair_key, csv_file_path=None):
    """
    Stream matches from one title to another based on configuration.
    
    Args:
        title_pair_key (str): The key for the title pair configuration to use
        csv_file_path (str, optional): Path to CSV file containing matches to copy.
                                      If provided, only these matches will be copied.
    """
    global access_token, id_token, target_title_id, destination_title_id, destination_private_key, headers
    
    # Get the title configuration based on the key
    try:
        title_config = get_title_pair(title_pair_key)
        logger.info(f"Using title pair: {title_config['name']}")
        
        # Set up the configuration from the selected title pair
        access_token = title_config['access_token_id']
        id_token = title_config['id_token']
        target_title_id = title_config['target_title_id']
        destination_title_id = title_config['destination_title_id']
        destination_private_key = title_config['destination_private_key']
        
        # Headers for API requests
        headers = {
            "X-Auth-Access-Token": access_token,
            "X-Auth-Id-Token": id_token,
        }
    except ValueError as e:
        logger.error(f"Error selecting title configuration: {str(e)}")
        return
    
    logger.info("=====================================================")
    logger.info(f"Starting to stream matches from title {target_title_id} to title {destination_title_id}")
    if csv_file_path:
        logger.info(f"Using CSV file: {csv_file_path} to select specific matches")
    else:
        logger.info("No CSV file provided - will fetch all matches in time window")
    logger.info("=====================================================")
    
    # Validate environment variables
    if not all([access_token, id_token, target_title_id, destination_title_id, destination_private_key]):
        logger.error("Missing required environment variables. Please check your .env file.")
        logger.error("Required: X_AUTH_ACCESS_TOKEN, X_AUTH_ID_TOKEN, GETGUD_TARGET_TITLE_ID, GETGUD_DESTINATION_TITLE_ID, GETGUD_DESTINATION_PRIVATE_KEY")
        return
    
    # Process CSV file if provided
    specific_matches = None
    if csv_file_path:
        all_matches_by_title = read_matches_from_csv(csv_file_path)
        
        # Get only the matches for our target title
        if target_title_id in all_matches_by_title:
            specific_matches = all_matches_by_title[target_title_id]
            logger.info(f"Found {sum(len(matches) for matches in specific_matches.values())} matches for title {target_title_id} in CSV file")
        else:
            logger.warning(f"No matches found for title {target_title_id} in CSV file")
            if not all_matches_by_title:
                logger.error("CSV file was empty or could not be parsed")
                return
            elif len(all_matches_by_title) > 0:
                other_titles = ', '.join(all_matches_by_title.keys())
                logger.warning(f"CSV contains matches for other titles: {other_titles}")
                return
    
    try:
        sdk = GetgudSDK()
        logger.info("SDK initialized successfully")
        
        # Fetch match data, filtered by specific matches if provided
        start_time = time.time()
        matches_data = fetch_matches(specific_matches)
        if not matches_data:
            logger.warning("No matches data available.")
            return

        # Calculate total games and matches for progress tracking
        total_games = len(matches_data.get("titleDic", {}).get(target_title_id, {}).get("gameDic", {}))
        total_matches = 0
        for game_id, game_data in matches_data.get("titleDic", {}).get(target_title_id, {}).get("gameDic", {}).items():
            total_matches += len(game_data.get("matchDic", {}))
        
        logger.info(f"Found {total_games} games with a total of {total_matches} matches to process")
        processed_games = 0
        processed_matches = 0
        successful_matches = 0
        failed_matches = 0
        
        # Get the title dic for easier access
        title_dic = matches_data.get("titleDic", {}).get(target_title_id, {})
        
        # Iterate through all games in the response
        for game_id, game_data in title_dic.get("gameDic", {}).items():
            processed_games += 1
            game_start_time = time.time()
            logger.info(f"Processing game {processed_games}/{total_games}: {game_id}")
            
            # Extract game-level info needed for all matches
            server_guid = game_data.get('serverGuid', f"server-{game_id}")
            game_mode = game_data.get('gameMode', "unknown")
            server_location = "UK"  # Default server location
            
            # Group matches by their start time
            match_dict = game_data.get("matchDic", {})
            if not match_dict:
                logger.warning(f"No matches found for game {game_id}")
                continue
            
            # If specific matches are provided, filter the match dictionary
            # to only include the matches specified in the CSV for this game
            if specific_matches and game_id in specific_matches:
                csv_match_ids = set(specific_matches[game_id])
                filtered_match_dict = {match_id: match_data for match_id, match_data in match_dict.items() 
                                     if match_id in csv_match_ids}
                
                # Log matches that were found vs. requested
                found_match_ids = set(filtered_match_dict.keys())
                missing_match_ids = csv_match_ids - found_match_ids
                
                if missing_match_ids:
                    logger.warning(f"Could not find {len(missing_match_ids)} requested matches for game {game_id}: {', '.join(missing_match_ids)}")
                
                match_dict = filtered_match_dict
                
                if not match_dict:
                    logger.warning(f"No matches from CSV found for game {game_id}")
                    continue
                    
            # Sort matches by start time to maintain chronology
            sorted_matches = []
            for match_id, match_data in match_dict.items():
                match_start_time_epoch = match_data.get('matchStartTimeEpoch', 0)
                sorted_matches.append((match_id, match_data, match_start_time_epoch))
            
            # Sort matches by their start time
            sorted_matches.sort(key=lambda x: x[2])
            
            # Start a single game session for all matches in this game
            try:
                logger.info(f"Starting game for game_id: {game_id} with {len(sorted_matches)} matches")
                game_guid = sdk.start_game(
                    title_id=int(destination_title_id),
                    private_key=destination_private_key,
                    server_guid=server_guid,
                    game_mode=game_mode,
                    server_location=server_location
                )
                logger.info(f"Game started with game_guid: {game_guid}")
                
                # Process all matches within this game
                for match_id, match_data, match_start_time_epoch in sorted_matches:
                    processed_matches += 1
                    match_process_start = time.time()
                    logger.info(f"Processing match {processed_matches}/{total_matches}: {match_id} (Game: {game_id})")
                    
                    try:
                        # Fetch action stream for the match
                        zipped_action_stream_data = fetch_action_stream(target_title_id, game_id, match_id)
                        if zipped_action_stream_data is None:
                            logger.warning(f"No action stream data for match: {match_id}")
                            failed_matches += 1
                            continue
                        
                        # Process the action stream
                        zipped_stream_key = list(zipped_action_stream_data.keys())[0]
                        logger.debug(f"Processing zipped stream with key: {zipped_stream_key}")
                        unzipped_action_stream_data = process_action_stream(zipped_action_stream_data[zipped_stream_key])
                        
                        # Start the match (game was already started above)
                        match_mode = match_data.get('matchMode', "unknown")
                        map_name = match_data.get('mapName', "unknown")
                        
                        logger.info(f"Starting match {match_id} with mode {match_mode} on map {map_name}")
                        match_guid = sdk.start_match(
                            game_guid=game_guid,
                            match_mode=match_mode,
                            map_name=map_name,
                            custom_field=match_id  # Store original match ID in custom field
                        )
                        logger.info(f"Match started with match_guid: {match_guid}, original match_id in custom field: {match_id}")
                        
                        # Extract and send chat messages
                        chat_messages = extract_chat_messages(zipped_action_stream_data, unzipped_action_stream_data)
                        if chat_messages:
                            logger.info(f"Processing {len(chat_messages)} chat messages for match {match_id}")
                            for chat_msg in chat_messages:
                                try:
                                    sdk.send_chat_message(
                                        match_guid=match_guid,
                                        message_time_epoch=chat_msg["timestamp"],
                                        player_guid=chat_msg["player_guid"],
                                        message=chat_msg["message"]
                                    )
                                except Exception as e:
                                    logger.warning(f"Failed to send chat message: {str(e)}")
                        
                        # Process all actions for this match
                        action_counts = {
                            'S': 0,  # Spawn
                            'A': 0,  # Attack
                            'D': 0,  # Damage
                            'DD': 0, # Death
                            'P': 0,  # Position
                            'H': 0,  # Heal
                            'AF': 0  # Affect
                        }
                        
                        total_actions = len(unzipped_action_stream_data['full_action_stream'])
                        logger.info(f"Beginning to process {total_actions} actions for match {match_id}")
                        progress_interval = max(1, total_actions // 10)  # Log progress at 10% intervals
                        
                        # Extract the player and weapon mappings to use the actual names instead of IDs
                        player_map = unzipped_action_stream_data.get('player_map', {})
                        weapon_map = unzipped_action_stream_data.get('weapon_map', {})
                        
                        logger.info(f"Using mappings - Players: {len(player_map)}, Weapons: {len(weapon_map)}")
                        
                        for idx, action in enumerate(unzipped_action_stream_data['full_action_stream']):
                            action_time_epoch = action[0]
                            action_type = action[1]
                            
                            # Look up the actual player GUID from the mapping
                            player_id = action[2]
                            player_guid = player_map.get(player_id, player_id)
                            
                            if action_type == 'S':  # Spawn Action
                                position = [float(p) for p in action[6].split('~')[:3]]
                                rotation = [float(r) for r in action[6].split('~')[3:]]
                                
                                # Look up actual character GUID and team GUID from the mappings if needed
                                character_id = action[3]
                                character_guid = character_id  # Character IDs usually don't have a mapping

                                team_id = action[4]
                                team_guid = team_id  # Team IDs usually don't have a mapping
                                
                                sdk.send_spawn_action(
                                    match_guid=match_guid,
                                    action_time_epoch=action_time_epoch,
                                    player_guid=player_guid,
                                    character_guid=character_guid,
                                    team_guid=team_guid,
                                    initial_health=float(action[5]),
                                    position=position,
                                    rotation=rotation
                                )
                            
                            elif action_type == 'A':  # Attack Action
                                # Look up the actual weapon GUID from the mapping
                                weapon_id = action[3]
                                weapon_guid = weapon_map.get(weapon_id, weapon_id)
                                
                                sdk.send_attack_action(
                                    match_guid=match_guid,
                                    action_time_epoch=action_time_epoch,
                                    player_guid=player_guid,
                                    weapon_guid=weapon_guid
                                )
                            
                            elif action_type == 'D':  # Damage Action
                                # Look up the actual victim player GUID and weapon GUID from the mappings
                                victim_id = action[3]
                                victim_player_guid = player_map.get(victim_id, victim_id)
                                
                                weapon_id = action[5]
                                weapon_guid = weapon_map.get(weapon_id, weapon_id)
                                
                                sdk.send_damage_action(
                                    match_guid=match_guid,
                                    action_time_epoch=action_time_epoch,
                                    player_guid=player_guid,
                                    victim_player_guid=victim_player_guid,
                                    damage_done=float(action[4]),
                                    weapon_guid=weapon_guid
                                )
                            
                            elif action_type == 'DD':  # Death Action
                                # Look up the actual attacker GUID from the mapping
                                attacker_id = action[3]
                                attacker_guid = player_map.get(attacker_id, attacker_id)
                                
                                sdk.send_death_action(
                                    match_guid=match_guid,
                                    action_time_epoch=action_time_epoch,
                                    player_guid=player_guid,
                                    attacker_guid=attacker_guid
                                )
                            
                            elif action_type == 'P':  # Position Action
                                position = [float(p) for p in action[3].split('~')[:3]]
                                rotation = [float(r) for r in action[3].split('~')[3:]]
                                
                                sdk.send_position_action(
                                    match_guid=match_guid,
                                    action_time_epoch=action_time_epoch,
                                    player_guid=player_guid,
                                    position=position,
                                    rotation=rotation
                                )
                            
                            elif action_type == 'H':  # Heal Action
                                sdk.send_heal_action(
                                    match_guid=match_guid,
                                    action_time_epoch=action_time_epoch,
                                    player_guid=player_guid,
                                    health_gained=float(action[3])
                                )
                            
                            elif action_type == 'AF':  # Affect Action
                                affect_state = AFFECT_STATE_MAPPING[int(action[4])]
                                
                                # Look up the actual affect GUID if there's a mapping for it
                                affect_id = action[3]
                                affect_guid = affect_id  # Affect IDs usually don't have a mapping
                                
                                sdk.send_affect_action(
                                    match_guid=match_guid,
                                    action_time_epoch=action_time_epoch,
                                    player_guid=player_guid,
                                    affect_guid=affect_guid,
                                    affect_state=affect_state
                                )
                                
                            time.sleep(0.00000125) 
                        
                            # Log progress at specified intervals
                            if (idx + 1) % progress_interval == 0:
                                progress = (idx + 1) / total_actions * 100
                                logger.info(f"Progress: {progress:.1f}% ({idx + 1}/{total_actions} actions processed)")
                            
                            # Count action types for summary
                            if action[1] in action_counts:
                                action_counts[action[1]] += 1
                        
                        # Log action summary for this match
                        logger.info(f"Action summary for match {match_id}:")
                        for action_type, count in action_counts.items():
                            if total_actions > 0:
                                logger.info(f"  {action_type}: {count} actions ({count/total_actions*100:.1f}%)")
                        
                        match_duration = time.time() - match_process_start
                        logger.info(f"Match {match_id} processed successfully in {match_duration:.2f} seconds")
                        successful_matches += 1
                        
                    except Exception as e:
                        logger.error(f"Failed to process match {match_id}: {str(e)}")
                        import traceback
                        logger.error(traceback.format_exc())
                        failed_matches += 1
                    
                    # Log progress
                    progress_pct = (processed_matches / total_matches) * 100
                    logger.info(f"Overall progress: {progress_pct:.1f}% ({processed_matches}/{total_matches})")
                
                # Mark the end of the game AFTER all matches are processed
                logger.info(f"Marking the end of game {game_id} with game_guid: {game_guid}")
                sdk.mark_end_game(game_guid)
                
            except Exception as e:
                logger.error(f"Error processing game {game_id}: {str(e)}")
                import traceback
                logger.error(traceback.format_exc())
            
            game_duration = time.time() - game_start_time
            logger.info(f"Game {game_id} processing completed in {game_duration:.2f} seconds")
        
        # Log summary statistics
        logger.info("=====================================================")
        logger.info("Processing Summary:")
        logger.info(f"Total games processed: {processed_games}/{total_games}")
        logger.info(f"Total matches processed: {processed_matches}/{total_matches}")
        if processed_matches > 0:
            logger.info(f"Successful matches: {successful_matches} ({successful_matches/processed_matches*100:.1f}%)")
            logger.info(f"Failed matches: {failed_matches} ({failed_matches/processed_matches*100:.1f}%)")
        logger.info("=====================================================")
        
        # Stop the SDK and wait for any pending operations
        logger.info("Flushing SDK - waiting for all pending operations to complete...")
        flush_result = sdk.flush()
        if flush_result == 1:
            logger.info("Flush completed successfully - all actions sent")
        else:
            logger.warning("Flush timed out - some actions may not have been sent")
        
    except Exception as e:
        logger.error(f"Error in main process: {str(e)}")
        import traceback
        logger.error(traceback.format_exc())
    finally:
        # Ensure SDK is stopped even if there was an error
        try:
            if 'sdk' in locals():
                sdk.dispose()
                time.sleep(2)
        except:
            pass
    logger.info("Finished streaming matches to different title")

if __name__ == "__main__":
    script_start_time = time.time()
    logger.info("Script started")
    
    # ===================================================================
    # CHANGE THE VALUES BELOW TO SELECT CONFIGURATION
    # ===================================================================
    # Title pair to use (options: "cs2-eval", "smite-2-eval", "cs2-renown-eval", "valorant-grid-eval")
    SELECTED_TITLE_PAIR = "cs2-eval"
    
    # CSV file path containing matches to copy (leave empty or None to copy all matches)
    # Format of CSV: title_id,game_id,match_id (one match per row)
    CSV_FILE_PATH = "matches.csv"  # Set to None or "" to copy all matches
    # ===================================================================
    
    try:
        # Display the selected title pair info
        pair = get_title_pair(SELECTED_TITLE_PAIR)
        print(f"Selected title pair: {SELECTED_TITLE_PAIR}")
        print(f"Name: {pair['name']}")
        print(f"Source title ID: {pair['target_title_id']}")
        print(f"Destination title ID: {pair['destination_title_id']}")
        
        # Process CSV file if provided
        if CSV_FILE_PATH:
            print(f"CSV file: {CSV_FILE_PATH}")
            # Validate the CSV file exists
            if not os.path.isfile(CSV_FILE_PATH):
                logger.error(f"CSV file not found: {CSV_FILE_PATH}")
                print(f"Error: CSV file not found: {CSV_FILE_PATH}")
                print("Will copy all matches in the time window instead.")
                CSV_FILE_PATH = None
        else:
            print("No CSV file provided. Will copy all matches in the time window.")
        
        print("---------------------------------------------------")
        
        # Run the title data copy process
        stream_matches_to_different_title(SELECTED_TITLE_PAIR, CSV_FILE_PATH)
        script_duration = time.time() - script_start_time
        logger.info(f"Script completed successfully in {script_duration:.2f} seconds")
    except Exception as e:
        logger.error(f"Script failed with error: {str(e)}")
        import traceback
        logger.error(traceback.format_exc())
        script_duration = time.time() - script_start_time
        logger.info(f"Script failed after {script_duration:.2f} seconds")