import binascii
import getpass
import os
import shutil

def list_file(path):
    files = [f for f in os.listdir(path) if os.path.isfile(os.path.join(path, f))]
    return files

def copy_file(source, dest):
    shutil.copyfile(source, dest)

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

def byte_equal(some_byte, value):
    hex_value = binascii.hexlify(some_byte)
    dev_value = int(hex_value, 16)
    return (dev_value == value)

def detect_jpeg_file(file):
    some_file = open(file, "rb")
    with some_file:
        some_bytes = some_file.read(2)
        if len(some_bytes) == 2 and (some_bytes[0] == 0xff) and (some_bytes[1] == 0xd8):
            #print(file)
            return True
    return False

def main():
    username = getpass.getuser()
    spotlight_wallpaper_dir = os.path.join('C:\\Users', username)
    spotlight_wallpaper_dir = os.path.join(spotlight_wallpaper_dir, 'AppData\\Local\\Packages\\Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy\\LocalState\\Assets')
    print(spotlight_wallpaper_dir)

    cwd = os.getcwd()
    wallpaper_temp_dir = os.path.join(cwd, 'spotlight_wallpapers')
    print(wallpaper_temp_dir)
    if os.path.exists(wallpaper_temp_dir):
        remove_dir(wallpaper_temp_dir)
    os.mkdir(wallpaper_temp_dir)

    copy_dir(spotlight_wallpaper_dir, wallpaper_temp_dir)

    #print(os.listdir(wallpaper_temp_dir))

    files = list_file(wallpaper_temp_dir)
    for file in files:
        file = os.path.join(wallpaper_temp_dir, file)
        if detect_jpeg_file(file):
            new_name = file + '.jpg'
            #print(file)
            #print(new_name)
            os.rename(file, new_name)
        else:
            remove_file(file)


if __name__ == '__main__':
    main()
