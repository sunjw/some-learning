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

    return stderr_text

def is_windows_sys():
    return (platform.system() == 'Windows')

def is_macos_sys():
    return (platform.system() == 'Darwin')

def fix_win_path(path):
    if is_windows_sys():
        return path.replace('/', '\\')
    else:
        return path

def find_strings_in_string(strings, in_string):
    for str_itr in strings:
        if in_string.find(str_itr) >= 0:
            return True

    return False

def copy_file(source, dest):
    shutil.copyfile(source, dest, follow_symlinks=False)

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
    if os.path.exists(path):
        os.remove(path)

def remove_dir(path):
    if os.path.exists(path):
        shutil.rmtree(path)

def read_file_content(file_path):
    if not os.path.exists(file_path):
        return ''
    file_content = open(file_path, 'rb').read()
    return file_content

def write_file_content(file_path, file_content):
    file_obj = open(file_path, 'wb')
    file_obj.write(file_content)
    file_obj.close()

def log_stage(stage_message):
    log('\n%s\n' % (stage_message))

def main():
    cwd = os.getcwd()

    publish_dir = 'publish'
    publish_dir_absolute = os.path.join(cwd, publish_dir)
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
    app_dirs = ['dist']
    for app_dir in app_dirs:
        dest_app_dir = os.path.join(publish_dir, app_dir)
        os.mkdir(dest_app_dir)
        copy_dir(app_dir, dest_app_dir)

    app_files = ['index.html']
    for app_file in app_files:
        dest_app_file = os.path.join(publish_dir, app_file)
        copy_file(app_file, dest_app_file)

    # Clear up.
    log_stage('Clear up...')
    clear_files = []
    for clear_file in clear_files:
        dest_clear_file = os.path.join(publish_dir, clear_file)
        if os.path.exists(dest_clear_file):
            os.remove(dest_clear_file)

if __name__ == '__main__':
    main()
