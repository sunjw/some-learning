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

    return stdout_text, stderr_text

def is_windows_sys():
    return (platform.system() == 'Windows')

def is_macos_sys():
    return (platform.system() == 'Darwin')

def is_linux_sys():
    return (platform.system() == 'Linux')

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
    file_content = open(file_path, 'rb').read()
    return file_content

def write_file_binary(file_path, file_content):
    file_obj = open(file_path, 'wb')
    file_obj.write(file_content)
    file_obj.close()

def log_stage(stage_message):
    log('\n%s\n' % (stage_message))

EXE_7Z_WIN = '7z'
EXE_RESOURCE_HACKER_WIN = 'ResourceHacker.exe'

USING_XZ_MACOS = True
SUPPORT_LINUX = False

APP_TITLE = 'wallpaper-picker'
PACKAGE_NAME = 'wallpaper-picker'

RESOURCE_HACKER_SCRIPT = 'rh_script_upd_icons.txt'
APP_BUNDLE_ID_MACOS = 'org.sunjw.WallpaperPicker'
APP_ICNS_MACOS = 'icons/mac/icon.icns'

DIST_DIR = 'dist'
APP_DIRS = ['node_modules', 'assets', 'css', 'js']
APP_FILES = ['index.html', 'main.js',
            'package.json', 'package-lock.json',
            'README.md', 'LICENSE']

EXEC_FIX_PATHS = ['./node_modules/.bin/electron-rebuild',
                './node_modules/@sunjw8888/wallpaper/source/macos-wallpaper']

REBUILD_CMD = 'rebuild-all'
REBUILD_CLEAN_PATHS_WIN = ['./node_modules/better-sqlite3/build/Release/obj',
                        './node_modules/better-sqlite3/build/deps/Release/obj',
                        './node_modules/icon-gen', './node_modules/@jimp']
REBUILD_CLEAN_PATHS_MACOS = ['./node_modules/icon-gen', './node_modules/@jimp']
REBUILD_CLEAN_PATHS_LINUX = []

