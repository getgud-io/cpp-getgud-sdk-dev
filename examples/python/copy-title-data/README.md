# GetGud Title Data Copy Tool

This tool allows you to copy data between different GetGud title IDs. It fetches match data from a source title and replicates it to a destination title using the GetGud SDK.

## Setup

1. Configure your title pairs in `title_config.py`
   - Each title pair contains:
     - `name`: Descriptive name for the pair
     - `access_token_id`: Access token for API authentication
     - `id_token`: ID token for API authentication
     - `target_title_id`: Source title ID to copy from
     - `destination_title_id`: Destination title ID to copy to
     - `destination_private_key`: Private key for the destination title

2. Replace the placeholder values in `title_config.py` with your actual API tokens and title IDs

## Usage

The tool now supports two modes:

1. **Copy all matches** from the past 365 days
2. **Copy specific matches** from a CSV file

### Configuration

To select which title pair to use and whether to use a CSV file for specific matches, open `main.py` and modify the constants at the top of the main block:

```python
# ===================================================================
# CHANGE THE VALUES BELOW TO SELECT CONFIGURATION
# ===================================================================
# Title pair to use (options: "cs2-eval", "smite-2-eval", "cs2-renown-eval", "valorant-grid-eval")
SELECTED_TITLE_PAIR = "cs2-eval"

# CSV file path containing matches to copy (leave empty or None to copy all matches)
# Format of CSV: title_id,game_id,match_id (one match per row)
CSV_FILE_PATH = "matches.csv"  # Set to None or "" to copy all matches
# ===================================================================
```

### Option 1: Copy All Matches in Time Window

To copy all matches from the past 365 days, set `CSV_FILE_PATH = None` or an empty string in the configuration section of `main.py`.

### Option 2: Copy Specific Matches from CSV

To copy only specific matches defined in a CSV file, set `CSV_FILE_PATH` to the path of your CSV file (e.g., `CSV_FILE_PATH = "matches.csv"`).

#### CSV File Format

The CSV file should have the following format:

```
title_id,game_id,match_id
133,game_123,match_456
133,game_123,match_789
...
```

- `title_id`: The source title ID
- `game_id`: The game ID containing the match
- `match_id`: The specific match ID to copy

The tool will only copy matches that belong to the specified title ID in the selected title pair.

### Run the Script

Once you've configured the settings in `main.py`, run the script:

```bash
python main.py
```

## How It Works

When using a CSV file:
1. The tool reads the specified CSV file to get the list of matches to copy
2. It searches for those specific matches within a 365-day time window
3. It processes only the matches specified in the CSV that belong to the target title

When not using a CSV file:
1. The tool fetches all matches from the past 365 days for the target title
2. It processes all matches found

In both cases, the matches are copied to the destination title with all their action data.

## Logs

All operations are logged to:
- Console (info level)
- Log file in the `logs` directory (debug level)

Log files are named with timestamps in the format: `copy_title_data_YYYYMMDD_HHMMSS.log`