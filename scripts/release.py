import subprocess
import sys
import re
import json
from pathlib import Path

ROOT = Path(__file__).parent.parent


def run(cmd, check=True):
    return subprocess.run(cmd, capture_output=True, text=True, check=check, cwd=ROOT)


def get_current_branch():
    result = run(["git", "rev-parse", "--abbrev-ref", "HEAD"])
    return result.stdout.strip()


def is_work_tree_clean():
    result = run(["git", "status", "--porcelain"])
    return result.stdout.strip() == ""


def get_current_version():
    cmake = (ROOT / "CMakeLists.txt").read_text()
    match = re.search(r'project\(duk VERSION (\d+)\.(\d+)\.(\d+)\)', cmake)
    if not match:
        print("Error: Could not find version in CMakeLists.txt")
        sys.exit(1)
    return int(match.group(1)), int(match.group(2)), int(match.group(3))


def bump_version(major, minor, patch, bump_type):
    if bump_type == "major":
        return major + 1, 0, 0
    elif bump_type == "minor":
        return major, minor + 1, 0
    else:
        return major, minor, patch + 1


def update_cmake(old_version, new_version):
    path = ROOT / "CMakeLists.txt"
    content = path.read_text()
    old = f"project(duk VERSION {old_version})"
    new = f"project(duk VERSION {new_version})"
    content = content.replace(old, new)
    path.write_text(content)


def update_vcpkg(new_version):
    path = ROOT / "vcpkg.json"
    data = json.loads(path.read_text())
    data["version"] = new_version
    path.write_text(json.dumps(data, indent=2) + "\n")


def main():
    print("Select a version type to release: 1. Patch, 2. Minor, 3. Major")
    choice = input("Enter choice (1/2/3): ").strip()

    bump_map = {"1": "patch", "2": "minor", "3": "major"}
    if choice not in bump_map:
        print("Invalid choice.")
        sys.exit(1)
    bump_type = bump_map[choice]

    branch = get_current_branch()
    if branch != "develop":
        print(f"Error: You must be on the 'develop' branch (currently on '{branch}').")
        sys.exit(1)

    if not is_work_tree_clean():
        print("Error: Working tree is not clean. Please commit or stash your changes.")
        sys.exit(1)

    major, minor, patch = get_current_version()
    old_version_str = f"{major}.{minor}.{patch}"

    new_major, new_minor, new_patch = bump_version(major, minor, patch, bump_type)
    new_version_str = f"{new_major}.{new_minor}.{new_patch}"

    print(f"Bumping version: {old_version_str} -> {new_version_str}")

    update_cmake(old_version_str, new_version_str)
    update_vcpkg(new_version_str)

    run(["git", "add", "CMakeLists.txt", "vcpkg.json"])
    run(["git", "commit", "-m", f"pkg: version {new_version_str}"])
    run(["git", "tag", new_version_str])
    run(["git", "push"])
    run(["git", "push", "origin", new_version_str])

    print(f"Released version {new_version_str} successfully.")


if __name__ == "__main__":
    main()

