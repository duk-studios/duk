#!/usr/bin/env python3
"""
Python script to format or check formatting of all C++ source files in the duk project using clang-format.
This replaces the CMake duk_format and duk_format_check targets.
"""

import os
import sys
import subprocess
import glob
import argparse
from pathlib import Path


def find_clang_format():
    """Find clang-format executable."""
    try:
        result = subprocess.run(['clang-format', '--version'], 
                              capture_output=True, text=True, check=True)
        print(result.stdout.strip())
        return 'clang-format'
    except (subprocess.CalledProcessError, FileNotFoundError):
        # Try common alternative names
        alternatives = ['clang-format-18', 'clang-format-17', 'clang-format-16', 
                       'clang-format-15', 'clang-format-14']
        for alt in alternatives:
            try:
                subprocess.run([alt, '--version'], 
                             capture_output=True, text=True, check=True)
                return alt
            except (subprocess.CalledProcessError, FileNotFoundError):
                continue
        
        print("Error: clang-format not found. Please make sure it is installed and in PATH.")
        sys.exit(1)


def find_source_files(project_root):
    """Find all .cpp and .h files in directories starting with 'duk_'."""
    source_files = []
    
    # Find all directories starting with "duk_"
    duk_dirs = glob.glob(os.path.join(project_root, 'duk_*'))
    
    for duk_dir in duk_dirs:
        if os.path.isdir(duk_dir):
            # Find all .cpp and .h files recursively in each duk_ directory
            cpp_files = glob.glob(os.path.join(duk_dir, '**', '*.cpp'), recursive=True)
            h_files = glob.glob(os.path.join(duk_dir, '**', '*.h'), recursive=True)
            source_files.extend(cpp_files)
            source_files.extend(h_files)
    
    return sorted(source_files)


def format_files(clang_format_exe, files, project_root, extra_args=None):
    """Format the given files using clang-format."""
    if not files:
        print("No source files found to format.")
        return True
    
    print(f"Found {len(files)} files to format.")
    print("Formatting files...")
    
    try:
        # Run clang-format with -i flag to format in-place
        cmd = [clang_format_exe, '-i']
        if extra_args:
            cmd.extend(extra_args)
        cmd.extend(files)
        
        result = subprocess.run(cmd, cwd=project_root, check=True)
        print(f"Successfully formatted {len(files)} files.")
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error running clang-format: {e}")
        return False


def check_formatting(clang_format_exe, files, project_root, extra_args=None):
    """Check formatting of the given files using clang-format."""
    if not files:
        print("No source files found to check.")
        return True

    print(f"Found {len(files)} files to check.")
    print("Checking file formatting...")

    try:
        # Run clang-format with --dry-run and --Werror flags
        cmd = [clang_format_exe, '--dry-run', '--Werror']
        if extra_args:
            cmd.extend(extra_args)
        cmd.extend(files)
        
        result = subprocess.run(cmd, cwd=project_root, check=True,
                                capture_output=True, text=True)
        print(f"All {len(files)} files are properly formatted.")
        return True
    except subprocess.CalledProcessError as e:
        print("Formatting check failed!")
        if e.stderr:
            print("clang-format errors:")
            print(e.stderr)
        if e.stdout:
            print("clang-format output:")
            print(e.stdout)
        print(f"\nTo fix formatting issues, run: python {sys.argv[0]}")
        return False


def main():
    """Main function."""
    parser = argparse.ArgumentParser(
        description='Format or check formatting of C++ source files in the duk project using clang-format.',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python format.py                    # Format all source files
  python format.py --check            # Check formatting of all source files
  python format.py --style=Google     # Format with Google style
  python format.py --check --verbose  # Check with verbose output
        """
    )
    
    parser.add_argument('--check', action='store_true',
                        help='Check formatting instead of applying it')
    parser.add_argument('--dry-run', action='store_true',
                        help='Show what would be formatted without making changes')
    parser.add_argument('--verbose', action='store_true',
                        help='Enable verbose output')
    
    # Add common clang-format options
    parser.add_argument('--style', metavar='STYLE',
                        help='Coding style to use (e.g., LLVM, Google, Chromium, Mozilla, WebKit)')
    parser.add_argument('--fallback-style', metavar='STYLE',
                        help='Fallback style to use if style is not found')
    parser.add_argument('--assume-filename', metavar='FILENAME',
                        help='Override filename used to determine the language')
    parser.add_argument('--sort-includes', action='store_true',
                        help='Sort includes')
    
    # Parse known args to allow passing through other clang-format options
    args, unknown_args = parser.parse_known_args()
    
    # Get the project root directory (parent of scripts directory)
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    
    if args.verbose:
        print(f"Project root: {project_root}")
    
    # Find clang-format executable
    clang_format_exe = find_clang_format()
    if args.verbose:
        print(f"Using clang-format: {clang_format_exe}")
    
    # Find all source files
    source_files = find_source_files(project_root)
    
    if not source_files:
        print("No source files found in duk_* directories.")
        return 0
    
    # Build extra arguments for clang-format
    extra_args = []
    if args.style:
        extra_args.extend(['--style', args.style])
    if args.fallback_style:
        extra_args.extend(['--fallback-style', args.fallback_style])
    if args.assume_filename:
        extra_args.extend(['--assume-filename', args.assume_filename])
    if args.sort_includes:
        extra_args.append('--sort-includes')
    if args.verbose:
        extra_args.append('--verbose')
    
    # Add any unknown arguments (pass-through to clang-format)
    extra_args.extend(unknown_args)
    
    # Determine operation mode
    if args.check:
        success = check_formatting(clang_format_exe, source_files, project_root, extra_args)
    elif args.dry_run:
        # For dry-run, we use check mode but with different messaging
        print(f"Found {len(source_files)} files that would be formatted.")
        print("Dry-run mode: showing what would be changed...")
        # Remove --Werror for dry-run to see all changes
        cmd = [clang_format_exe, '--dry-run']
        if extra_args:
            # Filter out --Werror if it was added
            filtered_args = [arg for arg in extra_args if arg != '--Werror']
            cmd.extend(filtered_args)
        cmd.extend(source_files)
        
        try:
            result = subprocess.run(cmd, cwd=project_root, check=False,
                                    capture_output=True, text=True)
            if result.stdout:
                print("Files that would be changed:")
                print(result.stdout)
            if result.stderr:
                print("Warnings/Errors:")
                print(result.stderr)
            if result.returncode == 0:
                print("No formatting changes needed.")
            success = True
        except subprocess.CalledProcessError as e:
            print(f"Error running clang-format: {e}")
            success = False
    else:
        success = format_files(clang_format_exe, source_files, project_root, extra_args)
    
    return 0 if success else 1


if __name__ == '__main__':
    sys.exit(main())
