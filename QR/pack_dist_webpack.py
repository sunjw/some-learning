import os
import platform
import shlex
import shutil
import stat
import sys

from subprocess import Popen, PIPE, STDOUT

# Log to stdout
def log(message):
    print('%s' % (message))

# Log to stderr
def log_err(message):
    print(message, file=sys.stderr)

def log_stage(stage_message):
    log('\n%s\n' % (stage_message))

def is_windows_sys():
    return (platform.system() == 'Windows')

def is_macos_sys():
    return (platform.system() == 'Darwin')

def fix_win_path(path):
    if is_windows_sys():
        return path.replace('/', '\\')
    else:
        return path

def fix_win_long_path(path):
    if is_windows_sys() and not path.startswith('\\\\?\\'):
        abs_path = os.path.abspath(path)
        return '\\\\?\\' + abs_path
    return path

def find_strings_in_string(strings, in_string):
    for str_itr in strings:
        if in_string.find(str_itr) >= 0:
            return True

    return False

def copy_file(source, dest):
    follow_symlinks_os = False
    if is_windows_sys():
        follow_symlinks_os = True
    shutil.copyfile(fix_win_long_path(source), fix_win_long_path(dest), follow_symlinks=follow_symlinks_os)

def copy_dir(source, dest):
    for filename in os.listdir(source):
        src_file = os.path.join(source, filename)
        dst_file = os.path.join(dest, filename)
        if os.path.isdir(src_file):
            if not os.path.exists(dst_file):
                os.mkdir(dst_file)
            copy_dir(src_file, dst_file)

        if os.path.isfile(src_file):
            copy_file(src_file, dst_file)

def remove_file(path):
    fixed_path = fix_win_long_path(path)
    if os.path.exists(fixed_path):
        os.remove(fixed_path)

def remove_dir(path):
    fixed_path = fix_win_long_path(path)
    if os.path.exists(fixed_path):
        shutil.rmtree(fixed_path)

def read_file_binary(file_path):
    if not os.path.exists(file_path):
        return ''
    with open(file_path, 'rb') as f:
        file_content = f.read()
    return file_content

def write_file_binary(file_path, file_content):
    file_obj = open(file_path, 'wb')
    file_obj.write(file_content)
    file_obj.close()

def run_cmd(cmd):
    log('Run: \n%s' % (cmd))
    os.system(cmd)

def run_cmd_with_stdio(cmd, input_data):
    log('Run: \n%s' % (cmd))
    p = Popen(shlex.split(cmd), stdout=PIPE, stdin=PIPE, stderr=STDOUT)
    return p.communicate(input = input_data)[0]

def run_cmd_with_stderr(cmd):
    log('Run: \n%s' % (cmd))
    p = Popen(shlex.split(cmd), stdout=PIPE, stdin=PIPE, stderr=PIPE)

    stdout_text, stderr_text = p.communicate()

    return stdout_text, stderr_text

PUBLISH_DIR = 'publish'

APP_DIRS = ['dist']
APP_FILES = ['index.html', 'qrdec.html']
CLEAR_FILES = []

def main():
    cwd = os.getcwd()

    publish_dir_absolute = os.path.join(cwd, PUBLISH_DIR)
    if not os.path.exists(publish_dir_absolute):
        os.mkdir(publish_dir_absolute)
    else:
        remove_dir(publish_dir_absolute)
        os.mkdir(publish_dir_absolute)

    # Update modules.
    log_stage('Update modules...')
    run_cmd('npm install')

    # Build webpack.
    log_stage('Build webpack...')
    run_cmd('npm run build')

    # Copy new app.
    log_stage('Copy new app...')
    for app_dir in APP_DIRS:
        dest_app_dir = os.path.join(PUBLISH_DIR, app_dir)
        os.mkdir(dest_app_dir)
        copy_dir(app_dir, dest_app_dir)

    for app_file in APP_FILES:
        dest_app_file = os.path.join(PUBLISH_DIR, app_file)
        copy_file(app_file, dest_app_file)

    # Clear up.
    log_stage('Clear up...')
    for clear_file in CLEAR_FILES:
        dest_clear_file = os.path.join(PUBLISH_DIR, clear_file)
        if os.path.exists(dest_clear_file):
            os.remove(dest_clear_file)

if __name__ == '__main__':
    main()
