#!/usr/bin/env python3
"""
Simple script to convert old animation JSON format to new format.
This is a one-time use script and will be discarded after conversion.
"""

import json
import sys
from pathlib import Path

# Mapping of old condition types to new ones
CONDITION_TYPE_MAP = {
    "finished": "duk::animation::FinishedCondition",
    "comparison": "duk::animation::ComparisonCondition",
    "trigger": "duk::animation::TriggerCondition"
}


def convert_condition(condition):
    """Convert a single condition from old format to new format."""
    old_type = condition.get("type")
    new_type = CONDITION_TYPE_MAP.get(old_type, old_type)

    # Create new condition with just the type
    new_condition = {"type": new_type}

    # Collect all fields except "type" into a "value" object
    value_fields = {k: v for k, v in condition.items() if k != "type"}

    # Only add "value" object if there are fields to put in it
    if value_fields:
        new_condition["value"] = value_fields

    return new_condition


def convert_variable(variable):
    """Convert a single variable from old format to new format."""
    new_variable = {
        "key": variable.get("name")
    }

    # Put type and value into a value object
    new_variable["value"] = {
        "type": variable.get("type"),
        "value": variable.get("value")
    }

    return new_variable


def convert_animation_json(input_file, output_file):
    """Convert animation JSON from old format to new format."""

    # Load the old JSON
    with open(input_file, 'r') as f:
        data = json.load(f)

    # Convert animations
    if "animations" in data:
        for animation in data["animations"]:
            if "transitions" in animation:
                for transition in animation["transitions"]:
                    if "conditions" in transition:
                        transition["conditions"] = [
                            convert_condition(cond)
                            for cond in transition["conditions"]
                        ]

    # Convert variables
    if "variables" in data:
        data["variables"] = [
            convert_variable(var)
            for var in data["variables"]
        ]

    # Write the new JSON
    with open(output_file, 'w') as f:
        json.dump(data, f, indent=2)

    print(f"Conversion complete!")
    print(f"Input: {input_file}")
    print(f"Output: {output_file}")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python convert_animation.py <input_file> <output_file>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    convert_animation_json(input_file, output_file)