def main():
    exe_7z_sys = EXE_7Z_WIN # 7z is not supported on macOS
    tar_ext = 'gz'
    tar_param = '-czvf'
    if USING_XZ_MACOS:
        tar_ext = 'xz'
        tar_param = '-cJvf'
    if is_linux_sys() and (not SUPPORT_LINUX):
        log_stage('Not supported Linux.')
        exit()

    system_name = 'win'
    if is_macos_sys():
        system_name = 'macos'
    elif is_linux_sys():
        system_name = 'linux'
    machine_name = platform.machine()
    machine_name = machine_name.lower()

    app_name = PACKAGE_NAME
    app_dir_path_relative = 'resources/app'
    if is_macos_sys():
        app_name = PACKAGE_NAME + '.app'
        app_dir_path_relative = 'Contents/Resources/app'
    app_path_relative = os.path.join(app_name, app_dir_path_relative)

    new_icns_name_mac = ''

    cwd = os.getcwd()

    # Update modules.
    log_stage('Update modules...')
    run_cmd('npm install')

    # Extract old package and remove old app.
    if not os.path.exists(DIST_DIR):
        os.mkdir(DIST_DIR)

    log_stage('No extract old package and copy from electron dist...')
    electron_node_module_dir = 'node_modules/electron'
    electron_dist_dir = 'dist'
    os.chdir(electron_node_module_dir)
    if is_windows_sys():
        electron_dist_package = 'dist.7z'
        run_cmd('%s -t7z a %s %s' % (exe_7z_sys, electron_dist_package, electron_dist_dir))
        os.chdir(cwd)
        electron_dist_package_path = os.path.join(electron_node_module_dir, electron_dist_package)
        copy_file(electron_dist_package_path, os.path.join(DIST_DIR, electron_dist_package))
        remove_file(electron_dist_package_path)
        os.chdir(DIST_DIR)
        run_cmd('%s -t7z x %s' % (exe_7z_sys, electron_dist_package))
        remove_file(electron_dist_package)
        os.rename(electron_dist_dir, app_name)
    elif is_macos_sys():
        os.chdir(electron_dist_dir)
        electron_dist_app_name = 'Electron.app'
        electron_dist_package = '%s.tar.%s' % (electron_dist_app_name, tar_ext)
        run_cmd('tar %s %s %s' % (tar_param, electron_dist_package, electron_dist_app_name))
        os.chdir(cwd)
        electron_dist_package_path = os.path.join(electron_node_module_dir, electron_dist_dir, electron_dist_package)
        copy_file(electron_dist_package_path, os.path.join(DIST_DIR, electron_dist_package))
        remove_file(electron_dist_package_path)
        os.chdir(DIST_DIR)
        run_cmd('tar -xvf %s' % (electron_dist_package))
        remove_file(electron_dist_package)
        os.rename(electron_dist_app_name, app_name)
    elif is_linux_sys():
        electron_dist_package = 'dist.tar.%s' % (tar_ext)
        run_cmd('tar %s %s %s' % (tar_param, electron_dist_package, electron_dist_dir))
        os.chdir(cwd)
        electron_dist_package_path = os.path.join(electron_node_module_dir, electron_dist_package)
        copy_file(electron_dist_package_path, os.path.join(DIST_DIR, electron_dist_package))
        remove_file(electron_dist_package_path)
        os.chdir(DIST_DIR)
        run_cmd('tar -xvf %s' % (electron_dist_package))
        remove_file(electron_dist_package)
        os.rename(electron_dist_dir, app_name)
    else:
        log_stage('Not supported system.')
        exit()
    os.chdir(cwd)

    os.chdir(DIST_DIR)
    os.mkdir(app_path_relative)
    os.chdir(cwd)

    # Copy new app.
    log_stage('Copy new app...')
    for app_dir in APP_DIRS:
        dest_app_dir = os.path.join(DIST_DIR, app_path_relative)
        dest_app_dir = os.path.join(dest_app_dir, app_dir)
        os.mkdir(dest_app_dir)
        copy_dir(app_dir, dest_app_dir)

    for app_file in APP_FILES:
        dest_app_file = os.path.join(DIST_DIR, app_path_relative)
        dest_app_file = os.path.join(dest_app_file, app_file)
        copy_file(app_file, dest_app_file)

    release_file = os.path.join(DIST_DIR, app_path_relative, 'assets', 'RELEASED')
    open(release_file, 'a').close()

    # Rebuild and clean.
    log_stage('Rebuild and clean...')
    os.chdir(os.path.join(DIST_DIR, app_path_relative))
    if is_macos_sys() or is_linux_sys():
        for exec_file in EXEC_FIX_PATHS:
            st = os.stat(exec_file)
            os.chmod(exec_file, st.st_mode | stat.S_IEXEC)
    run_cmd('npm rebuild')
    if REBUILD_CMD:
        run_cmd('npm run %s' % (REBUILD_CMD))
    remove_dir('./node_modules/electron/dist')
    rebuild_clean_paths = REBUILD_CLEAN_PATHS_WIN
    if is_macos_sys():
        rebuild_clean_paths = REBUILD_CLEAN_PATHS_MACOS
    if is_linux_sys():
        rebuild_clean_paths = REBUILD_CLEAN_PATHS_LINUX
    for rebuild_clean_path in rebuild_clean_paths:
        remove_dir(rebuild_clean_path)
    os.chdir(cwd)

    # Update icon.
    log_stage('Update icon...')
    if is_windows_sys() and RESOURCE_HACKER_SCRIPT != '':
        run_cmd('ResourceHacker.exe -script %s' % (RESOURCE_HACKER_SCRIPT))
    elif is_macos_sys() and APP_ICNS_MACOS != '':
        electron_default_icns_path = os.path.join(DIST_DIR, app_name, 'Contents/Resources/electron.icns')
        new_icns_name_mac = APP_TITLE + '.icns'
        new_icns_path = os.path.join(DIST_DIR, app_name, 'Contents/Resources', new_icns_name_mac)
        remove_file(electron_default_icns_path)
        copy_file(APP_ICNS_MACOS, new_icns_path)

    # Rename electron files.
    log_stage('Rename electron files...')
    os.chdir(os.path.join(DIST_DIR, app_name))
    electron_exe_dir = ''
    electron_exe_name = ''
    electron_exe_app_name = ''
    if is_windows_sys():
        electron_exe_dir = './'
        electron_exe_name = 'electron.exe'
        electron_exe_app_name = '%s%s.exe' % (electron_exe_dir, APP_TITLE)
    elif is_macos_sys():
        electron_exe_dir = './Contents/MacOS/'
        electron_exe_name = 'Electron'
        electron_exe_app_name = '%s%s' % (electron_exe_dir, APP_TITLE)
    elif is_linux_sys():
        electron_exe_dir = './'
        electron_exe_name = 'electron'
        electron_exe_app_name = '%s%s' % (electron_exe_dir, APP_TITLE)
    electron_exe_path = '%s%s' % (electron_exe_dir, electron_exe_name)
    if os.path.exists(electron_exe_path):
        os.rename(electron_exe_path, electron_exe_app_name)

    # Update Info.plist.
    if is_macos_sys():
        info_plist_path = './Contents/Info.plist'
        info_plist_content = read_file_binary(info_plist_path)
        info_plist_content = info_plist_content.replace(b'>Electron<',
                                bytes('>%s<' % (APP_TITLE), encoding='utf8'))
        info_plist_content = info_plist_content.replace(b'>com.github.Electron<',
                                bytes('>%s<' % (APP_BUNDLE_ID_MACOS), encoding='utf8'))
        if new_icns_name_mac != '':
            info_plist_content = info_plist_content.replace(b'>electron.icns<',
                                    bytes('>%s<' % (new_icns_name_mac), encoding='utf8'))
        write_file_binary(info_plist_path, info_plist_content)

    os.chdir(cwd)

    # Package and clean up.
    log_stage('Package and clean up...')
    os.chdir(DIST_DIR)
    app_package_file = '%s.%s.%s' % (app_name, system_name, machine_name)
    if is_windows_sys():
        app_package_file = '%s.7z' % (app_package_file)
        remove_file(app_package_file)
        run_cmd('%s -t7z -mx9 a %s %s' % (exe_7z_sys, app_package_file, app_name))
    else:
        app_package_file = '%s.tar.%s' % (app_package_file, tar_ext)
        remove_file(app_package_file)
        run_cmd('tar %s %s %s' % (tar_param, app_package_file, app_name))
    remove_dir(app_name)

if __name__ == '__main__':
    main()
