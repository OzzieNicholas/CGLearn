import sys
import os
from pathlib import Path
from typing import List, Set
import subprocess
import difflib
from io import StringIO
import platform
sys.stdout.reconfigure(encoding='utf-8')

FORMAT_VALID_CPP_TYPES = set(('cpp', 'h', 'hpp', 'hh', 'inl', 'cc', 'cu'))
FORMAT_EXCLUDE_PATHS = ["loong/IFWrapper/"]
FORMAT_CHECK_ARGS = ['-style=file']
FORMAT_FILE_ARGS = ['-style=file', '-i']

system = platform.system().lower()

bin_path = Path(os.path.dirname(__file__)).parent.joinpath('bin')

if system.startswith('windows'):
    bin_path = bin_path.joinpath('windows')
elif system.startswith('darwin'):
    bin_path = bin_path.joinpath('darwin')
elif system.startswith('linux'):
    bin_path = bin_path.joinpath('linux')
else:
    bin_path = bin_path.joinpath('unknown')

if not bin_path.exists():
    # bypass format check
    sys.exit(0)

clang_format = bin_path.joinpath('clang-format').__str__()


def get_format_valid_cpp_files(work_dir: Path, all_changes: List[str]) -> List[str]:
    files = []

    for change in all_changes:
        idx = change.rfind('.')

        if idx == -1 or '/third_party/' in change or '/misc/' in change:
            continue

        # check if chane is excluded
        exclude = False
        for exclude_path in FORMAT_EXCLUDE_PATHS:
            if change.startswith(exclude_path):
                exclude = True
                break
        if exclude:
            continue

        # check if change is valid type
        if change[idx + 1:] not in FORMAT_VALID_CPP_TYPES:
            continue

        # save path of the change
        file = work_dir.joinpath(change)
        if file.exists():
            files.append(change)

    return files


def format_cpp_files(files: List[str], auto_format=False):
    diff_strings = []
    base_check_cmds = [clang_format] + FORMAT_CHECK_ARGS
    base_format_cmds = [clang_format] + FORMAT_FILE_ARGS

    for file in files:
        if auto_format:
            subprocess.call(base_format_cmds + [file])
            continue

        cmds = base_check_cmds + [file]

        try:
            p = subprocess.Popen(cmds,
                                 stdout=subprocess.PIPE,
                                 stderr=None,
                                 stdin=subprocess.PIPE,
                                 universal_newlines=True,
                                 encoding='utf-8')

        except OSError as e:
            # Give the user more context when clang-format isn't
            # found/isn't executable, etc.
            raise RuntimeError(
                'Failed to run "%s" - %s"' % (" ".join(cmds), e.strerror))

        stdout, stderr = p.communicate()
        if p.returncode != 0:
            sys.exit(p.returncode)

        with open(file, encoding='utf-8') as f:
            code = f.readlines()

        formatted_code = StringIO(stdout).readlines()
        diff = difflib.unified_diff(code, formatted_code,
                                    file, file,
                                    '(before formatting)', '(after formatting)')
        diff_string = ''.join(diff)
        if len(diff_string) > 0:
            diff_strings.append(diff_string)

    if diff_strings:
        sys.stdout.write('Code format check failed!\n\n')
        sys.stdout.write('\n'.join(diff_strings))
        format_cpp_files(files, True)

    if auto_format:
        print('\nFiles has been automatically formatted, please check changes!\n')
        sys.exit(-1)


def get_format_valid_rust_files(work_dir: Path, all_changes: Set[str]) -> List[str]:
    files = []

    for change in all_changes:
        idx = change.rfind('.')

        if idx == -1 or '/third_party/' in change or '/misc/' in change:
            continue

        # check if chane is excluded
        exclude = False
        for exclude_path in FORMAT_EXCLUDE_PATHS:
            if change.startswith(exclude_path):
                exclude = True
                break
        if exclude:
            continue

        # check if change is valid type
        if change[idx + 1:] != 'rs':
            continue

        # save path of the change
        file = work_dir.joinpath(change)
        if file.exists():
            files.append(change)

    return files


def format_rust_files(files: List[str], auto_format=False):
    diff_strings = []

    for file in files:
        if auto_format:
            subprocess.call(['rustfmt', file])
            continue

        cmds = ['rustfmt', '--emit', 'stdout', file]

        try:
            p = subprocess.Popen(cmds,
                                 stdout=subprocess.PIPE,
                                 stderr=None,
                                 stdin=subprocess.PIPE,
                                 universal_newlines=True,
                                 encoding='utf-8')

        except OSError as e:
            # Give the user more context when clang-format isn't
            # found/isn't executable, etc.
            raise RuntimeError(
                'Failed to run "%s" - %s"' % (" ".join(cmds), e.strerror))

        stdout, stderr = p.communicate()
        if p.returncode != 0:
            sys.exit(p.returncode)

        if not stdout:
            print(f'failed to format {file}')
            continue

        with open(file, encoding='utf-8') as f:
            code = f.readlines()

        all_formatted_code = StringIO(stdout).readlines()

        formatted_code = []
        found_file = False
        name = os.path.split(file)[1] + ':'
        for line in all_formatted_code:
            if line.startswith('\\\?'):
                found_file = line.strip().endswith(name)
            if found_file:
                formatted_code.append(line)

        formatted_code = formatted_code[2:]

        diff = difflib.unified_diff(code, formatted_code,
                                    file, file,
                                    '(before formatting)', '(after formatting)')
        diff_string = ''.join(diff)
        if len(diff_string) > 0:
            diff_strings.append(diff_string)

    if diff_strings:
        sys.stdout.write('Code format check failed!\n\n')
        sys.stdout.write('\n'.join(diff_strings))
        format_rust_files(files, True)

    if auto_format:
        print('\nFiles has been automatically formatted, please check changes!\n')
        sys.exit(-1)


def check_format(work_dir: Path, all_changes: List[str]):
    cpp_files: List[Path] = get_format_valid_cpp_files(work_dir, all_changes)
    if cpp_files:
        format_cpp_files(cpp_files)

    rust_files = get_format_valid_rust_files(
        work_dir, set(all_changes) - set(cpp_files))
    if rust_files:
        format_rust_files(rust_files)


if __name__ == '__main__':
    work_dir = Path(sys.argv[1]).absolute()
    all_changes = sys.argv[2].split('\n')
    check_format(work_dir, all_changes)
