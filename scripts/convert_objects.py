#!/usr/bin/env python3
"""
Python script to convert Objects container JSON from old format to new format.

Changes made:
1. Each object gets a unique random ID (between 1,000,000 and uint64_t max)
2. Parent field changes from index-based to ID-based
3. Component data is wrapped in a "data" field
"""

import json
import sys
import random
import argparse
from pathlib import Path
from glob import glob


# Constants
MIN_ID = 1_000_000
MAX_ID = 2**64 - 1


def generate_unique_id(used_ids):
    """Generate a unique random ID that hasn't been used yet."""
    while True:
        new_id = random.randint(MIN_ID, MAX_ID)
        if new_id not in used_ids:
            used_ids.add(new_id)
            return new_id


def convert_objects_array(old_objects_array):
    """Convert an array of objects from old format to new format."""
    used_ids = set()
    id_mapping = {}  # Maps old index to new ID
    new_data = []

    # First pass: Generate IDs for all objects and build mapping
    for idx in range(len(old_objects_array)):
        new_id = generate_unique_id(used_ids)
        id_mapping[idx] = new_id

    # Second pass: Convert objects
    for idx, old_obj in enumerate(old_objects_array):
        new_obj = {}

        # Add ID
        new_obj["id"] = id_mapping[idx]

        # Convert parent field if it exists
        if "parent" in old_obj:
            old_parent_idx = old_obj["parent"]
            # Map old parent index to new parent ID
            new_obj["parent"] = id_mapping[old_parent_idx]
        else:
            new_obj["parent"] = 0  # Default parent to 0 if not specified (no parent)

        # Convert components
        if "components" in old_obj:
            new_components = []
            for component in old_obj["components"]:
                new_component = {}

                # Extract type
                if "type" in component:
                    new_component["type"] = component["type"]
                else:
                    raise ValueError("Component must have a 'type' field")

                # Extract data - everything except "type"
                component_data = {k: v for k, v in component.items() if k != "type"}

                # Only add "data" field if there's actual data
                if component_data:
                    new_component["data"] = component_data

                new_components.append(new_component)

            new_obj["components"] = new_components

        new_data.append(new_obj)

    return new_data


def convert_objects(old_data):
    """
    Convert old format to new format.

    Handles two input formats:
    1. Direct array of objects
    2. JSON object with "objects" member containing the array

    Returns the converted data in the same format as input.
    """
    if isinstance(old_data, list):
        # Format 1: Direct array
        return convert_objects_array(old_data)
    elif isinstance(old_data, dict):
        # Format 2: JSON object with "objects" member
        if "objects" in old_data:
            old_objects = old_data["objects"]
            if not isinstance(old_objects, list):
                raise ValueError("'objects' member must be an array")

            # Convert the objects array
            new_objects = convert_objects_array(old_objects)

            # Return the object with converted objects and other members unchanged
            result = old_data.copy()
            result["objects"] = new_objects
            return result
        else:
            raise ValueError("Root element must be an array or an object with 'objects' member")
    else:
        raise ValueError("Root element must be an array or a JSON object")


def main():
    parser = argparse.ArgumentParser(
        description="Convert Objects container JSON from old format to new format"
    )
    parser.add_argument(
        "input",
        help="Input JSON file (old format) or directory to process recursively"
    )
    parser.add_argument(
        "-o", "--output",
        help="Output JSON file (new format). Only valid when input is a file. If not specified with file input, output is printed to stdout"
    )

    args = parser.parse_args()

    input_path = Path(args.input)

    # Check if input is a directory
    if input_path.is_dir():
        if args.output:
            print("Error: -o/--output cannot be used with directory input", file=sys.stderr)
            sys.exit(1)

        # Find all .scn and .obj files recursively
        files_to_process = []
        for pattern in ["**/*.scn", "**/*.obj"]:
            files_to_process.extend(input_path.glob(pattern))

        if not files_to_process:
            print(f"No .scn or .obj files found in {input_path}", file=sys.stderr)
            sys.exit(1)

        print(f"Found {len(files_to_process)} file(s) to process", file=sys.stderr)

        # Process each file
        for file_path in files_to_process:
            process_file(file_path, file_path)

        print(f"Successfully converted {len(files_to_process)} file(s)", file=sys.stderr)
    else:
        # Single file processing
        if not input_path.exists():
            print(f"Error: Input file '{args.input}' not found", file=sys.stderr)
            sys.exit(1)

        output_path = Path(args.output) if args.output else None
        process_file(input_path, output_path)


def process_file(input_path, output_path):
    """
    Process a single file: read, convert, and write.

    Args:
        input_path: Path to input JSON file
        output_path: Path to output JSON file (or None to print to stdout)
    """
    try:
        with open(input_path, 'r') as f:
            old_data = json.load(f)
    except json.JSONDecodeError as e:
        print(f"Error: Failed to parse JSON in {input_path}: {e}", file=sys.stderr)
        return
    except Exception as e:
        print(f"Error: Failed to read {input_path}: {e}", file=sys.stderr)
        return

    # Convert
    try:
        new_data = convert_objects(old_data)
    except ValueError as e:
        print(f"Error: Conversion failed for {input_path}: {e}", file=sys.stderr)
        return

    # Output
    output_json = json.dumps(new_data, indent=2)

    if output_path:
        try:
            with open(output_path, 'w') as f:
                f.write(output_json)
            print(f"Successfully converted and saved to {output_path}", file=sys.stderr)
        except Exception as e:
            print(f"Error: Failed to write to {output_path}: {e}", file=sys.stderr)
    else:
        print(output_json)


if __name__ == "__main__":
    main()
