# use cmd.exe instead of sh:
set shell := ["cmd.exe", "/c"]

format:
  python scripts/format.py

convert_animation input output:
  python scripts/convert_animation.py {{input}} {{output}}

convert_objects input:
  python scripts/convert_objects.py {{input}}